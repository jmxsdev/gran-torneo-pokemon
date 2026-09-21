#!/usr/bin/env bash
# run_tests.sh — Batería completa de pruebas del Gran Torneo Pokémon (F9).
#
# Gate de fase RF-PRB-01 (diseño §8.2): compila el proyecto con
# gcc -std=c99 -Wall -Wextra (cero warnings como gate), ejecuta cada caso de
# tests/casos/ con stdin scriptado y diff contra su .esperado (byte a byte,
# UTF-8), y compila+ejecuta los probes de tests/probes/ (efectividad exacta
# y torneo completo 32→48→clasificación→49–64→campeón).
#
# Estructura de un caso: 1 archivo .in (guion stdin completo que navega el
# menú) + 1 archivo .esperado (stdout esperado completo). Cada caso corre en
# un directorio temporal aislado con una copia de data/, de modo que los
# datos del repositorio NUNCA se modifican. Convención por sufijo del nombre:
#   *_sin_pokedex      -> el sandbox arranca sin data/pokedex.txt
#   *_sin_entrenadores -> el sandbox arranca sin data/entrenadores.txt
#   (cualquier otro caso usa los 4 archivos de data/ tal cual)
#
# Uso: bash tests/run_tests.sh   (desde la raíz del repositorio)
# Salida: un PASS/FALLA por caso y probe; resumen final con código de salida
# 0 si y solo si todo pasó.

set -u

REPO="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$REPO" || exit 1

BIN="$REPO/build/torneo"
CASOS="$REPO/tests/casos"
PROBES="$REPO/tests/probes"
SRC_MODULOS="src/archivos.c src/combate.c src/entrenador.c src/equipo.c \
src/pokedex.c src/resultados.c src/tipos.c src/torneo.c src/validacion.c"

TMP_BASE="$(mktemp -d)"
trap 'rm -rf "$TMP_BASE"' EXIT

PASS=0
FAIL=0

ok()   { PASS=$((PASS + 1)); echo "PASS: $1"; }
falla() { FAIL=$((FAIL + 1)); echo "FALLA: $1"; }

echo "=== Gate: compilación (make clean && make, cero warnings) ==="
if ! { make clean && make > "$TMP_BASE/build.log" 2>&1; }; then
    echo "FALLA: make falló"
    tail -20 "$TMP_BASE/build.log"
    exit 1
fi
if grep -iE "warning|error" "$TMP_BASE/build.log" >/dev/null 2>&1; then
    echo "FALLA: build con warnings (gate RF-TEC-02)"
    grep -iE "warning|error" "$TMP_BASE/build.log"
    exit 1
fi
ok "build: make limpio, cero warnings (todos los .c de src/)"

echo ""
echo "=== Casos de menú (tests/casos/*.in → diff contra *.esperado) ==="
N_CASOS=0
for in_file in "$CASOS"/*.in; do
    [ -e "$in_file" ] || continue
    N_CASOS=$((N_CASOS + 1))
    nombre="$(basename "$in_file" .in)"
    sandbox="$TMP_BASE/caso-$nombre"
    salida="$sandbox/salida.txt"
    mkdir -p "$sandbox/data"

    # Sandbox aislado: copia de data/ con la convención por sufijo.
    cp "$REPO"/data/*.txt "$sandbox/data/" 2>/dev/null || true
    case "$nombre" in
        *_sin_pokedex)      rm -f "$sandbox/data/pokedex.txt" ;;
        *_sin_entrenadores) rm -f "$sandbox/data/entrenadores.txt" ;;
    esac

    # Ejecución con stdin scriptado; timeout de seguridad anti-cuelgue.
    if ! (cd "$sandbox" && timeout 60 "$BIN" < "$in_file" > "$salida" 2>&1); then
        falla "$nombre: ejecución fallida o timeout"
        continue
    fi
    if diff -u "$CASOS/$nombre.esperado" "$salida" > "$TMP_BASE/diff.txt" 2>&1; then
        ok "$nombre"
    else
        falla "$nombre: diff contra .esperado"
        head -10 "$TMP_BASE/diff.txt"
    fi
done
[ "$N_CASOS" -gt 0 ] || { echo "FALLA: no hay casos en tests/casos/"; exit 1; }

echo ""
echo "=== Probes de módulos (tests/probes/) ==="
for probe in probe_efectividad probe_torneo_completo; do
    exe="$TMP_BASE/$probe"
    if ! gcc -std=c99 -Wall -Wextra -I src -o "$exe" \
         $SRC_MODULOS "$PROBES/$probe.c" 2> "$TMP_BASE/$probe-build.log"; then
        falla "$probe: no compila"
        tail -10 "$TMP_BASE/$probe-build.log"
        continue
    fi
    if grep -iE "warning|error" "$TMP_BASE/$probe-build.log" >/dev/null 2>&1; then
        falla "$probe: build con warnings"
        continue
    fi
    if "$exe" > "$TMP_BASE/$probe.out" 2>&1; then
        ok "$probe: ejecución exitosa, 0 fallos"
    else
        falla "$probe: comprobaciones fallidas"
        grep -E "^FAIL:" "$TMP_BASE/$probe.out" | head -10
    fi
done

echo ""
echo "BATERÍA F9: $PASS PASS, $FAIL FALLA ($N_CASOS casos de menú + 2 probes)"
[ "$FAIL" -eq 0 ]
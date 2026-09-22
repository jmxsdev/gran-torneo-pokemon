#!/usr/bin/env bash
# verificar_entrega.sh — Verifica el paquete de entrega del Gran Torneo Pokémon (F11.2).
#
# Extrae dist/gran-torneo-pokemon-entrega.zip a un directorio temporal LIMPIO
# (ajeno al repositorio) y prueba que el paquete compila y ejecuta por sí solo,
# sin depender de archivos externos no entregados (sección 7 del enunciado):
#
#   1. Estructura: el paquete contiene src/ (main.c incluido), data/ (Pokédex),
#      tests/, docs/, Makefile, Doxyfile, README.md y openspec/.
#   2. Compilación: make clean && make con gcc -std=c99 -Wall -Wextra
#      (gate de cero warnings).
#   3. Prueba de humo: menú -> opción 1 (Pokédex: buscar el 25) -> salir (12),
#      exit code 0 y salida con "Pikachu" y "Saliendo del programa.".
#   4. Batería completa: bash tests/run_tests.sh dentro del paquete
#      (23 PASS, 0 FALLA).
#   5. Sin dependencias externas: rutas de datos relativas (data/) y sin rutas
#      absolutas en el código fuente ni en el Makefile.
#
# Uso: bash scripts/verificar_entrega.sh   (desde la raíz del repositorio)
# Salida: PASS/FALLA por gate; exit 0 si y solo si todo pasó.

set -u

REPO="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
ZIP="$REPO/dist/gran-torneo-pokemon-entrega.zip"

PASS=0
FAIL=0
ok()    { PASS=$((PASS + 1)); echo "PASS: $1"; }
falla() { FAIL=$((FAIL + 1)); echo "FALLA: $1"; }

if [ ! -f "$ZIP" ]; then
    echo "FALLA: no existe $ZIP — ejecute primero: make entrega"
    exit 1
fi

TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

echo "=== 0. Extracción a directorio temporal limpio ==="
if ! unzip -q "$ZIP" -d "$TMP"; then
    echo "FALLA: no se pudo extraer el zip (unzip)."
    exit 1
fi
PAQUETE="$TMP/gran-torneo-pokemon-entrega"
[ -d "$PAQUETE" ] || { echo "FALLA: el zip no contiene la raíz gran-torneo-pokemon-entrega/."; exit 1; }
ok "zip extraído en $TMP (directorio ajeno al repositorio)"

echo ""
echo "=== 1. Estructura del paquete ==="
ESTRUCTURA_OK=1
for ruta in src/main.c data/pokedex.txt data/efectividad.txt data/entrenadores.txt \
            tests/run_tests.sh docs/planificacion.md docs/informe-tecnico.md \
            Makefile Doxyfile README.md openspec/changes/gran-torneo-pokemon/tasks.md; do
    if [ -f "$PAQUETE/$ruta" ]; then
        echo "  OK   $ruta"
    else
        echo "  FALTA $ruta"
        ESTRUCTURA_OK=0
    fi
done
if [ "$ESTRUCTURA_OK" -eq 1 ]; then ok "estructura completa (código, datos, docs, tests, trazabilidad SDD)"; else falla "estructura incompleta"; fi

cd "$PAQUETE" || exit 1

echo ""
echo "=== 2. Compilación: make clean && make (cero warnings) ==="
if { make clean && make > "$TMP/build.log" 2>&1; }; then
    if grep -iE "warning|error" "$TMP/build.log" >/dev/null 2>&1; then
        falla "compilación con warnings"
        grep -iE "warning|error" "$TMP/build.log" | head -10
    else
        ok "make limpio, cero warnings (gcc -std=c99 -Wall -Wextra)"
    fi
else
    falla "make falló"
    tail -20 "$TMP/build.log"
fi

echo ""
echo "=== 3. Prueba de humo: menú -> opción 1 (Pokédex) -> salir (12) ==="
# Entrada: 1 (consultar Pokédex), 2 (buscar por número), 25 (Pikachu),
#          0 (volver al menú), 12 (salir).
if printf '1\n2\n25\n0\n12\n' | timeout 60 ./build/torneo > "$TMP/humo.txt" 2>&1; then
    if grep -q "025 Pikachu" "$TMP/humo.txt" && grep -q "Saliendo del programa." "$TMP/humo.txt"; then
        ok "humo: exit 0, Pokédex responde (025 Pikachu) y salida ordenada por 12"
    else
        falla "humo: salida sin la Pokédex o sin la despedida esperada"
        grep -E "Pikachu|Saliendo" "$TMP/humo.txt" || tail -5 "$TMP/humo.txt"
    fi
else
    falla "humo: ejecución fallida o timeout"
    tail -10 "$TMP/humo.txt"
fi

echo ""
echo "=== 4. Batería completa: bash tests/run_tests.sh ==="
if bash tests/run_tests.sh > "$TMP/bateria.log" 2>&1; then
    if grep -q "23 PASS, 0 FALLA" "$TMP/bateria.log"; then
        ok "batería: BATERÍA F9: 23 PASS, 0 FALLA (20 casos de menú + 2 probes)"
    else
        falla "batería: exit 0 pero sin el resumen esperado"
        tail -5 "$TMP/bateria.log"
    fi
else
    falla "batería: run_tests.sh falló"
    tail -15 "$TMP/bateria.log"
fi

echo ""
echo "=== 5. Sin dependencias externas (rutas relativas) ==="
if grep -q '^#define RUTA_POKEDEX "data/pokedex.txt"' src/constantes.h \
   && grep -q '^#define RUTA_ENTRENADORES "data/entrenadores.txt"' src/constantes.h; then
    ok "rutas de datos relativas en constantes.h (data/...)"
else
    falla "rutas de datos no relativas en constantes.h"
fi
RUTAS_ABS="$(grep -rnE '"/(home|tmp|usr|etc|opt)/' src Makefile 2>/dev/null || true)"
if [ -z "$RUTAS_ABS" ]; then
    ok "sin rutas absolutas en src/ ni en el Makefile"
else
    falla "rutas absolutas encontradas en el código o Makefile:"
    echo "$RUTAS_ABS"
fi

echo ""
echo "VERIFICACIÓN DE ENTREGA: $PASS PASS, $FAIL FALLA"
[ "$FAIL" -eq 0 ]
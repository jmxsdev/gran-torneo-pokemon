#!/usr/bin/env bash
# empaquetar.sh — Genera el paquete de entrega del Gran Torneo Pokémon (F11.1).
#
# Produce dist/gran-torneo-pokemon-entrega.zip: un único archivo comprimido con
# todo lo necesario para compilar y ejecutar el programa sin depender de
# archivos externos no entregados (sección 7 del enunciado):
#
#   src/        Código fuente: todos los .c/.h, incluido main.c
#   data/       Archivo de la Pokédex (pokedex.txt) y demás datos .txt
#   tests/      Batería scriptada (casos/, probes/, run_tests.sh)
#   docs/       Documentación breve (planificación, informe, convenciones)
#               SIN el html generado por Doxygen (docs/doxygen/)
#   openspec/   Trazabilidad SDD (propuesta, specs, diseño, tareas, verificación)
#   scripts/    Este propio script (y el de verificación de entrega)
#   Makefile  Doxyfile  README.md
#
# Reproducible: trabaja sobre una copia de staging en un directorio temporal y
# empaqueta con la raíz gran-torneo-pokemon-entrega/ (todas las rutas internas
# son relativas). El zip se regenera en cada ejecución; dist/ NO se versiona
# (está en .gitignore).
#
# Uso: bash scripts/empaquetar.sh   (o: make entrega)
# Salida: dist/gran-torneo-pokemon-entrega.zip + lista y resumen del contenido.

set -u

REPO="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$REPO" || exit 1

NOMBRE="gran-torneo-pokemon-entrega"
DIST_DIR="$REPO/dist"
ZIP="$DIST_DIR/$NOMBRE.zip"

# --- Prerequisitos: todo lo que debe empaquetarse existe en el repositorio ---
FALTAN=""
for ruta in src data tests docs openspec scripts Makefile Doxyfile README.md; do
    [ -e "$ruta" ] || FALTAN="$FALTAN $ruta"
done
if [ -n "$FALTAN" ]; then
    echo "ERROR: faltan elementos obligatorios del paquete:$FALTAN"
    echo "       Ejecute el script desde la raíz del repositorio."
    exit 1
fi

# --- Staging: copia limpia del contenido (el zip nunca se arma en el árbol) ---
STAGING="$(mktemp -d)"
trap 'rm -rf "$STAGING"' EXIT
RAIZ="$STAGING/$NOMBRE"
mkdir -p "$RAIZ"

cp -r src data tests openspec scripts "$RAIZ/"
mkdir -p "$RAIZ/docs"
cp docs/*.md "$RAIZ/docs/"
cp Makefile Doxyfile README.md "$RAIZ/"

# docs/doxygen/ es salida regenerable (make no la usa; se excluye del paquete).
rm -rf "$RAIZ/docs/doxygen"

# --- Empaquetado: zip si está disponible; si no, fallback con Python (zipfile) ---
rm -rf "$DIST_DIR"
mkdir -p "$DIST_DIR"

if command -v zip >/dev/null 2>&1; then
    (cd "$STAGING" && zip -qr "$ZIP" "$NOMBRE") || {
        echo "ERROR: falló la creación del zip (comando zip)."
        exit 1
    }
else
    (cd "$STAGING" && python3 -m zipfile -c "$ZIP" "$NOMBRE") || {
        echo "ERROR: falló la creación del zip (python3 -m zipfile)."
        echo "       Instale 'zip' o asegure python3 en el PATH."
        exit 1
    }
fi

# --- Integridad y contenido ---
if command -v unzip >/dev/null 2>&1; then
    unzip -t "$ZIP" >/dev/null || { echo "ERROR: el zip no pasó la prueba de integridad."; exit 1; }
else
    python3 -m zipfile -t "$ZIP" >/dev/null || { echo "ERROR: el zip no pasó la prueba de integridad."; exit 1; }
fi

echo "Paquete generado: $ZIP"
echo ""
echo "=== Contenido del paquete ==="
if command -v unzip >/dev/null 2>&1; then
    unzip -l "$ZIP" | sed 's/^/  /'
else
    python3 -m zipfile -l "$ZIP" | sed 's/^/  /'
fi

C_C=$(find "$RAIZ" -name '*.c' | wc -l)
C_H=$(find "$RAIZ" -name '*.h' | wc -l)
C_TXT=$(find "$RAIZ" -name '*.txt' | wc -l)
C_TOTAL=$(find "$RAIZ" -type f | wc -l)
TAMANO=$(du -h "$ZIP" | cut -f1)

echo ""
echo "=== Resumen ==="
echo "  Archivos .c : $C_C"
echo "  Archivos .h : $C_H"
echo "  Archivos .txt: $C_TXT"
echo "  Archivos totales: $C_TOTAL"
echo "  Tamaño del zip: $TAMANO"
echo ""
echo "Verificación de la entrega: bash scripts/verificar_entrega.sh"
echo "Reconstruir el paquete:      make entrega  (o bash scripts/empaquetar.sh)"
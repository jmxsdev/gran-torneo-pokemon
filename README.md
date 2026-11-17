# El Gran Torneo Pokémon

Sistema de consola en C (ISO C99) que administra el Gran Torneo Pokémon de la
Liga: carga las 150 especies de la primera generación desde archivo, registra
entrenadores con equipos de ejemplares propios, simula combates con
efectividad de tipos, organiza la fase de grupos round-robin con su
clasificación y resuelve la fase eliminatoria hasta coronar al campeón.

Proyecto final de **Programación I / Algoritmos I** (Universidad de Carabobo).

## Compilar y ejecutar

Requisitos: `gcc` y `make`.

```bash
make          # compila con gcc -std=c99 -Wall -Wextra (cero warnings)
make run      # ejecuta el programa
```

## Funcionalidades

- **Pokédex inmutable**: las 150 especies de la primera generación se leen de
  `data/pokedex.txt` (número, nombre, tipos y estadísticas base). El código no
  contiene datos de especies.
- **Consultas**: mostrar la Pokédex completa, buscar por número y por nombre,
  y consultar la ficha de una especie.
- **Efectividad de tipos**: matriz 18×18 cargada desde `data/efectividad.txt`,
  con respaldo embebido si el archivo falta.
- **Entrenadores y ejemplares**: separación especie/ejemplar, identificadores
  únicos y equipos de hasta 6 ejemplares con estadísticas derivadas.
- **Formación automática de equipos**: recursividad y backtracking con podas.
- **Simulación automática del torneo**: con solo cargar los entrenadores, el
  sistema rellena por backtracking los equipos que falten y juega los 64
  combates hasta coronar al campeón.
- **Torneo completo**: fase de grupos (combates 1–48), clasificación con
  desempates, eliminatorias (49–64), tercer lugar y final.
- **Persistencia**: entrenadores y resultados se guardan en `data/`.
- **Menú tolerante**: las entradas inválidas no terminan el programa.

## Estructura del repositorio

```
src/      Código fuente (.c/.h), incluido main.c
data/     Pokédex y datos del torneo (.txt)
tests/    Batería scriptada (casos/, probes/, run_tests.sh)
scripts/  Empaquetado y verificación de la entrega
Makefile  Doxyfile  README.md
```

## Pruebas

```bash
bash tests/run_tests.sh   # compila, ejecuta los casos y compara la salida
```

## Entrega

```bash
make entrega                       # genera dist/gran-torneo-pokemon-entrega.zip
bash scripts/verificar_entrega.sh  # comprueba el paquete en un directorio limpio
```

# El Gran Torneo Pokémon

Sistema de consola en C (ISO C99) que administra el Gran Torneo Pokémon de la
Liga: carga las 150 especies de la primera generación desde archivo, registra
entrenadores con equipos de ejemplares propios, simula combates con
efectividad de tipos, organiza la fase de grupos round-robin con su
clasificación y resuelve la fase eliminatoria hasta coronar al campeón.

Proyecto final de **Programación I / Algoritmos I** (Universidad de Carabobo).

**Repositorio**: https://github.com/jmxsdev/gran-torneo-pokemon/tree/main

## Compilar y ejecutar

Requisitos: `gcc` y `make`.

```bash
make          # compila con gcc -std=c99 -Wall -Wextra (cero warnings)
make run      # ejecuta el programa
```

Al iniciar, el programa carga la Pokédex y (si existe) el archivo de
entrenadores; desde el menú se registran entrenadores, se arman equipos, se
cargan resultados o se simula el torneo completo.

## Funcionalidades

- **Pokédex inmutable**: las 150 especies se leen de `data/pokedex.txt`
  (número, nombre, tipos y estadísticas base). El código no contiene datos de
  especies.
- **Consultas**: mostrar la Pokédex completa, buscar por número y por nombre,
  y consultar la ficha de una especie.
- **Efectividad de tipos**: matriz 18×18 cargada desde `data/efectividad.txt`.
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

## Módulos

`main` solo orquesta el flujo; la lógica vive en módulos con responsabilidad
única. Las dependencias apuntan "hacia abajo" (sin ciclos entre cabeceras).

| Módulo | Responsabilidad | Depende de |
|---|---|---|
| `main.c` | Menú de 12 opciones, carga inicial y despacho | todos (solo orquesta) |
| `pokedex.c` | Carga y consulta de las 150 especies | `tipos` |
| `tipos.c` | Catálogo de 18 tipos y matriz de efectividad | — |
| `entrenador.c` | Registro con id único, búsqueda y listado | `equipo` |
| `equipo.c` | Ejemplares, validación de equipos y backtracking | `pokedex`, `entrenador` |
| `combate.c` | Reglamento, fórmula de daño, orden y desempate | `equipo`, `tipos` |
| `torneo.c` | Grupos, calendario, clasificación, bracket y campeón | `entrenador`, `pantalla` |
| `resultados.c` | Carga y validación de resultados | `torneo`, `validacion` |
| `archivos.c` | Entrada/salida de los archivos de datos | `pokedex`, `entrenador`, `torneo` |
| `validacion.c` | Lectura segura de entradas con reintentos y EOF | — |
| `simulacion.c` | Simulación automática y relleno de equipos | `torneo`, `combate`, `entrenador`, `equipo` |
| `pantalla.c` | Utilidades de presentación (padding UTF-8) | — |

## Algoritmos clave

- **Estadísticas del ejemplar**: se derivan de la especie con una variación
  determinista por identificador:
  `variacion = (id * 7) % 16`;
  `HP = hp_base * nivel / 50 + nivel + 10 + variacion`;
  `ataque/defensa/velocidad = stat_base * nivel / 50 + nivel + 5 + variacion`.
- **Fórmula de daño**: `base = (2*nivel/5 + 2) * ataque / defensa + 2`,
  multiplicada por la efectividad del tipo atacante contra los dos tipos del
  defensor. El daño mínimo es 1 si hay efecto y 0 si es inmune.
- **Orden de ataque**: ataca primero el Pokémon con mayor velocidad; si hay
  empate, ataca el del entrenador 1.
- **Fin del combate**: se juega hasta que un entrenador no tenga Pokémon
  disponibles. En grupos, 20 turnos sin KO pueden terminar en empate; en
  eliminatoria se desempata por HP total, luego nivel total y, por último, el
  entrenador 1.
- **Backtracking de equipos**: explora combinaciones de especies y retrocede
  con podas de cantidad, cota inferior de nivel, cota de tipos con máscara de
  bits y restricciones de integrantes.
- **Torneo**: 32 entrenadores → 8 grupos de 4 → round-robin (6 combates por
  grupo, 1–48) → clasifican los dos primeros de cada grupo (16) → bracket
  49–64 (octavos, cuartos, semifinal, tercer lugar y final).
- **Simulación**: rellena los equipos faltantes con backtracking (nivel medio
  y ventana de especies rotativa por posición) y juega los combates pendientes
  en modo silencioso, aplicando cada resultado al estado del torneo.

## Archivos de datos

| Archivo | Contenido |
|---|---|
| `data/pokedex.txt` | 150 especies: número, nombre, tipos y estadísticas base |
| `data/efectividad.txt` | Matriz de efectividad 18×18 |
| `data/entrenadores.txt` | Entrenadores y sus equipos |
| `data/resultados.txt` | Resultados de los combates (admite parciales) |
| `data/clasificacion.txt` | Clasificación generada por el sistema |

## Estructura del repositorio

```
src/      Código fuente (.c/.h), incluido main.c
data/     Pokédex y datos del torneo (.txt)
tests/    Batería scriptada (casos/, probes/, run_tests.sh)
Makefile  Doxyfile  README.md
```

## Pruebas

```bash
bash tests/run_tests.sh   # compila, ejecuta los casos y compara la salida
```

La batería incluye casos de menú (entrada y salida esperada comparadas byte a
byte) y pruebas de módulo (efectividad de tipos y torneo completo).

# El Gran Torneo Pokémon

Sistema de consola en C (ISO C99) que administra íntegramente el Gran Torneo Pokémon de la Liga: carga las 150 especies de la primera generación desde archivo, registra entrenadores con equipos de ejemplares propios, simula combates con efectividad de tipos, organiza la fase de grupos round-robin con su clasificación y resuelve la fase eliminatoria hasta coronar al campeón.

Es el proyecto final de **Programación I / Algoritmos I** (Universidad de Carabobo): su propósito es demostrar el dominio de funciones, estructuras, TDAs, manejo de archivos, cadenas, recursividad y backtracking en C, con un diseño modular y defendible.

## Ruta rápida

1. **Requisitos**: `gcc` y `make` instalados.
2. **Compilar**:
   ```bash
   make
   ```
   La compilación exige cero warnings (`-std=c99 -Wall -Wextra`).
3. **Ejecutar**:
   ```bash
   make run
   ```
4. **Resultado esperado**: el menú principal del torneo con 12 opciones; la opción 1 permite consultar la Pokédex completa (mostrar todas, buscar por número o por nombre).

## Entrega (F11)

El proyecto se entrega en un único archivo comprimido (sección 7 del enunciado):

```bash
make entrega            # o: bash scripts/empaquetar.sh
bash scripts/verificar_entrega.sh   # verificación completa del paquete
```

`make entrega` genera `dist/gran-torneo-pokemon-entrega.zip` (NO versionado:
`dist/` está en `.gitignore`) con `src/`, `data/`, `tests/`, `docs/` (sin el
html de Doxygen), `openspec/`, `scripts/`, `Makefile`, `Doxyfile` y `README.md`.
El verificador extrae el zip a un directorio temporal limpio, compila con cero
warnings, ejecuta una prueba de humo del menú y corre la batería completa
(23 PASS, 0 FALLA) — comprobando que el paquete no depende de archivos externos.

## Estado del proyecto

Desarrollo guiado por un plan de 14 días dividido en 12 fases (ver [docs/planificacion.md](docs/planificacion.md)) — **plan 14/14 cerrado**:

| Fase | Alcance | Estado |
|------|---------|--------|
| F0 | Convenciones Doxygen, planificación, informe técnico, esqueleto modular y menú | ✅ Hecha |
| F1 | Tipos y matriz de efectividad, módulo Pokédex, catálogo de 150 especies | ✅ Hecha |
| F2 | Entrenadores, ejemplares y equipos | ✅ Hecha |
| F3 | Motor de combate (daño, turnos, KO) | ✅ Hecha |
| F4 | Backtracking: construcción automática de equipos | ✅ Hecha |
| F5 | Fase de grupos y puntuación | ✅ Hecha |
| F6 | Fase eliminatoria y campeón | ✅ Hecha |
| F7 | Persistencia de resultados y entrenadores | ✅ Hecha |
| F8 | Validación integral de entradas | ✅ Hecha |
| F9 | Pruebas completas (batería scriptada versionada) | ✅ Hecha |
| F10 | Documentación final (informe técnico, plan 14/14, Doxygen) | ✅ Hecha |
| F11 | Entrega: empaquetado, checklist y cierre | ✅ Hecha |

## Pruebas

El proyecto incluye una batería de pruebas scriptada versionada (RF-PRB-01):
20 casos de menú (`stdin` → `stdout`, diff byte a byte contra el esperado, en
un sandbox aislado) más 2 probes de módulos (efectividad exacta y torneo
completo 32 → 48 → clasificación → 49–64 → campeón). Para ejecutarla:

```bash
bash tests/run_tests.sh
```

El ejecutor compila primero con `gcc -std=c99 -Wall -Wextra` (aborta si hay
warnings) y termina con `BATERÍA F9: 23 PASS, 0 FALLA` y código de salida 0.
Detalle de la cobertura en [docs/informe-tecnico.md](docs/informe-tecnico.md) §6.

## Características actuales

- **Pokédex inmutable**: las 150 especies de la primera generación se leen de `data/pokedex.txt` (número, nombre, tipos, estadísticas base). El código fuente no contiene datos de especies.
- **Consultas**: mostrar la Pokédex completa, buscar por número y por nombre (búsqueda insensible a mayúsculas y acentos).
- **Efectividad de tipos**: matriz 18×18 cargada desde `data/efectividad.txt`, con respaldo embebido si el archivo falta.
- **Ejemplares y equipos**: separación especie/ejemplar, estadísticas derivadas por fórmula determinista (D2), equipos de hasta 6 como lista enlazada (TDA).
- **Formación automática de equipos**: recursividad + backtracking con podas (RF-EQP-04).
- **Torneo completo**: fase de grupos round-robin (combates 1–48), clasificación por desempates, bracket eliminatorio 49–64 y campeón; los participantes los resuelve el sistema (RF-RES-03).
- **Menú resistente**: entradas inválidas u opciones inexistentes no terminan el programa (RF-TEC-03).

## Estructura del repositorio

```
├── src/                  Código fuente (módulos con cabeceras .h)
│   ├── main.c            Flujo general y menú (sin lógica de negocio)
│   ├── constantes.h      Constantes del sistema
│   └── *.h               Interfaces de módulos: pokedex, tipos, entrenador,
│                         equipo, combate, torneo, resultados, archivos, validación
├── data/                 Datos del sistema (Pokédex, efectividad, entrenadores, resultados)
├── tests/                Batería scriptada: casos/ (entrada+esperado), probes/, run_tests.sh
├── docs/                 Planificación, convenciones Doxygen e informe técnico
├── openspec/             Artefactos SDD (propuesta, especificaciones, diseño, tareas)
├── Doxyfile              Configuración de documentación Doxygen (salida en español)
└── Makefile              Compilación con gate de cero warnings
```

## Documentación del proyecto

- [Plan de desarrollo y control de avance](docs/planificacion.md) — qué está hecho y qué queda pendiente (14/14).
- [Informe técnico explicativo](docs/informe-tecnico.md) — decisiones D1–D10 justificadas, fórmulas, algoritmos y diagramas finales para la defensa oral.
- [Convenciones de documentación Doxygen](docs/convenciones-doxygen.md) — reglas de documentación del código en español.
- [Artefactos SDD](openspec/changes/gran-torneo-pokemon/) — propuesta, especificaciones con escenarios, diseño técnico, tareas y reportes de verificación.

## Proyecto académico

Este repositorio documenta el trabajo de curso de la asignatura Programación I (Facultad Experimental de Ciencias y Tecnología, Universidad de Carabobo). El desarrollo sigue un flujo de trabajo spec-driven (SDD): cada fase nace de una especificación, se implementa de forma modular y se verifica con pruebas scriptadas antes de marcarse como completada.
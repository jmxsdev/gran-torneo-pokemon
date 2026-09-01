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

## Estado del proyecto

Desarrollo en curso, guiado por un plan de 14 días dividido en 12 fases (ver [docs/planificacion.md](docs/planificacion.md)):

| Fase | Alcance | Estado |
|------|---------|--------|
| F0 | Convenciones Doxygen, planificación, informe técnico, esqueleto modular y menú | ✅ Hecha |
| F1 | Tipos y matriz de efectividad, módulo Pokédex, catálogo de 150 especies | ✅ Hecha |
| F2 | Entrenadores, ejemplares y equipos | ⬜ Pendiente |
| F3 | Motor de combate (daño, turnos, KO) | ⬜ Pendiente |
| F4 | Backtracking: construcción automática de equipos | ⬜ Pendiente |
| F5 | Fase de grupos y puntuación | ⬜ Pendiente |
| F6 | Fase eliminatoria y campeón | ⬜ Pendiente |
| F7 | Persistencia de resultados y entrenadores | ⬜ Pendiente |
| F8 | Validación integral de entradas | ⬜ Pendiente |
| F9–F11 | Pruebas completas, documentación final y entrega | ⬜ Pendiente |

## Características actuales

- **Pokédex inmutable**: las 150 especies de la primera generación se leen de `data/pokedex.txt` (número, nombre, tipos, estadísticas base). El código fuente no contiene datos de especies.
- **Consultas**: mostrar la Pokédex completa, buscar por número y por nombre (búsqueda insensible a mayúsculas y acentos).
- **Efectividad de tipos**: matriz 18×18 cargada desde `data/efectividad.txt`, con respaldo embebido si el archivo falta.
- **Menú resistente**: entradas inválidas u opciones inexistentes no terminan el programa.

## Estructura del repositorio

```
├── src/                  Código fuente (módulos con cabeceras .h)
│   ├── main.c            Flujo general y menú (sin lógica de negocio)
│   ├── constantes.h      Constantes del sistema
│   └── *.h               Interfaces de módulos: pokedex, tipos, entrenador,
│                         equipo, combate, torneo, resultados, archivos, validación
├── data/                 Datos del sistema (Pokédex, efectividad, entrenadores...)
├── docs/                 Planificación, convenciones Doxygen e informe técnico
├── openspec/             Artefactos SDD (propuesta, especificaciones, diseño, tareas)
├── Doxyfile              Configuración de documentación Doxygen (salida en español)
└── Makefile              Compilación con gate de cero warnings
```

## Documentación del proyecto

- [Plan de desarrollo y control de avance](docs/planificacion.md) — qué está hecho y qué queda pendiente.
- [Informe técnico explicativo](docs/informe-tecnico.md) — se construye fase a fase, con diagramas de la arquitectura y de los algoritmos clave.
- [Convenciones de documentación Doxygen](docs/convenciones-doxygen.md) — reglas de documentación del código en español.
- [Artefactos SDD](openspec/changes/gran-torneo-pokemon/) — propuesta, especificaciones con escenarios, diseño técnico, tareas y reportes de verificación.

## Proyecto académico

Este repositorio documenta el trabajo de curso de la asignatura Programación I (Facultad Experimental de Ciencias y Tecnología, Universidad de Carabobo). El desarrollo sigue un flujo de trabajo spec-driven (SDD): cada fase nace de una especificación, se implementa de forma modular y se verifica con pruebas scriptadas antes de marcarse como completada.

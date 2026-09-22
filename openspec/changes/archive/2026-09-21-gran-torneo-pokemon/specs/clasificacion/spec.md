# Clasificación Specification

## Purpose

Generación de la tabla de clasificación de los entrenadores y su salida a archivo.

## Requirements

### Requirement: Tabla de clasificación (RF-CLS-01)

El sistema MUST generar la clasificación con: posición, id, nombre, victorias, empates, derrotas y puntos, ordenada según los criterios de RF-TRN-04.

#### Scenario: Tabla ordenada

- GIVEN entrenadores con resultados registrados
- WHEN se genera la clasificación
- THEN cada fila tiene posición, id, nombre, victorias, empates, derrotas y puntos, ordenada por los criterios

#### Scenario: Salida a archivo

- GIVEN la clasificación generada
- WHEN se escribe `data/clasificacion.txt`
- THEN el archivo contiene la tabla ordenada con el formato definido

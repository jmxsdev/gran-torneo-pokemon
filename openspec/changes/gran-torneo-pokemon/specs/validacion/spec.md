# Validación Specification

## Purpose

Validación integral de entradas del usuario: una entrada inválida no debe terminar el programa inesperadamente.

## Requirements

### Requirement: Validación de entradas (RF-TEC-03)

El sistema MUST validar: opciones de menú inexistentes, Pokémon inexistentes, entrenadores duplicados, identificadores inválidos, niveles inválidos, equipos que exceden el tamaño permitido, tipos inválidos, resultados inválidos, combates inexistentes, entrenadores no clasificados, eliminados que intentan continuar y archivos inexistentes o no abribles. Una entrada inválida MUST NOT terminar el programa inesperadamente.

#### Scenario: Opción de menú inválida no rompe el programa

- GIVEN el menú mostrado
- WHEN el usuario ingresa "99"
- THEN el sistema muestra un error
- AND vuelve a pedir entrada

#### Scenario: Pokémon inexistente

- GIVEN una búsqueda en la Pokédex
- WHEN se ingresa un nombre inexistente
- THEN se informa que no existe
- AND el programa continúa

#### Scenario: Entrenador duplicado

- GIVEN un entrenador con id 7 ya registrado
- WHEN se intenta registrar otro con id 7
- THEN se rechaza
- AND el programa continúa

#### Scenario: Nivel inválido

- GIVEN la creación de un ejemplar
- WHEN se ingresa nivel 0 o 51
- THEN se rechaza
- AND se solicita un nivel válido

#### Scenario: Archivo inexistente

- GIVEN un archivo de datos que no existe o no se puede abrir
- WHEN el sistema intenta leerlo
- THEN informa del error
- AND continúa sin terminar abruptamente

#### Scenario: Entrada inválida no termina el programa

- GIVEN cualquier entrada inválida
- WHEN el sistema la procesa
- THEN responde con un mensaje de error
- AND permanece en ejecución

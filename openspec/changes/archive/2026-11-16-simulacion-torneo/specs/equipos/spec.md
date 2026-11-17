# Equipos Specification

## Purpose

Creación de ejemplares a partir de especies (separación especie/ejemplar) y construcción de equipos mediante recursividad + backtracking, con validación de restricciones.

## Requirements

### Requirement: Backtracking de formación de equipos (RF-EQP-04) — MODIFIED

La construcción automática MUST usar recursividad + backtracking: explora combinaciones hasta cumplir TODAS las restricciones (cantidad determinada, nivel total máximo, mínimo de tipos diferentes, estadísticas objetivo, restricciones sobre integrantes); retrocede ante combinaciones parciales inválidas; si encuentra solución, muestra el equipo; si no existe solución, informa que no existe.

La restricción MAY incluir un **nivel objetivo** (`nivel_objetivo`): cuando es mayor que 0, cada ejemplar de la solución se genera en ese nivel (acotado al nivel total máximo), en lugar del nivel mínimo; cuando es 0, se conserva el comportamiento actual (nivel mínimo).

#### Scenario: Nivel objetivo medio en la formación

- GIVEN la Pokédex cargada y restricciones con `nivel_objetivo = 30` y nivel total máximo 180
- WHEN se invoca la formación automática
- THEN cada ejemplar de la solución se genera en nivel 30 (acotado al presupuesto)
- AND el nivel total de la solución no supera 180

#### Scenario: Nivel objetivo 0 conserva el comportamiento actual

- GIVEN restricciones con `nivel_objetivo = 0`
- WHEN se invoca la formación automática
- THEN el comportamiento es idéntico al anterior (nivel mínimo, como hoy)

### Requirement: Pertenencia a la Pokédex (RF-EQP-01)

Cada Pokémon de un equipo MUST pertenecer a alguna de las 150 especies de la Pokédex.

#### Scenario: Especie inexistente rechazada

- GIVEN la Pokédex cargada
- WHEN se intenta crear un ejemplar con número de especie #151
- THEN se rechaza con un mensaje de error
- AND no termina el programa

# Equipos Specification

## Purpose

Creación de ejemplares a partir de especies (separación especie/ejemplar) y construcción de equipos mediante recursividad + backtracking, con validación de restricciones.

## Requirements

### Requirement: Pertenencia a la Pokédex (RF-EQP-01)

Cada Pokémon de un equipo MUST pertenecer a alguna de las 150 especies de la Pokédex.

#### Scenario: Especie inexistente rechazada

- GIVEN la Pokédex cargada
- WHEN se intenta crear un ejemplar con número de especie #151
- THEN se rechaza con un mensaje de error
- AND no termina el programa

### Requirement: Creación de ejemplar (RF-EQP-02)

Al seleccionar una especie, el sistema MUST crear un ejemplar con: id único, especie, nombre, nivel, HP, ataque, defensa, velocidad, tipo(s) y estado actual.

#### Scenario: Ejemplar creado desde especie

- GIVEN la especie #001 Bulbasaur
- WHEN se crea un ejemplar de nivel 12
- THEN el ejemplar tiene especie Bulbasaur y nivel 12
- AND sus estadísticas se derivan con la fórmula documentada (base × nivel/50 + nivel + 10 para HP; base × nivel/50 + nivel + 5 para ataque/defensa/velocidad; más una variación determinista por id del ejemplar)
- AND ejemplares distintos de la misma especie normalmente difieren en estadísticas

### Requirement: Separación especie/ejemplar (RF-EQP-03)

Las estadísticas del ejemplar MUST generarse a partir de la especie, y dos ejemplares de la misma especie MAY tener estadísticas diferentes. El combate modifica solo el HP actual del ejemplar, MUST NOT los datos de la especie.

#### Scenario: Dos ejemplares de la misma especie difieren

- GIVEN la especie #001 Bulbasaur
- WHEN se crean dos ejemplares de niveles distintos
- THEN sus estadísticas pueden diferir
- AND la especie Bulbasaur permanece sin cambios

#### Scenario: Mutación solo del ejemplar

- GIVEN un ejemplar dañado en combate
- WHEN se consulta su especie en la Pokédex
- THEN el HP base de la especie permanece intacto
- AND solo el HP actual del ejemplar cambió

### Requirement: Backtracking de formación de equipos (RF-EQP-04)

La construcción automática MUST usar recursividad + backtracking: explora combinaciones hasta cumplir TODAS las restricciones (cantidad determinada, nivel total máximo, mínimo de tipos diferentes, estadísticas objetivo, restricciones sobre integrantes); retrocede ante combinaciones parciales inválidas; si encuentra solución, muestra el equipo; si no existe solución, informa que no existe.

#### Scenario: Equipo que cumple todas las restricciones

- GIVEN la Pokédex cargada y restricciones: 3 Pokémon, nivel total máximo 100, al menos 2 tipos distintos
- WHEN se invoca la formación automática
- THEN devuelve un equipo que cumple TODAS las restricciones
- AND muestra el equipo generado

#### Scenario: Sin solución informa que no existe

- GIVEN restricciones imposibles (por ejemplo, 6 Pokémon con nivel total máximo 5)
- WHEN se invoca la formación automática
- THEN el sistema informa que no existe solución
- AND no devuelve un equipo inválido

### Requirement: Validación de equipos (RF-EQP-05)

El sistema MUST validar: tamaño permitido (1–6; el torneo usa el equipo completo), especies existentes, niveles válidos (1–100) y tipos válidos.

#### Scenario: Tamaño fuera de rango

- GIVEN un equipo en construcción
- WHEN se intenta agregar un Pokémon excediendo el tamaño permitido
- THEN se rechaza con un mensaje de error
- AND no termina el programa

#### Scenario: Nivel inválido

- GIVEN un ejemplar en creación
- WHEN se asigna nivel 0 o 101
- THEN se rechaza
- AND se solicita un nivel válido

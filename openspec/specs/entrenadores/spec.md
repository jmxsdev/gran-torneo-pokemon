# Entrenadores Specification

## Purpose

Registro, identificación única, persistencia y consulta de los entrenadores del torneo.

## Requirements

### Requirement: Datos del entrenador (RF-ENT-01)

Cada entrenador MUST almacenar como mínimo: identificador único, nombre, equipo Pokémon, victorias, empates, derrotas y puntuación.

#### Scenario: Registro de entrenador

- GIVEN el sistema en ejecución
- WHEN se registra un entrenador con id y nombre válidos
- THEN se crea con victorias, empates, derrotas y puntuación en cero

### Requirement: Identificador único (RF-ENT-02)

El sistema MUST validar que el identificador sea único; MUST NOT existir dos entrenadores con el mismo id.

#### Scenario: Identificador duplicado

- GIVEN un entrenador ya registrado con id 7
- WHEN se intenta registrar otro entrenador con id 7
- THEN el registro se rechaza con un mensaje de error
- AND el programa continúa sin registrar el duplicado

#### Scenario: Identificador inválido

- GIVEN el sistema en ejecución
- WHEN se intenta registrar un entrenador con id vacío o no numérico
- THEN se rechaza con un mensaje de error
- AND no termina el programa

### Requirement: Persistencia (RF-ENT-03)

El sistema SHOULD persistir los entrenadores en `data/entrenadores.txt` con id, nombre e información suficiente para reconstruir el equipo.

#### Scenario: Guardado y recarga de entrenadores

- GIVEN entrenadores registrados en memoria
- WHEN se guardan en `data/entrenadores.txt` y se recargan
- THEN se reconstruyen id, nombre y equipo

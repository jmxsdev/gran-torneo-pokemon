# Torneo Specification

## Purpose

Fase de grupos round-robin: 32 entrenadores en 8 grupos de 4, 48 combates (1–48), puntuación y clasificación con desempates deterministas.

## Requirements

### Requirement: Dos fases (RF-TRN-01)

El torneo MUST tener dos fases: fase de grupos y fase eliminatoria.

#### Scenario: Estructura del torneo

- GIVEN el torneo iniciado
- WHEN se consulta su estructura
- THEN presenta fase de grupos (combates 1–48) y eliminatoria (49–64)

### Requirement: Grupos round-robin (RF-TRN-02)

El torneo MUST contar con 8 grupos de 4 entrenadores; dentro de cada grupo se juega round-robin: cada entrenador enfrenta una vez a cada uno de los otros (6 combates por grupo, 48 en total).

#### Scenario: 32 entrenadores exactos

- GIVEN un número de entrenadores distinto de 32
- WHEN se intenta armar los grupos
- THEN el sistema rechaza la operación
- AND informa que se requieren exactamente 32 entrenadores

#### Scenario: Round-robin de un grupo

- GIVEN un grupo de 4 entrenadores
- WHEN se calculan los enfrentamientos
- THEN genera 6 combates, todos contra todos una sola vez

### Requirement: Puntuación (RF-TRN-03)

En fase de grupos MUST aplicarse: victoria 3 puntos, empate 1 punto, derrota 0 puntos.

#### Scenario: Asignación de puntos

- GIVEN un combate de grupos terminado
- WHEN se registra el resultado
- THEN el ganador suma 3 y el perdedor 0, o ambos 1 en caso de empate

### Requirement: Desempate de clasificación (RF-TRN-04)

Ante igualdad, la clasificación MUST desempatar en orden: 1) mayor cantidad de puntos, 2) mayor cantidad de victorias, 3) mayor cantidad de Pokémon derrotados en la fase de grupos, 4) criterio adicional (enfrentamiento directo y, si persiste, id menor).

#### Scenario: Desempate por victorias

- GIVEN dos entrenadores con igual puntuación
- WHEN se ordenan
- THEN se desempata por mayor cantidad de victorias

#### Scenario: Desempate por Pokémon derrotados

- GIVEN dos entrenadores con igual puntuación y victorias
- WHEN se ordenan
- THEN se desempata por mayor cantidad de Pokémon derrotados

#### Scenario: Desempate por criterio adicional

- GIVEN empate en puntos, victorias y Pokémon derrotados
- WHEN se ordenan
- THEN se aplica el enfrentamiento directo y, si persiste, el id menor

### Requirement: Orden final de grupos (RF-TRN-05)

Al finalizar la fase de grupos, los entrenadores MUST ordenarse por posición según los criterios de RF-TRN-04.

#### Scenario: Tabla de posiciones del grupo

- GIVEN la fase de grupos completa
- WHEN se solicita la clasificación
- THEN muestra las posiciones 1 a 4 por grupo según los criterios

### Requirement: Clasificación automática (RF-TRN-06)

La clasificación a la eliminatoria MUST ser automática por posición y grupo (1A, 2A, 1B, 2B, ...); el usuario MUST NOT indicar manualmente quién clasifica.

#### Scenario: Clasificación sin intervención manual

- GIVEN la fase de grupos finalizada
- WHEN se determina quién clasifica
- THEN el sistema resuelve el 1.º y 2.º de cada grupo sin entrada del usuario

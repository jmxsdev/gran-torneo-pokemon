# Simulación Specification

## Purpose

Ejecutar automáticamente los 64 combates del torneo a partir de los entrenadores y sus equipos, usando el motor de combate existente, hasta determinar al campeón sin intervención del usuario (PDF §1.6 "procesar los enfrentamientos").

## Requirements

### Requirement: Simulación completa del torneo (RF-SIM-01)

El sistema MUST disponer de una operación que, dados 32 entrenadores con equipos válidos, ejecute los 48 combates de la fase de grupos y los 16 de la eliminatoria y determine el campeón y las cuatro posiciones finales, sin que el usuario cargue resultados uno a uno.

#### Scenario: Simulación desde equipos cargados

- GIVEN 32 entrenadores registrados, cada uno con un equipo válido (1–6 ejemplares)
- WHEN el usuario solicita simular el torneo
- THEN el sistema ejecuta los 64 combates de forma automática
- AND muestra el campeón y las cuatro posiciones finales

#### Scenario: Rechazo sin los 32 entrenadores

- GIVEN un número de entrenadores distinto de 32
- WHEN el usuario solicita simular el torneo
- THEN el sistema rechaza la operación e informa el requisito de 32 entrenadores
- AND el programa no termina

### Requirement: Determinismo de la simulación (RF-SIM-02)

La simulación MUST ser determinista: para los mismos datos de entrada produce el mismo resultado en cada ejecución (sin aleatoriedad), de modo que la salida sea reproducible y verificable.

#### Scenario: Reproducibilidad

- GIVEN el mismo conjunto de entrenadores y equipos
- WHEN se simula el torneo dos veces
- THEN ambas ejecuciones producen el mismo campeón y las mismas posiciones finales

### Requirement: Reutilización de la máquina de estados (RF-SIM-03)

La simulación MUST aplicar cada resultado a través de la puerta de mutación del torneo (`torneo_aplicar_resultado`) y MUST NOT reimplementar la puntuación, la clasificación ni el bracket de la eliminatoria.

#### Scenario: Sin duplicación de reglas

- GIVEN la simulación en curso
- WHEN se resuelve cada combate
- THEN el resultado se aplica con la misma función que la carga manual
- AND la clasificación y el bracket se derivan del estado del torneo, no de lógica duplicada

### Requirement: Relleno automático de equipos faltantes (RF-SIM-04)

La simulación MUST poder completar automáticamente los equipos de los entrenadores que no tengan un equipo válido, generándolos con el algoritmo de backtracking (`equipo_formar_backtracking`) con restricciones deterministas, de modo que baste con cargar los 32 entrenadores. Los equipos válidos existentes MUST NOT modificarse.

#### Scenario: Entrenadores sin equipo se completan

- GIVEN 32 entrenadores registrados, algunos sin equipo válido
- WHEN se prepara la simulación
- THEN a cada entrenador sin equipo válido se le genera un equipo por backtracking
- AND los entrenadores con equipo válido conservan su equipo

#### Scenario: Completitud determinista

- GIVEN el mismo conjunto de entrenadores (algunos sin equipo)
- WHEN se ejecuta la simulación dos veces
- THEN los equipos generados son idénticos en ambas ejecuciones (sin aleatoriedad)

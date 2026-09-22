# Combate Specification

## Purpose

Reglamento de combate entre dos entrenadores: selección de Pokémon, orden por velocidad, cálculo de daño con nivel/ataque/defensa/efectividad de tipos y determinación del ganador.

## Requirements

### Requirement: Enfrentamiento (RF-CMB-01)

Cada combate MUST enfrentar a dos entrenadores con sus equipos; al comenzar, cada entrenador selecciona un Pokémon.

#### Scenario: Inicio de combate

- GIVEN dos entrenadores con equipos válidos
- WHEN inicia el combate
- THEN cada entrenador selecciona un Pokémon para luchar

### Requirement: Orden por velocidad (RF-CMB-02)

El Pokémon con mayor velocidad MUST atacar primero; ante igualdad de velocidad se aplica un desempate determinista (ataca primero el ejemplar del entrenador 1 del enfrentamiento).

#### Scenario: Mayor velocidad ataca primero

- GIVEN el Pokémon A con velocidad 60 y el Pokémon B con velocidad 45
- WHEN se determina el orden
- THEN A ataca primero

#### Scenario: Igual velocidad, desempate determinista

- GIVEN dos Pokémon con igual velocidad
- WHEN se determina el orden
- THEN ataca primero el ejemplar del entrenador 1 del enfrentamiento

### Requirement: Fórmula de daño (RF-CMB-03)

El daño MUST calcularse considerando como mínimo: nivel, ataque, defensa, tipo del atacante y tipo(s) del defensor. La efectividad de tipos MUST usar los multiplicadores 2 (muy efectivo), 1 (normal), 0.5 (poco efectivo) y 0 (sin efecto).

#### Scenario: Daño normal

- GIVEN un ataque con efectividad normal (×1)
- WHEN se calcula el daño
- THEN el daño se deriva de nivel, ataque y defensa sin multiplicador adicional

#### Scenario: Daño superefectivo

- GIVEN el tipo del ataque es superefectivo contra el defensor
- WHEN se calcula el daño
- THEN se aplica multiplicador 2

#### Scenario: Daño poco efectivo

- GIVEN el tipo del ataque es poco efectivo contra el defensor
- WHEN se calcula el daño
- THEN se aplica multiplicador 0.5

#### Scenario: Sin efecto

- GIVEN el tipo del ataque no afecta al defensor
- WHEN se calcula el daño
- THEN el daño es 0 (sin efecto)

#### Scenario: Defensor con dos tipos

- GIVEN un defensor con dos tipos
- WHEN se calcula la efectividad
- THEN se consideran ambos tipos y el multiplicador es el producto (×0.25, ×0.5, ×1, ×2 o ×4)

### Requirement: HP y derrota (RF-CMB-04)

El HP actual del defensor MUST disminuir según el daño; con HP=0 queda derrotado y el entrenador selecciona otro Pokémon. El combate continúa hasta que un entrenador no tenga Pokémon disponibles; gana quien conserve al menos un Pokémon capaz de continuar.

#### Scenario: Derrota de un Pokémon

- GIVEN un Pokémon defensor con HP 10 y un ataque de daño 15
- WHEN se aplica el daño
- THEN su HP llega a 0 y queda derrotado

#### Scenario: Victoria por agotamiento

- GIVEN el entrenador B sin Pokémon disponibles
- WHEN el entrenador A conserva al menos un Pokémon
- THEN A es declarado ganador

### Requirement: Efectividad de tipos (RF-CMB-05)

Cada combinación atacante/defensor MUST producir Muy efectivo, Normal, Poco efectivo o Sin efecto; con defensor de dos tipos se consideran ambos; la tabla se almacena en una estructura de datos apropiada.

#### Scenario: Consulta de la tabla de efectividad

- GIVEN la tabla de tipos cargada (18×18)
- WHEN se consulta una combinación atacante/defensor
- THEN devuelve uno de los cuatro resultados posibles

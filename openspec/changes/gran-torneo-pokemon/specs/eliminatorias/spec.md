# Eliminatorias Specification

## Purpose

Fase eliminatoria (combates 49–64): octavos, cuartos, semifinal, tercer lugar y final, con emparejamientos fijos por reglamento y sin empates.

## Requirements

### Requirement: Estructura sin empates (RF-ELM-01)

La eliminatoria MUST componerse de octavos de final, cuartos de final, semifinal, tercer lugar y final. MUST NOT existir empates: cada combate MUST producir obligatoriamente un ganador. El perdedor queda eliminado, salvo los perdedores de semifinal (P61, P62), que disputan el tercer lugar.

#### Scenario: Sin empates en eliminatoria

- GIVEN un combate eliminatorio
- WHEN termina
- THEN hay exactamente un ganador y un perdedor

#### Scenario: Perdedores de semifinal al tercer lugar

- GIVEN los perdedores P61 y P62 de las semifinales
- WHEN se programa el combate 63
- THEN P61 y P62 disputan el tercer lugar

### Requirement: Octavos de final (RF-ELM-02)

Los octavos (combates 49–56) enfrentan a los 16 clasificados con emparejamientos fijos: 49: 1A-2B, 50: 1C-2D, 51: 1E-2F, 52: 1G-2H, 53: 1B-2A, 54: 1D-2C, 55: 1F-2E, 56: 1H-2G. El ganador se identifica como G<número de combate>.

#### Scenario: Emparejamiento fijo de octavos

- GIVEN los 16 clasificados
- WHEN se generan los combates 49–56
- THEN los emparejamientos son exactamente los fijados por reglamento

### Requirement: Cuartos de final (RF-ELM-03)

Cuartos (combates 57–60): 57: G49-G50, 58: G53-G54, 59: G51-G52, 60: G55-G56. Los ganadores avanzan a semifinal.

#### Scenario: Coherencia entre octavos y cuartos

- GIVEN el ganador del combate 49 (G49)
- WHEN se programa el combate 57
- THEN G49 participa en el combate 57

### Requirement: Semifinales (RF-ELM-04)

Semifinal (combates 61–62): 61: G57-G58, 62: G59-G60. Los ganadores clasifican a la final; los perdedores se identifican P61 y P62.

#### Scenario: Perdedores identificados

- GIVEN las semifinales terminadas
- WHEN se identifican los perdedores
- THEN se registran como P61 y P62

### Requirement: Tercer lugar y final (RF-ELM-05)

Tercer lugar y final (combates 63–64): 63: P61-P62, 64: G61-G62. Posiciones finales: campeón = G64, subcampeón = P64, tercer lugar = G63, cuarto lugar = P63. Al finalizar, el sistema muestra los cuatro primeros lugares.

#### Scenario: Coronación del campeón

- GIVEN la final (combate 64) terminada
- WHEN se determinan las posiciones
- THEN campeón = G64, subcampeón = P64, tercero = G63, cuarto = P63
- AND el sistema muestra los cuatro primeros lugares

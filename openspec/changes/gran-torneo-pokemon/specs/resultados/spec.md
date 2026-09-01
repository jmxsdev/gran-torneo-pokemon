# Resultados Specification

## Purpose

Carga y validación de resultados por teclado o archivo, con resultados parciales y consistencia entre rondas.

## Requirements

### Requirement: Almacenamiento y parciales (RF-RES-01)

El sistema MUST almacenar resultados por teclado o archivo, soportar resultados parciales y actualizar la información a medida que se incorporan nuevos resultados.

#### Scenario: Resultado parcial

- GIVEN un archivo con solo algunos resultados de la fase de grupos
- WHEN se cargan
- THEN el sistema acepta los resultados parciales
- AND los faltantes pueden ingresarse por teclado después

### Requirement: Validación de resultados (RF-RES-02)

Cada resultado MUST validarse: entrenadores existentes, pertenencia al combate correspondiente, ronda correcta, clasificación correcta de participantes, eliminado no reincide, ganador entre los participantes, sin empates en eliminatoria y compatibilidad con rondas anteriores.

#### Scenario: Entrenador inexistente

- GIVEN un resultado que referencia un entrenador no registrado
- WHEN se carga
- THEN se rechaza con un mensaje de error
- AND no termina el programa

#### Scenario: Eliminado no reaparece

- GIVEN un entrenador eliminado en octavos
- WHEN aparece en un resultado de cuartos
- THEN el resultado se rechaza por inválido

#### Scenario: Empate en eliminatoria rechazado

- GIVEN un resultado de eliminatoria que declara empate
- WHEN se carga
- THEN se rechaza porque la eliminatoria no admite empates

### Requirement: Participantes resueltos por el sistema (RF-RES-03)

El sistema MUST determinar automáticamente los participantes de cada enfrentamiento; el usuario MUST NOT introducir arbitrariamente los participantes de una ronda.

#### Scenario: Participante manual rechazado

- GIVEN el sistema con los participantes resueltos para el combate 57
- WHEN el usuario intenta cargar un resultado con participantes distintos
- THEN el resultado se rechaza

### Requirement: Archivo de resultados (RF-RES-04)

El archivo `data/resultados.txt` MUST identificar por registro: número de combate, entrenador 1, entrenador 2, resultado y ganador; MAY contener resultados parciales; los faltantes se ingresan por teclado.

#### Scenario: Carga desde archivo

- GIVEN `data/resultados.txt` con registros válidos
- WHEN se carga
- THEN se incorporan y validan los resultados

# Resultados Specification

## Purpose

Carga y validación de resultados del torneo, consulta del historial de combates y persistencia de los resultados aplicados.

## Requirements

### Requirement: Almacenamiento y parciales (RF-RES-01)

El sistema MUST permitir cargar resultados por teclado y por archivo, y trabajar con resultados parciales; los combates sin resultado permanecen pendientes y se actualizan a medida que se incorporan resultados.

#### Scenario: Resultados parciales

- GIVEN un torneo con resultados parciales cargados
- WHEN se consulta el estado
- THEN los combates con resultado constan como resueltos y los demás como pendientes

### Requirement: Validación de resultados (RF-RES-02)

Cada resultado MUST validarse: entrenadores existentes, participantes resueltos por el sistema, ronda correcta, clasificados correctos, eliminados no reaparecen, ganador perteneciente al combate, sin empates en eliminatoria y compatibilidad con rondas anteriores.

#### Scenario: Resultado inválido rechazado

- GIVEN un resultado que contradice el estado del torneo
- WHEN se intenta cargar
- THEN se rechaza con un mensaje específico
- AND el programa no termina

### Requirement: Participantes resueltos por el sistema (RF-RES-03)

Los participantes de cada combate MUST ser resueltos por el sistema (calendario y tabla de bracket); el usuario MUST NOT introducirlos arbitrariamente.

#### Scenario: Participantes automáticos

- GIVEN el estado del torneo
- WHEN se determina el enfrentamiento de un combate
- THEN los ids se resuelven con la tabla de fuentes, no por entrada del usuario

### Requirement: Archivo de resultados (RF-RES-04)

El sistema MUST poder cargar y guardar los resultados en un archivo de texto, admitiendo resultados parciales; el guardado persiste los combates resueltos y es recargable.

#### Scenario: Guardado recargable

- GIVEN resultados aplicados en la sesión
- WHEN se guarda el archivo de resultados
- THEN el archivo contiene los combates resueltos en el formato establecido
- AND al recargarlo se reproducen los mismos resultados

### Requirement: Persistencia de resultados al salir (RF-RES-05)

Los resultados aplicados en una sesión (por teclado, archivo o simulación) MUST persistirse en `data/resultados.txt` al salir del programa, de modo que no se pierdan entre sesiones.

#### Scenario: Resultados por teclado sobreviven al salir

- GIVEN el usuario aplica resultados por teclado
- WHEN sale del programa
- THEN `data/resultados.txt` refleja esos resultados

### Requirement: Historial de combates (RF-RES-06)

El sistema MUST poder consultar el historial de los combates: para cada combate 1–64, sus participantes, su ronda o grupo, su resultado, su ganador y sus KOs; los pendientes se identifican explícitamente.

#### Scenario: Historial completo

- GIVEN el torneo armado
- WHEN se consulta el historial
- THEN se listan los 64 combates con participantes y estado (resuelto o pendiente)

#### Scenario: Historial con parciales

- GIVEN resultados parciales cargados
- WHEN se consulta el historial
- THEN los combates resueltos muestran su ganador y KOs, y los pendientes se marcan

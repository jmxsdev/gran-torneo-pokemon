# Entrenadores Specification

## Purpose

Registro de entrenadores con id único, consulta y persistencia de entrenadores y sus equipos en archivo.

## Requirements

### Requirement: Datos del entrenador (RF-ENT-01)

Para cada entrenador el sistema MUST almacenar: identificador, nombre, equipo y contadores de victorias/empates/derrotas/puntuación.

#### Scenario: Datos completos

- GIVEN un entrenador registrado
- WHEN se consulta
- THEN constan su id, nombre, equipo y contadores

### Requirement: Identificador único (RF-ENT-02)

El identificador MUST ser único; el sistema MUST validar que no existan dos entrenadores con el mismo id.

#### Scenario: Duplicado rechazado

- GIVEN un id ya registrado
- WHEN se intenta registrar otro entrenador con ese id
- THEN el registro se rechaza

### Requirement: Persistencia de entrenadores (RF-ENT-03) — MODIFIED

El sistema MUST poder guardar y recargar la información de los entrenadores y sus equipos desde archivo. El ciclo cargar → guardar → recargar MUST preservar el orden de los ejemplares de cada equipo (round-trip estable): el guardado MUST NOT invertir el orden de los ejemplares respecto al archivo cargado.

#### Scenario: Round-trip estable del orden de equipos

- GIVEN un archivo de entrenadores con equipos en un orden determinado
- WHEN se carga y se guarda sin cambios
- THEN el archivo resultante conserva el mismo orden de ejemplares en cada equipo

#### Scenario: Recarga fiel

- GIVEN un archivo de entrenadores guardado
- WHEN se recarga
- THEN cada entrenador reconstruye su equipo con los mismos ejemplares y el mismo orden

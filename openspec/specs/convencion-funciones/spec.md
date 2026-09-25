# Convención de Funciones Specification

## Purpose

Convención del curso sobre el diseño de funciones y procedimientos: toda función no-void devuelve exactamente un resultado mediante `return`; las operaciones con múltiples salidas se expresan como procedimiento `void` con parámetros por referencia. Esta capacidad rige la conversión de 23 funciones del proyecto sin alterar el comportamiento observable.

## Requirements

### Requirement: Función de un solo resultado (RF-CNV-01)

Toda función no-void MUST devolver exactamente un resultado mediante `return`. Una función no-void MUST NOT modificar valores a través de sus parámetros (ni parámetros por referencia, ni archivos escritos, ni contadores globales como salida).

#### Scenario RF-CNV-01-1: Función pura conforme

- GIVEN la función `combate_calcular_danio(atacante, defensor)`
- WHEN se invoca
- THEN devuelve un único resultado (`int`, el daño) mediante `return`
- AND no modifica parámetros, archivos ni contadores globales

#### Scenario RF-CNV-01-2: Función con resultado + mutación, no conforme

- GIVEN la función `pokedex_cargar(pd, ruta)` que devuelve `bool` y además completa `*pd`
- WHEN se evalúa contra la convención
- THEN es no conforme: produce dos resultados (el `bool` y la Pokédex mutada)
- AND MUST convertirse a procedimiento `void`

### Requirement: Procedimiento para múltiples salidas (RF-CNV-02)

Las operaciones con más de una salida MUST expresarse como procedimiento `void` con parámetros por referencia, incluida la salida de éxito `bool *ok`. La conversión `bool f(..., T *out)` → `void f(..., T *out, bool *ok)` es el patrón obligatorio.

#### Scenario RF-CNV-02-1: Procedimiento multi-salida conforme

- GIVEN el procedimiento `torneo_participantes(t, numero, id1, id2)`
- WHEN se invoca
- THEN es `void` y entrega sus salidas (`*id1`, `*id2`) solo por parámetros por referencia

#### Scenario RF-CNV-02-2: Conversión de función a procedimiento

- GIVEN una función `bool f(..., T *out)`
- WHEN se convierte a la convención
- THEN pasa a `void f(..., T *out, bool *ok)` con el éxito en `*ok`

### Requirement: Equivalencia de comportamiento (RF-CNV-03)

Las conversiones MUST preservar el comportamiento observable: la batería `tests/run_tests.sh` (23 PASS) y los 20 goldens MUST pasar sin regenerarse, y el build con `gcc -std=c99 -Wall -Wextra` MUST quedar sin warnings.

#### Scenario RF-CNV-03-1: Refactor sin cambio observable

- GIVEN un lote de funciones convertidas a `void`
- WHEN se compila y se ejecuta la batería completa
- THEN los 23 casos y los 20 goldens pasan idénticos sin regeneración
- AND el build queda sin warnings

## Anexo: Trazabilidad del refactor

| # | Función | Ámbito | Módulo | Lote | Conversión objetivo |
|---|---------|--------|--------|------|---------------------|
| 1 | `validar_leer_cadena` | pública | validacion.c | L1 | `bool` → `void + bool *ok` |
| 2 | `validar_separar_campos` | pública | validacion.c | L1 | `int` → `void + int *n` |
| 3 | `tipos_es_valido` | pública | tipos.c | L1 | `bool` → `void + bool *ok` |
| 4 | `pokedex_cargar` | pública | pokedex.c | L1 | `bool` → `void + bool *ok` |
| 5 | `entrenador_registrar` | pública | entrenador.c | L2 | `bool` → `void + bool *ok` |
| 6 | `equipo_agregar_ejemplar` | pública | equipo.c | L2 | `bool` → `void + bool *ok` |
| 7 | `equipo_formar_backtracking` | pública | equipo.c | L2 | `bool` → `void + bool *ok` |
| 8 | `equipo_siguiente_id` | pública | equipo.c | L2 | `int(void)` → `void(int *id)` |
| 9 | `bt_rec` | interna | equipo.c | L2 | `bool` → `void + bool *ok` |
| 10 | `combate_ejecutar` | pública | combate.c | L3 | `bool` → `void + bool *ok` |
| 11 | `combate_atacar` | interna | combate.c | L3 | `int` → `void + int *ganador` |
| 12 | `torneo_armar_grupos` | pública | torneo.c | L4 | `bool` → `void + bool *ok` |
| 13 | `torneo_aplicar_resultado` | pública | torneo.c | L4 | `bool` → `void + bool *ok` |
| 14 | `aplicar_resultado` | interna | resultados.c | L4 | `bool` → `void + bool *ok` |
| 15 | `resultados_cargar_teclado` | pública | resultados.c | L4 | `bool` → `void + bool *ok` |
| 16 | `resultados_cargar_archivo` | pública | resultados.c | L4 | `bool` → `void + bool *ok` |
| 17 | `resultados_validar` | pública | resultados.c | L4 | `bool` → `void + bool *ok` |
| 18 | `archivos_cargar_entrenadores` | pública | archivos.c | L4 | `bool` → `void + bool *ok` |
| 19 | `archivos_cargar_resultados` | pública | archivos.c | L4 | `bool` → `void + bool *ok` |
| 20 | `archivos_guardar_entrenadores` | pública | archivos.c | L4 | `bool` → `void + bool *ok` |
| 21 | `archivos_guardar_resultados` | pública | archivos.c | L4 | `bool` → `void + bool *ok` |
| 22 | `archivos_guardar_clasificacion` | pública | archivos.c | L4 | `bool` → `void + bool *ok` |
| 23 | `armar_torneo_si_falta` | interna | main.c | L4 | `bool` → `void + bool *ok` |

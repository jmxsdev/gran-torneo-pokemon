# Propuesta: Refactor — funciones con un solo resultado

**Change**: `refactor-funciones-un-resultado`
**Fase**: sdd-propose
**Fecha**: 2026-09-25
**Idioma**: español (requisito del proyecto)

---

## Intención (Problema y objetivo)

El docente fijó la convención de que toda función no-void debe devolver **un solo resultado**. Hoy 19 funciones devuelven un valor (`bool`/`int`) **y además** mutan parámetros por referencia: técnicamente devuelven dos resultados y violan la regla. Para devolver múltiples valores debe usarse un **procedimiento `void` con parámetros por referencia**.

Objetivo: alinear el código a la convención **sin cambiar el comportamiento observable** (misma salida por consola, mismos goldens).

## Alcance

### In Scope

- **23 violadores** convertidos a `void`:
  - Patrón `bool f(X *out, …)` → `void f(X *out, …, bool *ok)`.
  - Patrón `int f(…, X *out, …)` → `void f(…, X *out, …, int *n)`.
- **Públicas (19)**: `pokedex_cargar`, `tipos_es_valido`, `entrenador_registrar`, `equipo_agregar_ejemplar`, `equipo_formar_backtracking`, `equipo_siguiente_id`, `combate_ejecutar`, `archivos_cargar_entrenadores`, `archivos_cargar_resultados`, `archivos_guardar_entrenadores`, `archivos_guardar_resultados`, `archivos_guardar_clasificacion`, `resultados_cargar_teclado`, `resultados_cargar_archivo`, `resultados_validar`, `torneo_armar_grupos`, `torneo_aplicar_resultado`, `validar_leer_cadena`, `validar_separar_campos`.
- **Internas (4)**: `bt_rec` (equipo.c), `combate_atacar` (combate.c), `aplicar_resultado` (resultados.c), `armar_torneo_si_falta` (main.c).
- **Llamadores** (~91 referencias) en todos los `.c`.
- **Probes versionados** `tests/probes/*.c` (~15 llamadas) con los mismos asserts.
- **Documentación**: `docs/informe-tecnico.md`, `docs/apuntes-tecnicos.md`, `docs/trazabilidad-requisitos.md` y Doxygen de las firmas.

### Out of Scope

- Cambios de comportamiento visible: los 20 goldens deben pasar **sin regenerarse** (solo se regeneran si un cambio de salida es inevitable y se justifica).
- Renombres de funciones (nombres se conservan).
- Optimizaciones.
- Nuevas funcionalidades (la opción 9 pendiente NO se toca).

## Capabilities

> Contrato con sdd-spec.

### New Capabilities

Ninguna.

### Modified Capabilities

Ninguna — refactor puro: no cambia requisitos a nivel de spec.

## Enfoque por lotes (≤800 líneas/lote)

| Lote | Módulos | Funciones |
|------|---------|-----------|
| L1 | validación + tipos + pokedex | `validar_leer_cadena`, `validar_separar_campos`, `tipos_es_valido`, `pokedex_cargar` |
| L2 | entrenador + equipo | `entrenador_registrar`, `equipo_agregar_ejemplar`, `equipo_formar_backtracking`, `equipo_siguiente_id`, `bt_rec` |
| L3 | combate | `combate_ejecutar`, `combate_atacar` |
| L4 | torneo + resultados + archivos + main | `torneo_armar_grupos`, `torneo_aplicar_resultado`, `aplicar_resultado`, `resultados_cargar_teclado/archivo`, `resultados_validar`, `archivos_cargar_entrenadores/resultados`, `archivos_guardar_entrenadores/resultados/clasificacion`, `armar_torneo_si_falta` |
| L5 | probes + docs + verificación | `tests/probes/*.c`, docs, batería completa |

## Affected Areas

| Área | Impacto | Descripción |
|------|---------|-------------|
| `src/*.h` (9) | Modificado | Firmas de los 15 violadores públicos |
| `src/validacion.c`, `src/tipos.c`, `src/pokedex.c` | Modificado | L1 |
| `src/entrenador.c`, `src/equipo.c` | Modificado | L2 |
| `src/combate.c` | Modificado | L3 |
| `src/torneo.c`, `src/resultados.c`, `src/archivos.c`, `src/main.c` | Modificado | L4 |
| `tests/probes/probe_efectividad.c`, `probe_torneo_completo.c` | Modificado | Adaptar llamadas |
| `docs/*.md` (3) | Modificado | Convención, trazabilidad, informe |

## Riesgos

| Riesgo | Probabilidad | Mitigación |
|--------|--------------|------------|
| `bt_rec` recursivo con doble puntero (`Ejemplar **salida`) | Media | Firma cuidadosa; probar en L2 aislado con probe |
| `combate_ejecutar` (res + ok) y sus llamadores en main/torneo | Media | Un solo lote L3, compilar y correr probes tras cada paso |
| Rework de probes rompe asserts | Media | Conservar asserts idénticos; solo adaptar la forma de llamada |
| Cambio accidental de salida por consola | Baja | `tests/run_tests.sh` como gate; diff byte a byte |

## Plan de rollback

Trabajo por commits, un commit por lote. Revertir cualquier lote con `git revert` del commit correspondiente; los lotes son independientes por dependencia (L1→L2→L3→L4). Los goldens y `data/` no se modifican, por lo que el blast radius es solo código + probes + docs.

## Dependencies

Ninguna externa. Solo `gcc -std=c99 -Wall -Wextra` y `tests/run_tests.sh`.

## Criterios de éxito

- [ ] **0** funciones no-void que modifiquen parámetros por referencia.
- [ ] `tests/run_tests.sh`: **23 PASS** idénticos, 20 goldens sin cambios.
- [ ] Probes adaptados con los mismos asserts, todos PASS.
- [ ] Build con `gcc -std=c99 -Wall -Wextra` **cero warnings**.
- [ ] Documentación (informe, apuntes, trazabilidad, Doxygen) actualizada a las nuevas firmas.

## Confirmados por el docente (2026-09-25)

El docente confirmó que **ambos casos cuentan como resultados**: el archivo escrito
por `archivos_guardar_*` y la mutación del contador global en `equipo_siguiente_id`.
Se incorporan al alcance (23 funciones en total):

- `archivos_guardar_entrenadores/resultados/clasificacion`: `bool (…)` →
  `void (…, bool *ok)`.
- `equipo_siguiente_id`: `int (void)` → `void (int *id)`.

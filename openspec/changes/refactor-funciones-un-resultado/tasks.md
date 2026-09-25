# Tasks: Refactor — funciones de un solo resultado

**Change**: `refactor-funciones-un-resultado` · **Fase**: sdd-tasks · **Fecha**: 2026-09-26
**Idioma**: español · **Inputs**: `proposal.md`, `specs/convencion-funciones/spec.md`, `design.md`
**Inventario**: 23 conversiones + 58 llamadas (design §3). **Pendiente docente**: ninguna (ambos casos confirmados).

## Review Workload Forecast

| Field | Value |
|-------|-------|
| Estimated changed lines | ~820 (add+del, authored) |
| 400-line budget risk | High |
| Chained PRs recommended | Yes |
| Delivery strategy | ask-on-risk (default; no entregada por orquestador) |
| Suggested split | PR 1 (R1) → PR 2 (R2) → PR 3 (R3) → PR 4 (R4) → PR 5 (R5) |
| Chain strategy | pending |

Decision needed before apply: Yes
Chained PRs recommended: Yes
Chain strategy: pending
400-line budget risk: High

### Forecast por lote (≤800 líneas/lote, gate verde por lote)

| Lote | Módulos | Conv. | Llamadores (src+probes) | Est. líneas | Riesgo 400 | PR |
|------|---------|-------|--------------------------|-------------|-----------|----|
| R1 | validacion, tipos, pokedex | 4 | 10 + 2 | ~130 | Bajo | PR 1 |
| R2 | entrenador, equipo | 5 | 10 + 2 | ~180 | Bajo | PR 2 |
| R3 | combate | 2 | 2 + 9 | ~140 | Bajo | PR 3 |
| R4 | torneo, resultados, archivos, main | 12 | 21 + 7 | ~250 | Medio | PR 4 |
| R5 | probes-final, docs, verificación | 0 | 0 | ~120 | Bajo | PR 5 |

### Suggested Work Units

| Unit | Goal | Likely PR | Focused test command | Runtime harness | Rollback boundary |
|------|------|-----------|----------------------|-----------------|-------------------|
| 1 | R1: validación+tipos+pokedex a `void` | PR 1 | `make` + `bash tests/run_tests.sh` | 23 PASS, goldens sin regenerar | `git revert` commit R1 |
| 2 | R2: entrenador+equipo (bt_rec incluido) | PR 2 | `make` + `bash tests/run_tests.sh` | 23 PASS, goldens idénticos | `git revert` commit R2 |
| 3 | R3: combate | PR 3 | `make` + `bash tests/run_tests.sh` | 23 PASS, goldens idénticos | `git revert` commit R3 |
| 4 | R4: torneo+resultados+archivos+main | PR 4 | `make` + `bash tests/run_tests.sh` | 23 PASS, goldens idénticos | `git revert` commit R4 |
| 5 | R5: docs + Doxygen + verificación final | PR 5 | `bash tests/run_tests.sh` + `doxygen Doxyfile` | 23 PASS + sin warnings nuevos | `git revert` commit R5 |

**GATE por lote (R1–R4)**: `make` cero warnings + `bash tests/run_tests.sh` 23 PASS + goldens SIN regenerar (`sha256sum` byte a byte) + árbol compilando (header+impl+todos los llamadores en el mismo lote).

> **Nota de resolución**: el design lista probes en L5 (§6/§10), pero el GATE exige `run_tests.sh` (que compila probes) tras cada lote. Por eso los llamadores de probes se adaptan en el MISMO lote de su función (design §5 ya lo asume para `pokedex_cargar`). R5 queda solo para docs + verificación final.

---

## R1 — validación + tipos + pokedex (4 conversiones)

- [x] **R1.1** Firmas en `src/validacion.h`, `src/tipos.h`, `src/pokedex.h`: `validar_leer_cadena`→`void(…,bool *exito)`, `validar_separar_campos`→`void(…,int *n)`, `tipos_es_valido`→`void(…,bool *es_valido)`, `pokedex_cargar`→`void(…,bool *exito)`. Ref RF-CNV-02, design §3 #1–4. *Aceptación*: `make` compila.
- [x] **R1.2** Cuerpos en `validacion.c`, `tipos.c`, `pokedex.c`: cada `return V;`→`*salida=V; return;`. Ref design §1 regla 1. *Aceptación*: `make` cero warnings.
- [x] **R1.3** Llamadores `main.c`: `validar_leer_cadena` en ~127/~161/~332 y `pokedex_cargar` en ~691 (declarar `bool exito;`). Ref design §5 L1. *Aceptación*: mensajes idénticos, `make`.
- [x] **R1.4** Llamadores internos: `pokedex.c` ~108 (`separar_campos`), ~155 (`tipos_es_valido` doble, `ok1/ok2` sin cortocircuito, D-F), `tipos.c` ~176, `archivos.c` ~104, `resultados.c` ~304. *Aceptación*: `make`.
- [x] **R1.5** Probes: `probe_efectividad.c` ~109 y `probe_torneo_completo.c` ~97 (`pokedex_cargar`, `verificar(exito,…)`). Ref design §6. *Aceptación*: probes compilan.

**GATE R1**: `make` cero warnings + `run_tests.sh` 23 PASS + goldens idénticos.

## R2 — entrenador + equipo (5 conversiones)

- [x] **R2.1** Firmas en `entrenador.h` y `equipo.h`: `entrenador_registrar`, `equipo_agregar_ejemplar`, `equipo_formar_backtracking`→`void(…,bool *exito)`; `equipo_siguiente_id`→`void(int *id)`. Ref §3 #5–8.
- [x] **R2.2** Cuerpo `entrenador.c` (`entrenador_registrar`). Ref §3 #5.
- [x] **R2.3** Cuerpos `equipo.c`: `equipo_agregar_ejemplar`, `equipo_formar_backtracking`, `equipo_siguiente_id`, `bt_rec` (static, D-C). *Aceptación*: `bt_rec` escribe `*exito` antes de CADA `return` (éxito/i>=cantidad/4 podas/no admitida/bucle/exclusión); `equipo_formar_backtracking` usa `bool exito=false;` defensivo.
- [x] **R2.4** Llamadores `main.c` ~174, ~234, ~341, ~347. Ref §5 L2.
- [x] **R2.5** Llamadores `archivos.c` ~153 (`goto linea_invalida`), ~164 (cortocircuito conservado con `bool agregado`). Ref §5 L2.
- [x] **R2.6** Llamador interno `equipo.c` ~364 (`equipo_siguiente_id(&id)`). Ref §5 L2.
- [x] **R2.7** Probes: `probe_efectividad.c` ~74 (`&ej->id`), ~86 (`(void)exito;`). Ref §6.

**GATE R2**: `make` + `run_tests.sh` 23 PASS + goldens idénticos (casos 010/011 + probe cubren bt_rec).

## R3 — combate (2 conversiones)

- [x] **R3.1** Firma `combate.h` (`combate_ejecutar`→`void(…,bool *exito)`). Ref §3 #10.
- [x] **R3.2** Cuerpos `combate.c`: `combate_ejecutar` (8 salidas: params, equipos vacíos, 4 cancelaciones EOF, victoria, turno 20 — checklist `*exito`) y `combate_atacar` (static, `*ganador`∈{0,id,-1}, D-B). Ref §3 #10–11.
- [x] **R3.3** Llamadores internos `combate.c` ~259, ~270 (`combate_atacar(…,&g)`; `if(g==-1)`/`if(g!=0)` intactos). Ref §5 L3.
- [x] **R3.4** Llamador `main.c` `jugar_combate` ~502. Ref §5 L3.
- [x] **R3.5** Probes: `probe_efectividad.c` 9× `combate_ejecutar` (3 validación→`exito==false`, 6→`exito==true`+res). Ref §6.

**GATE R3**: `make` + `run_tests.sh` 23 PASS + goldens idénticos (casos 012–014).

## R4 — torneo + resultados + archivos + main (12 conversiones)

- [x] **R4.1** Firmas `torneo.h`, `resultados.h`, `archivos.h` (10 públicas → `void(…,bool *exito)`). Ref §3 #12–22.
- [x] **R4.2** Cuerpos `torneo.c` (`torneo_armar_grupos`, `torneo_aplicar_resultado`). Ref §3 #12–13.
- [x] **R4.3** Cuerpos `resultados.c`: `aplicar_resultado` (static), `resultados_cargar_teclado`, `resultados_cargar_archivo`, `resultados_validar` (16 `return false`+`return true`→`*exito`; `msg` se llena igual). Ref §3 #14–17.
- [x] **R4.4** Cuerpos `archivos.c`: `cargar_entrenadores`, `cargar_resultados` (delegación pura→`resultados_cargar_archivo`, #19), `guardar_entrenadores`, `guardar_resultados`, `guardar_clasificacion`. Ref §3 #18–22, D-D.
- [x] **R4.5** `main.c`: `armar_torneo_si_falta` (static, #23) + cuerpos llamadores `guardar_al_salir`, `cargar_resultados`. Ref §3 #23.
- [x] **R4.6** Llamadores `main.c`: ~535, ~554/~596/~628, ~558, ~632/~634 (`(void)exito`), ~649, ~698 (carga inicial, mismo aviso). Ref §5 L4.
- [x] **R4.7** Llamadores `resultados.c`: ~165 (`resultados_validar`), ~169 (`torneo_aplicar_resultado`), ~255/~335 (`aplicar_resultado`, `aplicados++`/`rechazadas++`). Ref §5 L4.
- [x] **R4.8** Probes `probe_torneo_completo.c`: ~59 (`aplicar_v1` wrapper conserva `return`), ~100, ~107, ~152/~165/~204/~218. Ref §6.

**GATE R4**: `make` + `run_tests.sh` 23 PASS + goldens idénticos (casos 016–020).

## R5 — docs + Doxygen + verificación final

- [x] **R5.1** Doxygen `.h` (19 públicas: eliminar `@return`, añadir `@param exito/n/id/es_valido`); `equipo_siguiente_id` `@param id`; `combate_atacar`/`bt_rec`/`combate_ejecutar`/`aplicar_resultado`/`armar_torneo_si_falta` `@param exito`/`@param ganador`. Ref design §7. *Aceptación*: `doxygen Doxyfile` sin warnings nuevos.
- [x] **R5.2** `docs/informe-tecnico.md`: §2.1 (`wc -l`), §2.3 mermaid (`*exito=true`), §3.1 viñeta, nueva §3.2. Buscar firmas `bool`/`res` citadas. Ref design §8.
- [x] **R5.3** `docs/apuntes-tecnicos.md`: §4 tercer uso de `void`, §5 snippet `bool exito;` + `archivos_guardar_entrenadores(…,&exito)`. Ref design §8.
- [x] **R5.4** `docs/trazabilidad-requisitos.md`: anotar RF-TEC-03 (`validar_leer_cadena/separar_campos` son `void`). Ref design §8 (opcional).
- [x] **R5.5** Auditoría final: `rg -n '^\s*(bool|int|float)\s+\w+\(' src/*.h` lista solo sección 4; `run_tests.sh` 23 PASS; `sha256sum` goldens byte a byte; `git status --porcelain tests/casos/ data/` vacío. Ref §9.

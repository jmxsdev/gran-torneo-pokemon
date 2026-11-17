# Tareas: Simulación del torneo + historial de combates + cierres de persistencia

**Change**: `simulacion-torneo` · **Fase**: sdd-tasks · **Fecha**: 2026-11-16
**Idioma del artefacto**: español neutro y profesional.
**Entradas**: `proposal.md` (L1–L7), `design.md` (D11–D19), `specs/` (1 nueva + 5 modificadas).

---

## Review Workload Forecast

Presupuesto de sesión: **300 líneas/lote**. Estimaciones `additions` sin contar datos generados. Entrega `single-pr`.

| Lote | Contenido | Líneas est. | ≤300 |
|------|-----------|-------------|------|
| L1 | `combate.c/.h`: parámetro `silencioso` + selector automático (D11/D12) | ≈ 120 | ✓ |
| L2 | `simulacion.c/.h`: relleno de equipos (D18) + `simulacion_ejecutar` + selector + submenú opción 6 (D13) | ≈ 260 | ✓ |
| L3 | `pantalla.c/.h` (padding UTF-8, D19) + `torneo_mostrar_historial` con estilo + `case 9` en `main.c` + 2 goldens | ≈ 200 | ✓ |
| L4 | Persistencia de resultados (`resultados_sucios` + guardado al salir, D16) | ≈ 90 | ✓ |
| L5 | Fix 63/64 (D14) + round-trip de orden de guardado (D15) | ≈ 130 | ✓ |
| L6 | Blindar los tests de combate 012–015 (autónomos `_sin_entrenadores`) + docs + verificación | ≈ 500 (dividible L6a/L6b) | ver nota |
| L7 | Rediseño de pantalla D19 (opciones 1/4/5/7/8/10/11) + regenerar goldens | ≈ 400 (dividible L7a/L7b) | ver nota |

Total ≈ 1 700 líneas en 7 lotes; L6 y L7 exceden 300 y se dividen si hace falta ⇒ sin PRs encadenados.

```
Decision needed before apply: No (resuelta: conservar equipo de Misty + blindar tests de combate + estilo D19)
Chained PRs recommended: No
Chain strategy: single-pr
300-line budget risk: Bajo (L6/L7 pueden dividirse)
```

---

## Decisión pendiente (no bloquea L1–L5, SÍ L6) — RESUELTA (2026-11-16)

El árbol de trabajo tiene `data/entrenadores.txt` invertido y con el equipo nuevo de Misty (ids 178–183), lo que rompe los goldens 012–015 (19/23 PASS).

- [x] **Opción A — Conservar el equipo de Misty** (elegida por el usuario): se conserva el equipo nuevo; **no se regeneran** los goldens, sino que se reescriben como tests autónomos.
- [x] **Blindar los tests de combate** (elegida por el usuario): 012–015 pasan a `_sin_entrenadores` y registran sus propios entrenadores/equipos.
- [ ] ~~Opción B — Revertir~~ descartada.

Resolución: **Opción A + blindado (D17)**. En L6 se reescriben 012–015 como autónomos; `data/entrenadores.txt` conserva el equipo de Misty y deja de ser entrada de la batería de combate. El fix D15 garantiza que un guardado futuro no invierta el orden de los equipos no modificados.

---

## Lote L1 — Modo silencioso (combate) — ✅ HECHO (2026-11-16)

- [x] L1.1 Añadir `bool silencioso` a `combate_ejecutar` y propagarlo a `combate_atacar`/desempate; con `silencioso=true` se suprimen todas las impresiones de traza, conservando el `ResultadoCombate` (D12). Doxygen de `combate.h` actualizado.
- [x] L1.2 Actualizar los puntos de llamada: `main.c` (combate amistoso) y las 9 llamadas de `probe_efectividad.c` pasan `silencioso=false` (comportamiento idéntico, RF-CNV-03).
- [x] L1.3 Verificación: `make` cero warnings; `probe_efectividad` 21/21; batería 19 PASS / 4 FALLA (los mismos 4 fallos por dato, sin regresión nueva).
- [ ] ~~L1.4 selector `seleccionar_primero_disponible`~~ → movido a L2.3 (evita `-Wunused-function` al no usarse en L1).

## Lote L2 — Simulación del torneo (módulo `simulacion.c` + menú) — ✅ HECHO (2026-11-16)

- [x] L2.1 Añadir el campo `nivel_objetivo` a `RestriccionesEquipo` (equipo.h) y su lógica en `bt_rec` (equipo.c): con `0` conserva el comportamiento actual (nivel 1); con `>0` asigna ese nivel medio a cada ejemplar (acotado al presupuesto) — RF-EQP-04 (mod). *(También se inicializó `nivel_objetivo = 0` en `formar_equipo_automatico` de main.c para no leer basura.)*
- [x] L2.2 Crear `src/simulacion.c/.h`: `simulacion_preparar_equipos` (relleno por backtracking con **ventana rotativa** de 24 especies y **nivel medio 30**, D18) y `simulacion_ejecutar` (bucle 1–64 saltando resueltos, `combate_ejecutar` silencioso, `torneo_aplicar_resultado`, posiciones finales) (RF-SIM-01..04).
- [x] L2.3 Añadir `COMBATES_GRUPO` (48) a `constantes.h` (retirando los `#define` locales de `torneo.c`/`resultados.c`) y el selector `seleccionar_primero_disponible` (D11) en `simulacion.c`.
- [x] L2.4 Añadir el submenú `3. Simular torneo automáticamente` en la opción 6 (`cargar_resultados`, main.c) y su `case` (D13); `cargar_resultados` gana el parámetro `const Pokedex *pd`.
- [x] L2.5 Verificación: (a) 32 con equipos → campeón; (b) 32 sin equipos → relleno nivel 30 + ventana → campeón distinto; (c) rechazo ordenado con 31; (d) híbrido 20 parciales + simulación. Batería 19/4 (solo los 4 de combate por dato, pendientes de L6). Goldens 016/017/019 regenerados con la línea del submenú.

## Lote L3 — Opción 9: historial de combates (estilo D19) — ✅ HECHO (2026-11-16)

- [x] L3.1 Crear `src/pantalla.c/.h` con padding por **caracteres UTF-8** (D19): `pantalla_ancho`, `pantalla_pad`, `pantalla_centro`, `pantalla_linea`.
- [x] L3.2 Implementar `torneo_mostrar_historial` (design §2.3) con el estilo D19: título `═`, cabecera + regla `─`, `Nombre (id N)`, estados en palabras (`Victoria`/`Empate`/`Pendiente`), KOs `X-Y`, subtítulo y pie con conteos de resueltos/pendientes.
- [x] L3.3 Añadir `case 9:` en `main.c` con helper `consultar_historial` (arma el torneo si falta; informa rechazo si ≠32, RF-TEC-03).
- [x] L3.4 Verificación: goldens `021_historial_sin_resultados` y `022_historial_con_resultados` (20/48) PASS; batería 21/4 (solo los 4 de combate por dato, pendientes de L6); `make` cero warnings. *(Nota: `run_tests.sh` ganó `pantalla.c` en `SRC_MODULOS` para los probes.)*

## Lote L4 — Persistencia de resultados (D16)

- [ ] L4.1 Añadir `resultados_sucios` en `main.c`; marcarla al aplicar resultados (teclado, archivo o simulación).
- [ ] L4.2 Ampliar `guardar_al_salir` para guardar resultados si `resultados_sucios` (usa `archivos_guardar_resultados`, ya existente).
- [ ] L4.3 Verificación: cargar resultados por teclado → salir → `data/resultados.txt` refleja los aplicados (round-trip recargable).

## Lote L5 — Fix 63/64 + round-trip de orden

- [ ] L5.1 En `torneo_aplicar_resultado`: finalizar solo cuando 63 y 64 tienen resultado; aplicar 64 antes de 63 mantiene `TORNEO_ELIMINATORIAS` (D14). Ajustar `mostrar_posiciones` para 63 pendiente.
- [ ] L5.2 En `archivos_guardar_entrenadores`: escribir ejemplares en orden inverso (D15) para round-trip estable.
- [ ] L5.3 Actualizar `tests/probes/probe_torneo_completo.c` para aplicar 63 antes de 64 (o ambos) y verificar posiciones completas.
- [ ] L5.4 Verificación: regresión completa + caso 64-antes-de-63 ya no bloquea el tercer lugar.

## Lote L6 — Blindar los tests de combate (D17) y cierre

- [ ] L6.1 Reescribir `tests/casos/012_combate_victoria` como `_sin_entrenadores`: registra 2 entrenadores + equipos por menú (opciones 2/3) y combate (8 → 1); verifica victoria determinista.
- [ ] L6.2 Reescribir `013_combate_empate` (autónomo): equipos que llegan a 20 turnos sin KO mutuo (D4).
- [ ] L6.3 Reescribir `014_combate_eliminatoria` (autónomo): verifica el anti-empate D5.
- [ ] L6.4 Reescribir `015_efectividad_sin_efecto` (autónomo): multiplicador ×0 E2E.
- [ ] L6.5 Conservar el equipo de Misty en `data/entrenadores.txt`; el archivo deja de ser entrada de la batería de combate.
- [ ] L6.6 Actualizar `docs/informe-tecnico.md` (D11–D18 + sección de simulación) y `docs/trazabilidad-requisitos.md` (RF-SIM-*, RF-RES-05, RF-RES-06, RF-MEN-01/02/03, RF-ELM-06, RF-ENT-03, RF-PRB-01).
- [ ] L6.7 `make` cero warnings; `bash tests/run_tests.sh` verde (20 casos + 2 probes); `verify-report.md` del change.

## Lote L7 — Rediseño de pantalla (UI/UX, D19)

- [ ] L7.1 Migrar la clasificación (opción 7, `torneo_mostrar_clasificacion`) al estilo D19 (tabla por grupo con `Nombre (id N)`, cabeceras, reglas).
- [ ] L7.2 Migrar los enfrentamientos/calendario (opción 8 → 2, `torneo_mostrar_enfrentamientos`) y el bracket (opción 10, `torneo_mostrar_resultados`).
- [ ] L7.3 Migrar la Pokédex (opción 1: mostrar todas, ficha de especie), entrenadores (opción 4) y equipos (opción 5).
- [ ] L7.4 Migrar la consulta del campeón (opción 11) y las posiciones finales con el mismo estilo.
- [ ] L7.5 Regenerar los goldens afectados (001–004, 016–020 y los que toquen pantalla) y correr la batería completa verde.

---

## Orden de implementación

L1 → L2 → L3 → L4 → L5 → L6 → L7. Cada lote compila limpio y, cuando toca salida, pasa la batería `diff` antes del siguiente. L1–L5 no tocan `data/`; L6 reescribe los tests de combate (dividible en L6a/L6b) y L7 migra las pantallas restantes regenerando sus goldens.

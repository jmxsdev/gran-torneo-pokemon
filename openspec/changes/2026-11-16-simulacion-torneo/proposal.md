# Propuesta: Simulación del torneo + historial de combates + cierres de persistencia

**Change**: `simulacion-torneo`
**Fase**: sdd-propose
**Fecha**: 2026-11-16
**Idioma del artefacto**: español neutro y profesional (requisito explícito del proyecto)

---

## Intención (Problema y objetivo)

El torneo determina al campeón solo si se cargan manualmente los 64 resultados (48 de grupos + 16 de eliminatoria). El motor de combate (`combate.c`) existe y funciona, pero **nunca se conecta al estado del torneo**: únicamente se usa en el "combate amistoso" (opción 8 → 1), interactivo y desconectado. El usuario espera poder **simular el torneo completo con solo cargar los entrenadores y sus equipos** ("organizar el torneo, procesar los enfrentamientos y determinar finalmente al campeón", PDF §1.6).

Además, la auditoría contra el PDF (2026-11-16) detectó cuatro defectos concretos:

1. **Opción 9 "Consultar historial de combates" sin implementar**: el `switch` de `main.c` no tiene `case 9`; cae al `default` y muestra "Opción 9 en construcción". Es la única función inconclusa del programa (reconocido en `docs/defensa-proyecto.tex`).
2. **Los resultados por teclado no se persisten**: `archivos_guardar_resultados()` existe pero **no tiene ningún punto de llamada**; el progreso de una sesión se pierde al salir (PDF §1.12/§1.16).
3. **Bug de orden 63/64**: si se aplica la final (64) antes del tercer lugar (63), el torneo pasa a `TORNEO_FINALIZADO` y el combate 63 queda bloqueado para siempre → tercero/cuarto lugar quedan `? (id 0)` (reproducido).
4. **El guardado de entrenadores invierte el orden de los equipos**: `equipo_agregar_ejemplar` inserta al frente de la lista enlazada y `archivos_guardar_entrenadores` recorre de cabeza a cola, por lo que cada guardado invierte el orden respecto al archivo cargado (round-trip no idempotente).

Objetivo: añadir la **simulación automática del torneo**, implementar la **opción 9**, conectar la **persistencia de resultados** y corregir los dos **defectos de integridad** (63/64 y orden de guardado), preservando el comportamiento observable actual salvo donde un cambio de salida sea inevitable y esté justificado.

## Alcance

### In Scope

1. **Simulación automática del torneo** (nueva capacidad `simulacion`):
   - Ejecutar los 64 combates (48 de grupos + 16 de eliminatoria) usando el motor `combate_ejecutar` existente, con un **seleccionador automático determinista** (primer Pokémon disponible) y **sin traza por consola** (modo silencioso), aplicando cada resultado con `torneo_aplicar_resultado` hasta coronar al campeón.
   - Requiere exactamente 32 entrenadores (D10); reutiliza la máquina de estados del torneo sin duplicar reglas.
   - **Relleno automático de equipos (D18)**: si un entrenador no tiene equipo válido, se le genera uno con `equipo_formar_backtracking` (restricciones por defecto deterministas) antes de simular. Así, basta con cargar los 32 entrenadores: los que vengan sin equipo se completan automáticamente.

2. **Opción 9 "Consultar historial de combates"** (modifica `menu` + `resultados`):
   - Nueva función `torneo_mostrar_historial` que lista los combates 1–64 con participantes, ronda/grupo, resultado, ganador y KOs; los pendientes se marcan explícitamente.

3. **Persistencia de resultados** (modifica `archivos` + `main`):
   - Conectar `archivos_guardar_resultados`: bandera `resultados_sucios` y guardado en `data/resultados.txt` al salir (opción 12) cuando se aplicó al menos un resultado en la sesión.

4. **Corrección del bug 63/64** (modifica `torneo` + `resultados`):
   - La transición a `TORNEO_FINALIZADO` debe requerir que **ambos** combates 63 y 64 tengan resultado; aplicar 64 antes de 63 ya no bloquea el tercer lugar.

5. **Corrección de la inversión de orden en el guardado** (modifica `archivos`):
   - `archivos_guardar_entrenadores` debe escribir en el orden que produce un round-trip estable (cargar → guardar → recargar deja el mismo orden de ejemplares).

6. **Blindar los tests de combate (decisión del usuario)**: los goldens 012–015 se reescriben como **tests autónomos** (`_sin_entrenadores`): registran sus propios entrenadores y equipos por el menú en lugar de depender de `data/entrenadores.txt`. Así, ninguna edición futura de ese archivo rompe la batería (mismo patrón ya usado por 005–011). Se **conserva el equipo nuevo de Misty** en `data/entrenadores.txt` (Opción A) sin necesidad de regenerar goldens.

### Out of Scope

- Interfaz gráfica, multijugador en red, movimientos/PP/IVs/EVs (ya fuera del alcance original).
- Cambiar la fórmula de daño (D1) o la derivación de stats (D2).
- Cambiar el número de entrenadores (sigue siendo exactamente 32, D10).
- Regenerar goldens innecesariamente: los tests de combate 012–015 se **reescriben como autónomos** (no se regeneran); el resto de goldens solo se toca ante un cambio de salida justificado.
- Rediseñar la fase de grupos o el bracket (49–64) — ya verificados contra el PDF.

## Capabilities

> Contrato entre propuesta y specs. Cada capacidad nueva → `openspec/specs/<name>/spec.md`.

### New Capabilities

- `simulacion` — ejecución automática de los 64 combates (con relleno automático de equipos por backtracking) hasta coronar al campeón (nuevo requisito derivado de PDF §1.6 "procesar los enfrentamientos" y §1.5 "construcción automática de equipos").

### Modified Capabilities

- `menu` — opción 9 "Consultar historial de combates" implementada (RF-MEN-01); punto de entrada de la simulación como submenú de la opción 6 "Cargar resultados".
- `equipos` — campo `nivel_objetivo` en `RestriccionesEquipo` para que el backtracking genere niveles medios (RF-EQP-04).
- `torneo` — finalización del torneo exige que 63 y 64 tengan resultado (corrige RF-ELM-05 sin cambiar el bracket).
- `resultados` — historial de combates (consulta de combates jugados con sus datos) y persistencia de resultados al salir.
- `entrenadores` — round-trip estable del archivo de entrenadores (RF-ENT-03): cargar → guardar no invierte el orden de los equipos.

## Enfoque por lotes (≤300 líneas/lote)

Presupuesto de sesión: **300 líneas/lote**. Estimaciones `additions` sin contar datos generados de gran volumen.

| Lote | Contenido | Líneas est. | ≤300 |
|------|-----------|-------------|------|
| L1 | Seleccionador automático + modo silencioso en `combate.c` (parámetro `silencioso` + callback de selección) | ≈ 120 | ✓ |
| L2 | `simulacion` en `torneo.c` (bucle 1–64, aplica resultados) + submenú de la opción 6 | ≈ 180 | ✓ |
| L3 | Opción 9: `torneo_mostrar_historial` + `case 9` en `main.c` | ≈ 130 | ✓ |
| L4 | Persistencia de resultados (`resultados_sucios`, guardado al salir) + fix orden de guardado | ≈ 120 | ✓ |
| L5 | Fix 63/64 (finalización exige 63+64) en `torneo.c`/`resultados.c` | ≈ 90 | ✓ |
| L6 | Blindar los tests de combate 012–015 (autónomos `_sin_entrenadores`) + docs + verificación | ≈ 500 (dividible en L6a/L6b) | ver nota |

Total ≈ 1 140 líneas en 6 lotes; solo L6 (blindado de tests) excede 300 y se divide en L6a/L6b si es necesario ⇒ sin PRs encadenados. La simulación reutiliza el motor existente; no se duplica lógica de combate ni de torneo.

```
Decision needed before apply: No (resuelta: conservar equipo de Misty + blindar tests de combate)
Chained PRs recommended: No
Chain strategy: single-pr
300-line budget risk: Bajo (L6 de blindado puede dividirse si excede el lote)
```

## Affected Areas

| Área | Impacto | Descripción |
|------|---------|-------------|
| `src/combate.c/.h` | Modificado | Parámetro `silencioso` (o función de traza suprimible) para la simulación |
| `src/equipo.c/.h` | Modificado | Campo `nivel_objetivo` en `RestriccionesEquipo` + lógica en `bt_rec` (niveles medios) |
| `src/simulacion.c/.h` | Nuevo | `simulacion_ejecutar` + `simulacion_preparar_equipos` (relleno por backtracking) + selector automático |
| `src/torneo.c/.h` | Modificado | `torneo_mostrar_historial` + finalización exige 63+64 |
| `src/resultados.c/.h` | Modificado | Historial (o delegación) + persistencia de resultados |
| `src/archivos.c` | Modificado | `archivos_guardar_entrenadores` round-trip estable; conectar `archivos_guardar_resultados` |
| `src/main.c` | Modificado | `case 9`; submenú de simulación en opción 6; bandera `resultados_sucios`; guardado al salir |
| `src/constantes.h` | Modificado | `COMBATES_GRUPO` público (48) para distinguir fase de grupos/eliminatoria |
| `data/entrenadores.txt` | Conservado | Se conserva el equipo nuevo de Misty; el orden deja de afectar la batería al blindar los tests |
| `tests/casos/012–015` | Reescrito | Tests de combate autónomos (`_sin_entrenadores`): registran sus propios entrenadores/equipos |
| `docs/informe-tecnico.md` | Modificado | Decisiones D11–D18 y nueva sección de simulación |

## Riesgos

| Riesgo | Probabilidad | Impacto | Mitigación |
|--------|--------------|---------|------------|
| La simulación con 6v6 y 20 turnos deriva en empates masivos en grupos (D4) | Media | Medio | Determinista y aceptado por D4; la eliminatoria resuelve con D5. Documentar distribución esperada |
| El modo silencioso altera accidentalmente la salida de los combates amistosos existentes | Baja | Medio | Parámetro independiente; el combate amistoso conserva la traza; gate `tests/run_tests.sh` |
| Cambiar la finalización (63+64) rompe el probe `probe_torneo_completo` que aplica 64 como último | Media | Alto | Actualizar el probe para aplicar 63 antes de 64 (o ambos) y verificar posiciones completas |
| Editar `data/entrenadores.txt` rompe los goldens de combate (012–015) | Media | Medio | Blindar: reescribir 012–015 como tests autónomos (`_sin_entrenadores`) que no leen ese archivo |
| La persistencia de resultados sobreescribe `data/resultados.txt` de ejemplo | Baja | Medio | Guardado solo con `resultados_sucios`; documentar el comportamiento en README |
| El relleno automático genera equipos nivel 1 (degenerados) o todos idénticos | Media | Bajo | Determinista y válido; alineado al comportamiento actual de la formación automática (ataque_total_min=0 → nivel 1). Variedad por entrenador es opcional (ventana de especies rotativa por posición) |

## Plan de rollback

Trabajo por commits, un commit por lote (L1→L6). Revertir cualquier lote con `git revert` del commit correspondiente; los lotes son independientes por dependencia. Los goldens y `data/` solo se modifican en L6 (consistencia), de modo que el blast radius de L1–L5 es solo código + specs + docs. El punto de entrada de la simulación es un submenú nuevo (no altera las 12 opciones), por lo que desactivarla = quitar la rama del submenú sin romper el resto.

## Dependencies

Ninguna externa. Solo `gcc -std=c99 -Wall -Wextra`, `make` y `tests/run_tests.sh`.

## Criterios de éxito

- [ ] **Simulación**: cargando 32 entrenadores (con o sin equipos), un comando rellena los equipos faltantes por backtracking, ejecuta los 64 combates y corona campeón sin intervención del usuario (PDF §1.5/§1.6).
- [ ] **Opción 9**: "Consultar historial de combates" lista los combates jugados con participantes, resultado, ganador y KOs; ya no responde "en construcción".
- [ ] **Persistencia**: los resultados cargados por teclado sobreviven al salir en `data/resultados.txt` (§1.12/§1.16).
- [ ] **Fix 63/64**: aplicar 64 antes de 63 no bloquea el tercer lugar; las 4 posiciones finales quedan definidas al finalizar.
- [ ] **Round-trip estable**: cargar → guardar entrenadores no invierte el orden de los equipos.
- [ ] `tests/run_tests.sh` verde (tests de combate 012–015 autónomos, sin dependencia de `data/entrenadores.txt`), `make` con **cero warnings**.
- [ ] Documentación (informe técnico D11–D18, trazabilidad) actualizada.

# Tareas: Gran Torneo Pokémon

**Change**: `gran-torneo-pokemon` · **Fase**: sdd-tasks · **Fecha**: 2026-09-01
**Idioma del artefacto**: español neutro y profesional (requisito explícito del proyecto).
**Entradas**: `proposal.md` (F0–F11), `design.md` (Engram #70), `specs/` (11 capacidades, 42 RF).

---

## Review Workload Forecast

Presupuesto de sesión: **800 líneas/lote** (el default del skill es 400; el orquestador lo fijó en 800). Cada lote = un único PR (`single-pr`). Estimaciones `additions` sin contar datos generados de gran volumen (`data/pokedex.txt`).

| Lote | Fase | Líneas est. | ≤800 | Contenido |
|---|---|---|---|---|
| 1 (HOY) | F0 | ≈ 650 | ✓ | Doxyfile, guía Doxygen, planificación, informe esqueleto, 10 headers, `main.c`, `Makefile` |
| 2 | F1 | ≈ 500 | ✓ | `tipos.c` + `pokedex.c` + `data/pokedex.txt` |
| 3 | F2 | ≈ 400 | ✓ | `entrenador.c` + `equipo.c` (ejemplares) + `entrenadores.txt` |
| 4 | F3 | ≈ 260 | ✓ | `combate.c` |
| 5 | F4 | ≈ 200 | ✓ | backtracking en `equipo.c` |
| 6 | F5 | ≈ 310 | ✓ | `torneo.c` (grupos) |
| 7 | F6 | ≈ 250 | ✓ | `torneo.c` (bracket) |
| 8 | F7 | ≈ 380 | ✓ | `resultados.c` + `archivos.c` |
| 9 | F8 | ≈ 280 | ✓ | `validacion.c` |
| 10 | F9 | ≈ 350 | ✓ | `tests/casos/*` + `run_tests.sh` |
| 11 | F10 | ≈ 300 | ✓ | informe final |
| 12 | F11 | ≈ 150 | ✓ | empaquetado |

Total ≈ 4 000 líneas en 12 PRs independientes. Ningún lote supera 800 ⇒ **no se requieren PRs encadenados** ni `size:exception`. F0 es el lote más pesado; si durante el apply tendiera a >750, recortar `docs/informe-tecnico.md` (los diagramas extra se difieren a F10).

```
Decision needed before apply: No
Chained PRs recommended: No
Chain strategy: pending
400-line budget risk: Low
```

## Decisiones pendientes (no bloquean F0)

- [ ] **Niveles 1–50 vs 1–100** (diseño §16): el spec fija 1–50. Si el docente confirma 1–100 ⇒ delta de spec (MODIFIED RF-EQP-05) + cambiar `NIVEL_MAX` antes del apply de F2.
- [ ] **Equipo de torneo 3 vs 6** (diseño §16): si se confirma 6 ⇒ delta menor de spec + `TAM_EQUIPO_TORNEO`.
- [ ] **D9 desempate adicional** (diseño §16): confirmar con el docente; documentar en `informe-tecnico.md`.

---

## Fase F0 — Convenciones + entorno + esqueleto (Día 1, lote de apply HOY)

- [x] F0.1 Crear `Doxyfile` con `OUTPUT_LANGUAGE=Spanish`, `INPUT_ENCODING=UTF-8`, `EXTRACT_ALL=YES` — DOC-01, diseño §10.
- [x] F0.2 Crear `docs/convenciones-doxygen.md` con bloques `@file/@brief/@param/@return/@author` en español, 99 cols — DOC-01, §10.
- [x] F0.3 Crear `docs/planificacion.md`: plan 14 días, estado hecho/pendiente por fase F0–F11 — DOC-02, §15.
- [x] F0.4 Crear `docs/informe-tecnico.md` esqueleto con semilla de diagramas Mermaid del diseño (módulos §1.1, combate §7, bracket §6.2) — DOC-03, §12.
- [x] F0.5 Crear `src/constantes.h` (`MAX_EQUIPO=6`, `TAM_EQUIPO_TORNEO=3`, `NIVEL_MIN/MAX`, `MAX_ENTRENADORES=32`, `POKEDEX_MAX=150`, `TOTAL_COMBATES=64`, `MAX_TURNOS_COMBATE=20`, `CANT_TIPOS=18`, buffers, rutas) — §1.2.
- [x] F0.6 Crear headers `src/{tipos,pokedex,equipo,entrenador,combate,torneo,resultados,archivos,validacion}.h` con structs y prototipos de §1.3/§2. **MINOR-1**: la matriz `efectividad[18][18]` NO se declara `static` en `tipos.h`; vive en `tipos.c` (F1), acceso solo vía `tipos_multiplicador()`.
- [x] F0.7 Crear `src/main.c`: menú de 12 opciones (RF-MEN-01); cada opción imprime «Opción X en construcción»; 12 = salir — §1.2.
- [x] F0.8 Crear `Makefile` con `gcc -std=c99 -Wall -Wextra -o build/torneo` (cero warnings como gate) — config.yaml `verify`, §13.
- [x] F0.9 Compilar limpio y primer lote de commits en español (`feat:`, `docs:`, `chore:`) — config.yaml `rules.commits`.

## Fase F1 — Pokédex + tipos + datos

- [ ] F1.1 Crear `src/tipos.c`: matriz `static float efectividad[18][18]` (MINOR-1), `tipos_inicializar`, `tipos_es_valido`, `tipos_a_texto`, `tipos_multiplicador` — RF-CMB-05/RF-PDX-03, §1.3.
- [ ] F1.2 Crear `data/pokedex.txt` con las 150 especies (formato §5.1, separador `;`) — RF-PDX-01/02/06, §5.1.
- [ ] F1.3 Crear `src/pokedex.c`: `pokedex_cargar` (150 líneas estrictas), búsquedas por número/nombre, mostrar — RF-PDX-01..06, §1.3.
- [ ] F1.4 Cablear opción 1 (consultar Pokédex) en `main` — RF-MEN-01, RF-TEC-02.
- [ ] F1.5 Verificación scriptada: carga 150, búsqueda existente/inexistente, archivo ausente — RF-PRB-01, §13/§8.2.

## Fase F2 — Entrenadores y ejemplares (separación especie/ejemplar)

- [ ] F2.1 Crear `src/entrenador.c`: registro con id único, búsqueda, listado — RF-ENT-01/02, §1.3.
- [ ] F2.2 Crear `src/equipo.c` (parte ejemplares): `equipo_crear_ejemplar` (D2), `equipo_agregar_ejemplar`, `equipo_validar` — RF-EQP-01/02/03/05, §2.
- [ ] F2.3 Crear `data/entrenadores.txt` de ejemplo (32) — RF-ENT-03, §5.2.
- [ ] F2.4 Cablear opciones 2/3/4/5 en `main` — RF-MEN-01.
- [ ] F2.5 Verificación: registro válido/duplicado/inválido, nivel 0/51 rechazado, tamaño excedido — RF-PRB-01, §13.

## Fase F3 — Combate

- [ ] F3.1 Crear `src/combate.c`: `combate_calcular_danio` (D1), `combate_ataca_primero` (D3), `combate_ejecutar` (bucle, KO, empate D4, anti-empate D5) — RF-CMB-01..05, §7/§3.
- [ ] F3.2 Cablear opción de combate en `main` — RF-MEN-01.
- [ ] F3.3 Verificación: multiplicadores 2/1/0.5/0 y productos ×4/×0.25, mínimo 1, empate por 20 turnos — RF-PRB-01, §8.2.

## Fase F4 — Backtracking de formación de equipos

- [ ] F4.1 En `src/equipo.c`: `equipo_formar_backtracking` + `bt_rec` con podas §4.3 — RF-EQP-04, §4.
- [ ] F4.2 Liberar ejemplares al retroceder (sin fugas) — §4.4.
- [ ] F4.3 Cablear opción de formación automática — RF-MEN-01.
- [ ] F4.4 Verificación: factible (3/100/2 tipos) y sin solución (6/nivel máx 5) — RF-EQP-04, §4.

## Fase F5 — Torneo: grupos + clasificación

- [ ] F5.1 Crear `src/torneo.c`: `torneo_armar_grupos` (exige 32), calendario 1–48, `torneo_aplicar_resultado` (3/1/0), `torneo_ordenar_grupos` (RF-TRN-04 completo) — RF-TRN-01..06, §6.
- [ ] F5.2 Cablear opciones 7 (clasificación) y 8 (enfrentamientos) — RF-MEN-01, RF-CLS-01.
- [ ] F5.3 Verificación: rechazo ≠32, round-robin 6/grupo, desempates por victorias/derrotados/directo/id — RF-PRB-01, §13.

## Fase F6 — Eliminatorias (bracket 49–64)

- [ ] F6.1 En `src/torneo.c`: tabla `ORIGEN_PARTICIPANTE[64][2]`, `torneo_participantes` (RF-RES-03), bracket §6.2, transición GRUPOS→ELIMINATORIAS→FINALIZADO, posiciones finales — RF-ELM-01..05, §6.2.
- [ ] F6.2 Cablear opciones 10/11 (resultados, campeón) — RF-MEN-01.
- [ ] F6.3 Verificación: emparejamientos fijos, encadenado G#/P#, campeón = G64 — RF-PRB-01, §13.

## Fase F7 — Archivos y resultados

- [ ] F7.1 Crear `src/resultados.c`: carga teclado/archivo, `resultados_validar` (catálogo §8.1) — RF-RES-01/02/04, §8.1.
- [ ] F7.2 Crear `src/archivos.c`: guardar/cargar entrenadores, cargar resultados, guardar clasificación — RF-ENT-03/RF-CLS-01, §5.2–5.4.
- [ ] F7.3 Crear `data/resultados.txt` parcial de ejemplo — RF-RES-01, §5.3.
- [ ] F7.4 Verificación: parciales, entrenador inexistente, empate en 49–64, eliminado reaparece, participante manual — RF-RES-02, §8.1.

## Fase F8 — Validación integral de entradas

- [ ] F8.1 Crear `src/validacion.c`: `validar_leer_entero`, `validar_leer_cadena` con reintentos — RF-TEC-03, §1.3.
- [ ] F8.2 Sustituir lecturas directas de `main` por `validacion` (ninguna validación llama `exit`) — RF-TEC-03, §8.1.
- [ ] F8.3 Verificación E2E rutas inválidas: opción 99, nivel 51, id duplicado, archivo ausente (programa sigue vivo) — RF-TEC-03, §13.

## Fase F9 — Pruebas completas

- [ ] F9.1 Crear `tests/run_tests.sh` (compila, ejecuta stdin, `diff`) — §8.2.
- [ ] F9.2 Crear `tests/casos/*` cubriendo RF-PRB-01 completo — RF-PRB-01.
- [ ] F9.3 Caso de integración: torneo completo 32→48→clasificación→49–64→campeón — §13.
- [ ] F9.4 Gate: batería completa verde, cero warnings — config.yaml `verify`.

## Fase F10 — Documentación final

- [ ] F10.1 Completar `docs/informe-tecnico.md` con D1–D10 justificados + diagramas finales — DOC-03, §3.
- [ ] F10.2 Actualizar `docs/planificacion.md` a 14/14 — DOC-02.
- [ ] F10.3 Verificar que Doxygen compila sin warnings — DOC-01.

## Fase F11 — Entrega

- [ ] F11.1 Empaquetar `.c`/`.h`/`main.c`/`data`/`tests`/`docs`/`Doxyfile` en un comprimido único — RF-PRB-02.
- [ ] F11.2 Verificar compilación y ejecución sin dependencias externas — RF-PRB-02.
- [ ] F11.3 Checklist de entrega y cierre en `planificacion.md` — DOC-02.

---

## Orden de implementación

F0 hoy (esqueleto + convenciones). Después F1→F11 por dependencias (tipos→pokedex→ejemplares→combate→backtracking→grupos→bracket→resultados/archivos→validación→pruebas→docs→entrega). Cada fase compila limpio y pasa su batería `diff` antes de la siguiente; gate de fase en `tests/run_tests.sh`.

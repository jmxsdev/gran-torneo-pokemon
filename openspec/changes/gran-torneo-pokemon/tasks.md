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

## Decisiones pendientes (no bloquean F0) — RESUELTAS (2026-09-19)

El docente respondió: **libertad total de modelado** (niveles, tamaño de equipo y reglas a conveniencia) y fórmula oficial NO exigida — se admite una fórmula simplificada documentada y validada con el nivel. Valores adoptados:

- [x] **Niveles 1–50 vs 1–100** → **1–100** (canónico). Delta de spec aplicado (MODIFIED RF-EQP-05 + escenario nivel 0/101) y `NIVEL_MAX = 100` en `constantes.h`.
- [x] **Equipo de torneo 3 vs 6** → **6**: el torneo combate con el equipo completo (máximo `MAX_EQUIPO`); `TAM_EQUIPO_TORNEO = 6` y delta menor de spec aplicado.
- [x] **D9 desempate adicional** → decidido por el equipo con la libertad otorgada por el docente: enfrentamiento directo en grupos + id menor; documentado en `informe-tecnico.md`.
- [x] **Fórmula de stats (D2)** → fórmula simplificada con variación determinista por id: `(base × nivel / 50) + nivel + 10` (HP) / `+ 5` (resto) + `(id × 7) % 16`; documentada en `design.md` §3 e `informe-tecnico.md`.

---

## Fase F0 — Convenciones + entorno + esqueleto (Día 1, lote de apply HOY)

- [x] F0.1 Crear `Doxyfile` con `OUTPUT_LANGUAGE=Spanish`, `INPUT_ENCODING=UTF-8`, `EXTRACT_ALL=YES` — DOC-01, diseño §10.
- [x] F0.2 Crear `docs/convenciones-doxygen.md` con bloques `@file/@brief/@param/@return/@author` en español, 99 cols — DOC-01, §10.
- [x] F0.3 Crear `docs/planificacion.md`: plan 14 días, estado hecho/pendiente por fase F0–F11 — DOC-02, §15.
- [x] F0.4 Crear `docs/informe-tecnico.md` esqueleto con semilla de diagramas Mermaid del diseño (módulos §1.1, combate §7, bracket §6.2) — DOC-03, §12.
- [x] F0.5 Crear `src/constantes.h` (`MAX_EQUIPO=6`, `TAM_EQUIPO_TORNEO=3`, `NIVEL_MIN/MAX`, `MAX_ENTRENADORES=32`, `POKEDEX_MAX=150`, `TOTAL_COMBATES=64`, `MAX_TURNOS_COMBATE=20`, `CANT_TIPOS=18`, buffers, rutas) — §1.2. *(Actualizado 2026-09-19: `TAM_EQUIPO_TORNEO=6` y `NIVEL_MAX=100` por decisiones docentes resueltas.)*
- [x] F0.6 Crear headers `src/{tipos,pokedex,equipo,entrenador,combate,torneo,resultados,archivos,validacion}.h` con structs y prototipos de §1.3/§2. **MINOR-1**: la matriz `efectividad[18][18]` NO se declara `static` en `tipos.h`; vive en `tipos.c` (F1), acceso solo vía `tipos_multiplicador()`.
- [x] F0.7 Crear `src/main.c`: menú de 12 opciones (RF-MEN-01); cada opción imprime «Opción X en construcción»; 12 = salir — §1.2.
- [x] F0.8 Crear `Makefile` con `gcc -std=c99 -Wall -Wextra -o build/torneo` (cero warnings como gate) — config.yaml `verify`, §13.
- [x] F0.9 Compilar limpio y primer lote de commits en español (`feat:`, `docs:`, `chore:`) — config.yaml `rules.commits`.

## Fase F1 — Pokédex + tipos + datos

- [x] F1.1 Crear `src/tipos.c`: matriz `static float efectividad[18][18]` (MINOR-1), `tipos_inicializar`, `tipos_es_valido`, `tipos_a_texto`, `tipos_multiplicador` — RF-CMB-05/RF-PDX-03, §1.3.
- [x] F1.2 Crear `data/pokedex.txt` con las 150 especies (formato §5.1, separador `;`) — RF-PDX-01/02/06, §5.1.
- [x] F1.3 Crear `src/pokedex.c`: `pokedex_cargar` (150 líneas estrictas), búsquedas por número/nombre, mostrar — RF-PDX-01..06, §1.3.
- [x] F1.4 Cablear opción 1 (consultar Pokédex) en `main` — RF-MEN-01, RF-TEC-02.
- [x] F1.5 Verificación scriptada: carga 150, búsqueda existente/inexistente, archivo ausente — RF-PRB-01, §13/§8.2.

## Fase F2 — Entrenadores y ejemplares (separación especie/ejemplar)

- [x] F2.1 Crear `src/entrenador.c`: registro con id único, búsqueda, listado — RF-ENT-01/02, §1.3. *(2026-09-19: validación robusta: id inválido, nombre vacío, duplicado; fix de F0: tag `struct Entrenador` en entrenador.h para completar la declaración adelantada de equipo.h.)*
- [x] F2.2 Crear `src/equipo.c` (parte ejemplares): `equipo_crear_ejemplar` (D2 exacta: variación `(id*7)%16`, HP `base*nivel/50+nivel+10+v`, resto `+5`, división entera truncada, `hp_actual=hp_max`), `equipo_agregar_ejemplar`, `equipo_validar` (tamaño 1–6, especies, niveles 1–100, tipos), `equipo_liberar` (free disciplinado) — RF-EQP-01/02/03/05, §2. *(Firmas ajustadas al lote: `equipo_crear_ejemplar(pd, numero_especie, nombre, nivel, id)` valida RF-EQP-01 en origen; `equipo_validar(pd, ent, tamano)`; se añadieron `equipo_contar`, `equipo_mostrar`, `equipo_siguiente_id` y `equipo_fijar_contador_id` para el id único global; la especie NO se muta.)*
- [x] F2.3 Crear `data/entrenadores.txt` de ejemplo (32) — RF-ENT-03, §5.2. *(Formato: 3 campos base ID;NOMBRE;CANT + 4 por ejemplar ID_EJEMPLAR;NUM_ESPECIE;APODO;NIVEL ⇒ 3+4·CANT campos/línea, máx 27; niveles variados 5–50 que demuestran D2.)*
- [x] F2.4 Cablear opciones 2/3/4/5 en `main` — RF-MEN-01. *(Carga inicial de entrenadores.txt opcional con aviso; creación por número de especie + nivel 1–100; liberación de equipos al salir.)*
- [x] F2.5 Verificación: registro válido/duplicado/inválido, nivel 0/101 rechazado, tamaño excedido — RF-PRB-01, §13. *(Batería scriptada F2.5: 7 casos PASS + probe D2; make cero warnings; detalle en apply-progress.)*

## Fase F3 — Combate

- [x] F3.1 Crear `src/combate.c`: `combate_calcular_danio` (D1), `combate_ataca_primero` (D3), `combate_ejecutar` (bucle, KO, empate D4, anti-empate D5) — RF-CMB-01..05, §7/§3. *(2026-09-21: 365 líneas; D1 exacta (base entera + multiplicador, mínimo 1 / sin efecto 0); D3 mayor velocidad con empate al entrenador 1; `combate_ejecutar` gana el parámetro `es_eliminatoria` y un callback `CombateSeleccionar` (el motor no lee consola); restaura `hp_actual = hp_max` al iniciar (D2); traza de turnos/daños/KOs; fin por agotamiento, empate en grupos (D4) o cadena HP→nivel→entrenador 1 (D5).)*
- [x] F3.2 Cablear opción de combate en `main` — RF-MEN-01. *(2026-09-21: la opción 8 «Consultar enfrentamientos» abre un submenú de combate amistoso (fase de grupos / eliminatoria); seleccionador de Pokémon activo por posición 1..N con reintentos y EOF ordenado; validación de entrenadores existentes, distintos y con equipo válido. **F5.2 (2026-09-21)**: la opción 8 pasa a submenú «1. Combate amistoso / 2. Calendario del torneo (combates 1-48) / 0. Volver»; el combate amistoso de F3 queda accesible en el submenú 8→1 sin cambios en `combate.c` ni en la batería F3 de combate (los casos de menú de la batería F3 que entraban a la opción 8 con «8\n1» ahora requieren «8\n1\n1»).)*
- [x] F3.3 Verificación: multiplicadores 2/1/0.5/0 y productos ×4/×0.25, mínimo 1, empate por 20 turnos — RF-PRB-01, §8.2. *(2026-09-21: batería scriptada 16/16 PASS + probe F3 21/21 (D1 exacto por caso, D3, D4, D5a/b/c, agotamiento con y sin reemplazo); make cero warnings; pokedex.txt/efectividad.txt intactos.)*

## Fase F4 — Backtracking de formación de equipos

- [x] F4.1 En `src/equipo.c`: `equipo_formar_backtracking` + `bt_rec` con podas §4.3 — RF-EQP-04, §4. *(2026-09-21: recursión real sobre el índice de especie; ramifica sobre k copias (1..restantes si `permitir_repetidas`, 1 si no) y luego excluye; podas EXACTAS §4.3.1/4.3.2/4.3.3 (cantidad con `pd->cantidad` como límite real, cota inferior `nivel_acum + restantes*NIVEL_MIN`, cota de tipos con tabla sufijo `uint32_t[POKEDEX_MAX+1]` O(18·150) y máscara de bits — el diseño §4.2 menciona "máscara de tipos", reemplaza el arreglo bool §4.1) + salida temprana §4.3.5; se añade la poda extra 3b (`tipos_dist + 2*restantes < min_tipos`: un ejemplar aporta a lo sumo 2 tipos) que acota el peor caso "min_tipos alto" que §4.5 daba por cubierto y no lo está; nivel del ejemplar §4.2: mínimo factible (NIVEL_MIN) y escala al máximo del presupuesto si hay `ataque_total_min`; restricciones de integrantes: lista `especies_permitidas`/`cantidad_permitidas` añadida al struct (contrato F4.1, el diseño §2 no la definía) + `permitir_repetidas`; `equipo_asignar` encapsula la lista generada en el entrenador sin romper el TDA.)*
- [x] F4.2 Liberar ejemplares al retroceder (sin fugas) — §4.4. *(2026-09-21: cada copia creada en una rama se libera con free si la rama no conduce a solución; invariante `*creados` (vivos) vuelve a 0 tras cada fracaso; al éxito la lista es propiedad del llamador y main la libera con `equipo_liberar` al salir; revisión manual 1:1 de cada malloc/crear ejemplar → free.)*
- [x] F4.3 Cablear opción de formación automática — RF-MEN-01. *(2026-09-21: la opción 3 "Crear equipo" abre submenú 1=manual (flujo previo intacto), 2=automática (backtracking), 0=volver; restricciones con validación y reintentos: cantidad 1..6, nivel total positivo, tipos 1..18; si no existe solución informa "No existe un equipo que cumpla las restricciones indicadas." sin devolver equipo inválido y conserva el equipo anterior; `leer_entero` pasa a triestado (1/0/-1) — cierra el hallazgo LOW de F2 (EOF en bucles internos colgaba el programa).)*
- [x] F4.4 Verificación: factible (3/100/2 tipos) y sin solución (6/nivel máx 5) — RF-EQP-04, §4. *(2026-09-21: probe F4 24/24 PASS (factible cumple TODAS las restricciones, sin solución con salida NULL y poda en 3 µs, min_tipos 18 inalcanzable con poda 3b en 2 µs, especies permitidas + repetidas Bulbasaur×3, ataque objetivo 150 con niveles escalados, restricciones inválidas rechazadas); batería F4 12/12 PASS (factible mostrado, sin solución en 8 ms con medición de tiempo, validaciones con reintento, entrenador inexistente, opción inválida, EOF ordenado en submenú y restricciones, sha256 datos intactos, ≤99 cols); `make clean && make` cero warnings.)*

## Fase F5 — Torneo: grupos + clasificación

- [x] F5.1 Crear `src/torneo.c`: `torneo_armar_grupos` (exige 32), calendario 1–48, `torneo_aplicar_resultado` (3/1/0), `torneo_ordenar_grupos` (RF-TRN-04 completo) — RF-TRN-01..06, §6. *(2026-09-21: 441 líneas. `torneo_armar_grupos(t, reg)` exige exactamente 32 (D10) y distribuye por orden de registro: posición k ⇒ grupo k/4, puesto k%4; calendario con pares fijos (m0,m1)(m0,m2)(m0,m3)(m1,m2)(m1,m3)(m2,m3) y número 6·grupo+par+1 ⇒ A=1..6 … H=43..48; inicializa los 64 combates (49–64 pendientes para F6). `torneo_aplicar_resultado(t, reg, r, msg, n)` valida: estado GRUPOS, número 1..48 (49–64 rechazados hasta F6), no duplicado, participantes == resueltos por el sistema (RF-RES-03), resultado y ganador coherentes, KOs ≥ 0; aplica 3/1/0 (RF-TRN-03) y acumula KOs en `pokemon_derrotados`; al completar 48/48 ordena y define clasificados (RF-TRN-06) sin transicionar a ELIMINATORIAS (eso es F6). `torneo_ordenar_grupos`/`torneo_clasificados` comparten `clasificar_grupos`: orden por puntos desc → victorias desc → pokemon_derrotados desc → enfrentamiento directo (D9) → id asc (RF-TRN-04); las estadísticas se derivan de los combates (fuente de verdad), no de contadores sueltos. Se añaden a `torneo.h` `torneo_clasificados`, `torneo_mostrar_clasificacion` y `torneo_mostrar_enfrentamientos` (RF-CLS-01).)*
- [x] F5.2 Cablear opciones 7 (clasificación) y 8 (enfrentamientos) — RF-MEN-01, RF-CLS-01. *(2026-09-21: opción 7 arma el torneo al vuelo si faltaba (rechazo con conteo si ≠32, D10) y muestra la tabla por grupo; opción 8 pasa a submenú «1. Combate amistoso (migrado de F3) / 2. Calendario del torneo (combates 1-48) / 0. Volver»; el calendario imprime NUM;GRUPO;ID1;NOMBRE1;ID2;NOMBRE2 para 1..48. Migración del combate amistoso documentada en F3.2: sigue accesible en 8→1, la batería F3 de combate queda intacta en el motor y los casos de menú adaptan un nivel de submenú.)*
- [x] F5.3 Verificación: rechazo ≠32, round-robin 6/grupo, desempates por victorias/derrotados/directo/id — RF-PRB-01, §13. *(2026-09-21: probe F5 104/104 PASS + batería F5 17/17 PASS; detalle en apply-progress.)*

## Fase F6 — Eliminatorias (bracket 49–64)

- [x] F6.1 En `src/torneo.c`: tabla `ORIGEN_PARTICIPANTE[64][2]`, `torneo_participantes` (RF-RES-03), bracket §6.2, transición GRUPOS→ELIMINATORIAS→FINALIZADO, posiciones finales — RF-ELM-01..05, §6.2. *(2026-09-21: `ORIGEN_PARTICIPANTE[64][2]` con fuentes CLASIFICADO (índice en `id_clasificados`) para 49–56 y GANADOR/PERDEDOR (combate fuente) para 57–64; filas 1–48 en ORIGEN_NINGUNO (el calendario las fija). `torneo_participantes` resuelve los ids contra el estado real (0 si una fuente aún no existe ⇒ «combate no disponible aún»). `torneo_aplicar_resultado` habilita 49–64: exige estado ELIMINATORIAS, participantes == resueltos (RF-RES-03), sin empates (RF-ELM-01), ganador ∈ {id1, id2} (V1/V2), KOs ≥ 0; persiste los participantes resueltos en el combate (el perdedor de una fuente P# los lee); transición GRUPOS→ELIMINATORIAS al completar 48/48 (ordena, define clasificados y resuelve octavos 49–56) y →FINALIZADO al aplicar el 64 con posiciones (campeón G64, subcampeón P64, tercero G63, cuarto P63). La eliminatoria no toca los contadores de grupos (RF-TRN-03 puntúa solo 1–48).)*
- [x] F6.2 Cablear opciones 10/11 (resultados, campeón) — RF-MEN-01. *(2026-09-21: opción 10 «Mostrar resultados del torneo» → `torneo_mostrar_resultados` (bracket 49–64 con ronda, etiqueta de origen 1A/G49/P61, participantes y ganador por combate; posiciones finales si terminó); opción 11 «Mostrar campeón» → `torneo_mostrar_campeon` (avisa si el torneo sigue en curso); las opciones previas 1–5, 7, 8 y 12 intactas; la 6 (cargar resultados) se completa en F7.)*
- [x] F6.3 Verificación: emparejamientos fijos, encadenado G#/P#, campeón = G64 — RF-PRB-01, §13. *(2026-09-21: probe F6 43/43 PASS + batería F6 13/13 PASS (incluye regresión probe F5 104/104 y batería F5 17/17); detalle en apply-progress.)*

## Fase F7 — Archivos y resultados

- [x] F7.1 Crear `src/resultados.c`: carga teclado/archivo, `resultados_validar` (catálogo §8.1) — RF-RES-01/02/04, §8.1. *(2026-09-21: `resultados_validar(t, reg, r, msg, n)` con el catálogo completo §8.1 y mensajes específicos por caso (combate inválido, torneo sin armar/finalizado, ronda incorrecta, duplicado, entrenador inexistente, participantes ≠ resueltos RF-RES-03 — cubre eliminado que no reaparece y participante manual —, fuente del bracket sin resolver, resultado inválido, empate en eliminatoria RF-ELM-01, KOs negativos, ganador incoherente V1/V2/E); teclado con participantes mostrados por el sistema (el usuario solo declara resultado y KOs) y archivo con esquema §5.3 (5 o 7 campos: KOs opcionales); `resultados_mostrar_pendientes` para parciales; lectores locales triestado (patrón de main.c) hasta la unificación en validacion (F8).)*
- [x] F7.2 Crear `src/archivos.c`: guardar/cargar entrenadores, cargar resultados, guardar clasificación — RF-ENT-03/RF-CLS-01, §5.2–5.4. *(2026-09-21: `archivos_guardar_entrenadores` (formato §5.2, stats no persistidas — se re-derivan con D2 y quedan idénticas por id), `archivos_cargar_resultados` (delega en resultados_cargar_archivo), `archivos_guardar_resultados` (§5.3 con "-" para empates, recargable y byte-idéntico al cargar+guardar) y `archivos_guardar_clasificacion` (formato §5.4 EXACTO: cabecera [GRUPO X] + POS;ID;NOMBRE;V;E;D;P, SIN la columna derrotados que sí muestra la pantalla; firma extendida con `const RegistroEntrenadores *` para los nombres — desviación menor documentada, patrón de F6 —; orden compartido con la pantalla vía la nueva `torneo_clasificacion_grupo`).)*
- [x] F7.3 Crear `data/resultados.txt` parcial de ejemplo — RF-RES-01, §5.3. *(2026-09-21: 20 de los 48 combates de grupos (A:1–6, B:7–12, C:13–18, D:19–20), formato §5.3 exacto, consistente con data/entrenadores.txt; sha256 e08bdc4e… estable tras cargar+guardar.)*
- [x] F7.4 Verificación: parciales, entrenador inexistente, empate en 49–64, eliminado reaparece, participante manual — RF-RES-02, §8.1. *(2026-09-21: probe F7 39/39 PASS (parciales 20/48, clasificación parcial coherente A=2,1,4,3 B=5,7,6,8 C=9,11,12,10 D=15,13,14,16, rechazos con mensajes específicos, round-trip resultados byte-idéntico, guardar/recargar entrenadores con stats D2 idénticas, formato §5.4 exacto) + batería F7 13/13 + regresión completa F5/F6 (probe F6 43/43, batería F6 13/13, probe F5 104/104, batería F5 17/17); make limpio cero warnings (9 .c); sha256 de los 4 datos intactos; ≤99 columnas.)*

## Fase F8 — Validación integral de entradas

- [x] F8.1 Crear `src/validacion.c`: `validar_leer_entero`, `validar_leer_cadena` con reintentos — RF-TEC-03, §1.3. *(2026-09-21: `validacion.c` (129 líneas) implementa `validar_leer_entero` (rango + reintentos, EOF ⇒ 0), `validar_leer_entero_msg` (mismo con mensaje específico por prompt — extensión de F8 para conservar los mensajes que la batería F4 verifica), `validar_leer_cadena` (bool: false ante EOF — desviación LOW de la firma void del diseño §1.3, exigida por el contrato "retorna estado") y `validar_separar_campos` (división por ';' sin omitir campos vacíos — cierra SUGGESTION F2). Garantía §8.1: ninguna función llama a `exit` (verificado con grep en la batería F8).)*
- [x] F8.2 Sustituir lecturas directas de `main` por `validacion` (ninguna validación llama `exit`) — RF-TEC-03, §8.1. *(2026-09-21: `main.c` elimina `leer_opcion`/`leer_linea`/`descartar_linea`/`leer_entero` locales y migra TODAS sus lecturas a `validar_*` (menú 1-12, submenús 0-2/0-4, ids, especie, nivel, posición, apodo/nombre); `resultados.c` migra teclado y archivo; los lectores locales triestado desaparecen. EOF en 0 es el marcador de cierre ordenado; donde 0 es un valor legítimo (KOs, ids) el llamador distingue con `feof(stdin)`. CIERRE DE HALLAZGOS: (a) submenú de la opción 1 avisa «La Pokédex no está cargada.» en TODAS sus ramas cuando `cantidad == 0` (WARNING F1); (b) parsers de datos (pokedex, entrenadores, resultados, efectividad) detectan campos vacíos `;;` con `validar_separar_campos` en lugar del desplazamiento silencioso de `strtok` (SUGGESTION F2); (c) ningún bucle de lectura sin EOF persiste (batería F8: EOF en restricciones/selección/teclado cierra ordenado con timeout).)*
- [x] F8.3 Verificación E2E rutas inválidas: opción 99, nivel 101, id duplicado, archivo ausente (programa sigue vivo) — RF-TEC-03, §13. *(2026-09-21: batería F8 15/15 PASS — opción 99, texto en menú, nivel 101/0 con reintento, id duplicado, Pokédex ausente (todas las ramas avisan), EOF a mitad de flujo (restricciones, selección de Pokémon, teclado de resultados) → el programa sigue vivo o sale ordenado con 0; `make clean && make` cero warnings (10 .c); regresión completa: probe F7 39/39, batería F7 13/13, batería F6 13/13, batería F5 17/17, probe F6 43/43, probe F5 104/104, batería F4 12/12, probe F3 21/21 (motor); sha256 de los 4 datos intactos; ≤99 columnas. NOTA: `bateria_f3.sh` quedó obsoleta desde F5 (sus casos de menú usan la opción 8 directa de F3; el submenú llegó en F5.2) — verificado que ya fallaba 12/12 sobre el código F7 previo a F8; el motor de combate se regresa con el probe F3.)*

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

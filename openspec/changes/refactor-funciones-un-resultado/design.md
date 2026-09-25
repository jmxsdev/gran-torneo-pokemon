# Design: Refactor — funciones de un solo resultado

**Change**: `refactor-funciones-un-resultado` · **Fase**: sdd-design · **Fecha**: 2026-09-25
**Idioma**: español (requisito del proyecto) · **Inputs**: `proposal.md`, `specs/convencion-funciones/spec.md`, código real (firmas y 58 llamadas inventariadas en `src/*.c` y `tests/probes/*.c`).

---

## 1. Enfoque técnico

Refactor mecánico, sin cambio observable: 23 funciones que devolvían un valor y además mutaban almacenamiento visible al llamador pasan a ser procedimientos `void` con salidas por referencia. El patrón canónico (RF-CNV-02) es:

```c
/* Antes: dos resultados (return + out/archivo/contador) */
bool f(..., X *out);
/* Después: procedimiento, éxito al FINAL */
void f(..., X *out, bool *exito);

/* Llamador */
bool exito;
f(..., &out, &exito);
if (!exito) { /* mensaje idéntico al actual */ }
```

Reglas del refactor:

1. **Cuerpo**: cada `return V;` se traduce a `*exito = V; return;` (o `*n`, `*id`, `*ganador` según la salida). Toda ruta de salida escribe la salida antes de retornar; sin `return` con valor.
2. **Llamadores**: la expresión `f(...)` se separa en llamada + uso de la variable local (`exito`, `n`, `id`, `ganador`). Los `if (!f(...))` pasan a `if (!exito)`; los mensajes, `printf` y orden de sentencias NO cambian.
3. **Alcance cerrado**: no se renombran funciones, no se tocan `data/` ni los 20 `.esperado`, no se agregan parámetros que no sean la salida nueva.
4. **Una variable por ámbito**: `bool exito;` se declara en la función llamadora (una vez), no por llamada; donde el retorno se ignoraba hoy se conserva el ignore con `(void)exito;`.

El orden de aplicación es por lotes L1→L4 (header + implementación + TODOS sus llamadores en el mismo commit, para que cada lote compile limpio), y L5 cierra probes, docs y batería completa.

---

## 2. Decisiones de arquitectura

### D-A: `exito` al final y en español

**Choice**: `void f(..., X *out, bool *exito)`, con `exito` como último parámetro; en `tipos_es_valido` se llama `es_valido` por coherencia semántica; en `validar_separar_campos` es `int *n`; en `equipo_siguiente_id`, `int *id`; en `combate_atacar`, `int *ganador`.
**Alternatives**: `bool *ok` (inglés, como en el texto genérico del spec); éxito antes de los out; struct de retorno.
**Rationale**: RF-CNV-02 fija el patrón `T *out` + éxito al final; `apply.guidelines` de `openspec/config.yaml` exige identificadores en español. Agrupar todos los out seguidos del éxito da llamadas uniformes y permite `if (!exito)` inmediato. El struct de retorno introduciría un tipo nuevo y copias, sin ganancia.

### D-B: `validar_separar_campos`, `equipo_siguiente_id` y `combate_atacar` sin `bool *exito`

**Choice**: conservan una única salida entera con dominio de valores: `n` (cantidad de campos; 0 = entrada inválida), `id` (siempre éxito), `ganador` = -1 cancelado / 0 continúa / id ganador.
**Alternatives**: añadir `bool *exito` a los tres; dividir `combate_atacar` en dos funciones.
**Rationale**: el spec anexo fija `int *n`, `int *id` e `int *ganador`; son procedimientos ya multi-salida y el entero agota la información (un `exito` siempre verdadero o redundante con `n==0` sería ruido). Para `combate_atacar`, el dominio {-1,0,id} es el contrato actual y se documenta en su bloque Doxygen; añadir un cuarto estado duplicaría la señal de cancelación.

### D-C: `bt_rec` recursivo con `bool *exito`

**Choice**: `static void bt_rec(..., Ejemplar **parcial, int *creados, bool *exito)`. Cada caso base/poda escribe `*exito = false; return;`; el caso base de éxito escribe la condición; las llamadas recursivas reenvían el MISMO puntero y el padre hace `if (*exito) return;` para propagar la primera solución; el bucle de retroceso (`free` de las j copias) queda intacto.
**Alternatives**: devolver la solución por `Ejemplar **` (NULL = sin solución) — confunde "sin solución" con "equipo vacío" y rompe la propagación de la primera solución; usar una variable `static` — estado global innecesario.
**Rationale**: `bool *exito` es la traducción 1:1 del `return bool` actual (salida temprana incluida) y mantiene el algoritmo, las podas §4.3 y la liberación al retroceder §4.4 sin tocar su lógica.

### D-D: `guardar_*` cuentan como resultado (archivo escrito)

**Choice**: `archivos_guardar_entrenadores/resultados/clasificacion` pasan a `void (..., bool *exito)`.
**Alternatives**: dejarlas `bool` argumentando que el archivo es efecto colateral.
**Rationale**: decisión confirmada por el docente (2026-09-25, proposal): el archivo escrito es una segunda salida. `archivos_guardar_resultados` no tiene llamadores hoy, pero se convierte igual para que la API quede 100 % conforme.

### D-E: Qué NO se convierte

No se convierten las funciones puras o de retorno único (sección 4): su valor de `return` ES el único resultado y no mutan almacenamiento del llamador. `entrenador_buscar` recibe `RegistroEntrenadores *` no-const por su firma histórica, pero no escribe (verificado); `validar_leer_entero*` consumen stdin (I/O inherente a su operación, como un `printf`), pero su resultado observable es el entero retornado.

### D-F: `tipos_es_valido` doble llamada (sin cortocircuito)

**Choice**: en `pokedex.c` se llama dos veces con `bool ok1, ok2` y luego `if (!ok1 || !ok2)`, en vez de `if (!f(...) || !f(...))`.
**Alternatives**: conservar el cortocircuito con `if (!ok1) {...} else { f(segundo, &ok2); ... }`.
**Rationale**: el cuerpo de `tipos_es_valido` solo escribe `*salida` en éxito (verificado en `tipos.c:229-247`), así que llamar siempre la segunda vez no altera `esp->tipo_secundario` ni la salida por consola (la rama de error imprime el mismo mensaje y hace `continue`). Mantiene el llamador plano.

---

## 3. Tabla de conversión exacta (23)

Convención de la tabla: `…` = parámetros actuales sin cambios. Llamadas = expresiones de llamada en `.c` (excluye definiciones y docs).

| # | Lote | Función | Antes | Después | Llamadas |
|---|------|---------|-------|---------|----------|
| 1 | L1 | `validar_leer_cadena` | `bool (const char *, char *, size_t)` | `void (…, bool *exito)` | 3 (main) |
| 2 | L1 | `validar_separar_campos` | `int (char *, char *[], int)` | `void (…, int *n)` | 4 (pokedex, tipos, archivos, resultados) |
| 3 | L1 | `tipos_es_valido` | `bool (const char *, Tipo *)` | `void (const char *, Tipo *salida, bool *es_valido)` | 2 (pokedex) |
| 4 | L1 | `pokedex_cargar` | `bool (Pokedex *, const char *)` | `void (…, bool *exito)` | 3 (main, 2 probes) |
| 5 | L2 | `entrenador_registrar` | `bool (RegistroEntrenadores *, int, const char *)` | `void (…, bool *exito)` | 2 (main, archivos) |
| 6 | L2 | `equipo_agregar_ejemplar` | `bool (Entrenador *, Ejemplar *)` | `void (…, bool *exito)` | 3 (main, archivos, probe_efectividad) |
| 7 | L2 | `equipo_formar_backtracking` | `bool (const Pokedex *, const RestriccionesEquipo *, Ejemplar **, int *)` | `void (…, bool *exito)` | 1 (main) |
| 8 | L2 | `equipo_siguiente_id` | `int (void)` | `void (int *id)` | 3 (main, equipo, probe_efectividad) |
| 9 | L2 | `bt_rec` (static) | `bool (…, Ejemplar **parcial, int *creados)` | `void (…, bool *exito)` | 4 (equipo, recursivas) |
| 10 | L3 | `combate_ejecutar` | `bool (Entrenador *, Entrenador *, bool, CombateSeleccionar, ResultadoCombate *)` | `void (…, bool *exito)` | 10 (main + 9 probe_efectividad) |
| 11 | L3 | `combate_atacar` (static) | `int (…, ResultadoCombate *res)` | `void (…, int *ganador)` | 2 (combate) |
| 12 | L4 | `torneo_armar_grupos` | `bool (Torneo *, const RegistroEntrenadores *)` | `void (…, bool *exito)` | 2 (main, probe_torneo) |
| 13 | L4 | `torneo_aplicar_resultado` | `bool (Torneo *, RegistroEntrenadores *, const ResultadoCargado *, char *, size_t)` | `void (…, bool *exito)` | 6 (resultados + 5 probe_torneo) |
| 14 | L4 | `aplicar_resultado` (static) | `bool (Torneo *, RegistroEntrenadores *, const ResultadoCargado *)` | `void (…, bool *exito)` | 2 (resultados) |
| 15 | L4 | `resultados_cargar_teclado` | `bool (Torneo *, RegistroEntrenadores *)` | `void (…, bool *exito)` | 1 (main) |
| 16 | L4 | `resultados_cargar_archivo` | `bool (Torneo *, RegistroEntrenadores *, const char *)` | `void (…, bool *exito)` | 2 (archivos, main) |
| 17 | L4 | `resultados_validar` | `bool (const Torneo *, const RegistroEntrenadores *, const ResultadoCargado *, char *, size_t)` | `void (…, bool *exito)` | 1 (resultados) |
| 18 | L4 | `archivos_cargar_entrenadores` | `bool (RegistroEntrenadores *, const Pokedex *, const char *)` | `void (…, bool *exito)` | 2 (main, probe_torneo) |
| 19 | L4 | `archivos_cargar_resultados` | `bool (Torneo *, RegistroEntrenadores *, const char *)` | `void (…, bool *exito)` | 0 |
| 20 | L4 | `archivos_guardar_entrenadores` | `bool (const RegistroEntrenadores *, const char *)` | `void (…, bool *exito)` | 1 (main) |
| 21 | L4 | `archivos_guardar_resultados` | `bool (const Torneo *, const char *)` | `void (…, bool *exito)` | 0 |
| 22 | L4 | `archivos_guardar_clasificacion` | `bool (const Torneo *, const RegistroEntrenadores *, const char *)` | `void (…, bool *exito)` | 1 (main) |
| 23 | L4 | `armar_torneo_si_falta` (static) | `bool (Torneo *, const RegistroEntrenadores *)` | `void (…, bool *exito)` | 3 (main) |

Cuerpos con conversión no trivial:

- **#9 `bt_rec`**: `if (restantes == 0) { *exito = <condición>; return; }`; podas y caso `i >= pd->cantidad` con `*exito = false; return;`; llamada de especie no admitida `bt_rec(..., exito); return;`; en el bucle, `if (j == k) { bt_rec(..., exito); if (*exito) return; }` (equivale al `if (j == k && bt_rec(...)) return true;` actual) y tras el retroceso se cae al siguiente k; al final `bt_rec(..., exito);` (excluir especie). `equipo_formar_backtracking` llama `bt_rec(..., exito); if (!*exito) return;` reutilizando su propio parámetro de salida.
- **#11 `combate_atacar`**: cada `return V;` → `*ganador = V; return;` con V ∈ {0 (continúa), id (gana), -1 (cancelado por EOF)}. Los dos llamadores de `combate_ejecutar` cambian solo el final de la llamada a `..., res, &g);`; los `if (g == -1)` / `if (g != 0)` existentes quedan igual.
- **#13/#14**: `aplicar_resultado` reenvía `exito` a `resultados_validar` y a `torneo_aplicar_resultado`; los llamadores usan `bool exito;` local (ver L4).
- **#17 `resultados_validar`**: 16 `return false` + `return true` final → `*exito = false/true; return;`; el buffer `msg` se sigue llenando igual (goldens de `017_resultados_invalidos`).
- **#19 `archivos_cargar_resultados`**: cuerpo pasa a `resultados_cargar_archivo(t, reg, ruta, exito);` (delegación pura).

---

## 4. Funciones que NO se convierten (10 grupos)

| Función(es) | Firma | Por qué NO |
|---|---|---|
| `combate_calcular_danio` | `int (const Ejemplar *, const Ejemplar *)` | Pura: parámetros `const`, un único resultado por `return` |
| `tipos_multiplicador` | `float (Tipo, Tipo, Tipo)` | Pura (RF-CMB-05); matriz estática interna de solo lectura |
| `tipos_a_texto` | `const char *(Tipo)` | Pura, retorno único (texto del tipo; nunca NULL, no muta nada) |
| `combate_ataca_primero` | `bool (const Ejemplar *, const Ejemplar *)` | Pura, retorno único; `const` en todo |
| `equipo_contar` | `int (const Entrenador *)` | Consulta pura sobre lista enlazada |
| `equipo_validar` | `bool (const Pokedex *, const Entrenador *, int)` | Chequeo puro, sin mutación |
| `entrenador_buscar` | `Entrenador *(RegistroEntrenadores *, int)` | Devuelve puntero a un elemento; no escribe el registro (parámetro no-const por firma histórica, no por mutación) |
| `pokedex_buscar_numero/nombre` | `const Especie *(…, …)` | Búsquedas de retorno único; `pokedex.c` es inmutable (RF-PDX-04/05) |
| `equipo_crear_ejemplar` | `Ejemplar *(…, int id_ejemplar)` | El ejemplar creado (malloc + inicialización) ES el único resultado; el `malloc` no es una salida del llamador |
| `validar_leer_entero`, `validar_leer_entero_msg` | `int (…)` | Retorno único (rango/EOF codificados); stdin es la entrada de la operación, no una salida por referencia |
| Procedures ya conformes | p. ej. `torneo_participantes`, `torneo_clasificacion_grupo`, `equipo_asignar`, `equipo_liberar`, `equipo_fijar_contador_id`, `todos los mostrar_*` | Ya son `void` con out por referencia; no devuelven nada |

---

## 5. Plan de llamadores por lote

Regla general por lote: editar header + implementación + **todos** los llamadores de ese lote y compilar (`make`) antes de pasar al siguiente. Los textos de `printf`/`msg` no se tocan.

### L1 — validación + tipos + pokedex

| Archivo | Llamada nueva |
|---|---|
| `main.c` (`consultar_pokedex` ~127, `registrar_entrenador` ~161, `crear_equipo_manual` ~332) | `bool exito;` + `validar_leer_cadena(prompt, buf, n, &exito); if (!exito) { <bloque actual> }` |
| `main.c` `main` ~691 | `pokedex_cargar(&pokedex, RUTA_POKEDEX, &exito); if (!exito) { <aviso actual> }` |
| `pokedex.c` ~108 | `validar_separar_campos(linea, campos, 8, &ncampos);` (resto igual) |
| `pokedex.c` ~155 | `bool ok1, ok2; tipos_es_valido(campos[2], &esp->tipo_primario, &ok1); tipos_es_valido(campos[3], &esp->tipo_secundario, &ok2); if (!ok1 \|\| !ok2) { <mensaje actual> }` |
| `tipos.c` ~176 | `validar_separar_campos(linea, campos, CANT_TIPOS, &ncampos);` |
| `archivos.c` ~104 | `validar_separar_campos(linea, campos, CAMPOS_MAX_LINEA, &ncampos);` |
| `resultados.c` ~304 | `validar_separar_campos(linea, campos, CAMPOS_MAX_LINEA, &ncampos);` |
| `probe_efectividad.c` ~109, `probe_torneo_completo.c` ~97 | `bool exito; pokedex_cargar(&pd, RUTA_POKEDEX, &exito); verificar(…exito…)` (mensajes idénticos) |

### L2 — entrenador + equipo

| Archivo | Llamada nueva |
|---|---|
| `main.c` ~174 | `entrenador_registrar(reg, id, nombre, &exito); if (!exito) { <mensaje "registro lleno"> }` |
| `main.c` ~234 | `equipo_formar_backtracking(pd, &restricciones, &equipo, &cantidad, &exito); if (!exito) { <mensaje actual> }` |
| `main.c` ~341 | `equipo_siguiente_id(&id_ejemplar);` (se elimina la asignación por retorno) |
| `main.c` ~347 | `equipo_agregar_ejemplar(ent, ej, &exito); if (!exito) { <"equipo lleno">; free(ej); break; }` |
| `archivos.c` ~153 | `entrenador_registrar(reg, id_ent, nombre_ent, &exito); if (!exito) goto linea_invalida;` |
| `archivos.c` ~164 | patrón con cortocircuito conservado: `bool agregado = false; if (ej != NULL) { equipo_agregar_ejemplar(ent, ej, &agregado); } if (!agregado) { <rollback actual> }` |
| `equipo.c` ~364 | `int id; equipo_siguiente_id(&id); ej = equipo_crear_ejemplar(pd, esp->numero, esp->nombre, nivel, id);` |
| `probe_efectividad.c` ~74 | `equipo_siguiente_id(&ej->id);` |
| `probe_efectividad.c` ~86 | `bool exito; equipo_agregar_ejemplar(ent, ej, &exito); (void)exito;` (retorno ignorado hoy) |

### L3 — combate

| Archivo | Llamada nueva |
|---|---|
| `combate.c` ~259 y ~270 | `combate_atacar(turno, …, res, &g);` (los `if (g == -1)` / `if (g != 0)` quedan intactos) |
| `main.c` `jugar_combate` ~502 | `combate_ejecutar(local, visita, modo == 2, seleccionar_pokemon_activo, &res, &exito); if (!exito) { <"El combate no se pudo ejecutar."> }` |

### L4 — torneo + resultados + archivos + main

| Archivo | Llamada nueva |
|---|---|
| `main.c` ~535 (dentro de `armar_torneo_si_falta`) | `torneo_armar_grupos(t, reg, exito); if (!*exito) { <mensaje actual>; return; }` |
| `main.c` ~554, ~596, ~628 | `bool exito; armar_torneo_si_falta(t, reg, &exito); if (!exito) return;` |
| `main.c` ~558 | `archivos_guardar_clasificacion(t, reg, RUTA_CLASIFICACION, &exito); if (exito) { … } else { … }` |
| `main.c` ~632/634 | `resultados_cargar_teclado(t, reg, &exito);` / `resultados_cargar_archivo(t, reg, RUTA_RESULTADOS, &exito);` + `(void)exito;` (se ignora como hoy) |
| `main.c` ~649 | `archivos_guardar_entrenadores(reg, RUTA_ENTRENADORES, &exito); if (exito) { … } else { … }` |
| `main.c` ~698 (carga inicial en `main`) | `archivos_cargar_entrenadores(&registro, &pokedex, RUTA_ENTRENADORES, &exito); if (!exito) { printf("Aviso: no se cargaron entrenadores desde el archivo; use la opción 2 para registrarlos.\n"); }` (mismo aviso actual) |
| `resultados.c` ~165 | `resultados_validar(t, reg, r, msg, sizeof(msg), &exito); if (!exito) { printf("%s\n", msg); return; }` |
| `resultados.c` ~169 | `torneo_aplicar_resultado(t, reg, r, msg, sizeof(msg), &exito); if (!exito) { <"Error al aplicar: %s"> }` |
| `resultados.c` ~255 y ~335 | `aplicar_resultado(t, reg, &r, &exito); if (exito) { aplicados++; }` (en ~335, `else { rechazadas++; }`) |

### L5 — probes + docs + verificación

Ver secciones 6–9. No hay cambios de lógica en probes: solo forma de llamada.

---

## 6. Plan de probes (mismos asserts, mismos mensajes)

**`probe_efectividad.c`** (12 referencias):

1. `crear_ejemplar` (~74, ~86): `equipo_siguiente_id(&ej->id);` y `bool exito; equipo_agregar_ejemplar(ent, ej, &exito); (void)exito;`.
2. `pokedex_cargar` (~109): `bool exito; pokedex_cargar(&pd, RUTA_POKEDEX, &exito); if (!exito) { printf("FAIL: no se pudo cargar la Pokédex (%s).\n", RUTA_POKEDEX); return 1; }` (mensaje idéntico).
3. Los 9 `combate_ejecutar` dentro de `verificar(...)`: se declara `bool exito;` una vez en `main`; cada assert pasa a dos sentencias. Ejemplo compuesto:
   ```c
   combate_ejecutar(a, b, true, stub_seleccionar, &res, &exito);
   verificar(exito == true && res.empate == false && res.id_ganador == 1,
             "D5a gana por mayor HP total (200 vs 160) -> local");
   ```
   Los 3 casos de validación usan `verificar(exito == false, "validación: …")`. El contador `casos`/`fallos` y el texto final no cambian.

**`probe_torneo_completo.c`** (8 referencias):

1. `aplicar_v1` (~49) conserva su firma `static int aplicar_v1(...)` y su `return`; internamente:
   ```c
   bool exito;
   torneo_aplicar_resultado(t, reg, &r, msg, n, &exito);
   return exito;
   ```
   Así los 3 usos de `aplicar_v1` (líneas 68, 80 y 118 de `probe_torneo_completo.c`, incluido el `verificar(!aplicar_v1(...))`) NO cambian.
2. Las 4 llamadas directas a `torneo_aplicar_resultado` (~152, 165, 204, 218): `torneo_aplicar_resultado(&t, &reg, &r, msg, sizeof(msg), &exito); verificar(!exito, "<mensaje idéntico>");`.
3. `pokedex_cargar` (~97) y `archivos_cargar_entrenadores` (~100): `...(&exito); verificar(exito, "<mensaje idéntico>");`.
4. `torneo_armar_grupos` (~107): `torneo_armar_grupos(&t, &reg, &exito); verificar(exito, "grupos: torneo armado (8x4)");`.

Los mensajes `strstr(msg, "…")` (~120, ~126) siguen funcionando porque `msg` se rellena igual.

---

## 7. Doxygen

Regla aplicada (con `docs/convenciones-doxygen.md` §4: "las `void` omiten `@return`"):

1. **Públicas convertidas (19, en `.h`)**: eliminar el `@return true si…; false si…` y añadir `@param exito` (o `@param n`, `@param id`, `@param es_valido`), moviendo el texto semántico del retorno a la descripción del nuevo parámetro. El resto del bloque (`@brief`, prosa, demás `@param`) no cambia.
2. **`equipo_siguiente_id`**: `@return Un id…` → `@param id Puntero donde se escribe el id (no debe ser NULL).`
3. **`combate_atacar` (static, bloque en `.c`)**: la frase "Devuelve el id del ganador (0 si continúa; -1 si se cancela)" → `@param ganador id del ganador, 0 si el combate continúa o -1 si se canceló por EOF`.
4. **`bt_rec` (static)**: `@return true si el parcial…` → `@param exito true si el parcial se completó cumpliendo las restricciones; false en caso contrario`.
5. **`combate_ejecutar`, `aplicar_resultado`, `armar_torneo_si_falta`**: añadir `@param exito`, eliminar `@return`.
6. Verificación: `doxygen Doxyfile` sin warnings nuevos en `doxygen_warnings.log` (si la herramienta está disponible).

---

## 8. Documentación a actualizar

| Documento | Sección | Cambio |
|---|---|---|
| `docs/informe-tecnico.md` | §2.1 tabla de módulos (líneas 67–78) | Recalcular el conteo de líneas de los módulos tocados con `wc -l` tras el apply |
| `docs/informe-tecnico.md` | §2.3 mermaid (línea 114) | Nodo "propagar true (primera hallada)" → "propagar `*exito = true` (primera hallada)" |
| `docs/informe-tecnico.md` | §3.1 "Firmas extendidas" (línea 284) | Añadir viñeta: convención de un solo resultado (2026-09-25): 23 funciones convertidas a `void` con salida por referencia; gate 23 PASS / 20 goldens |
| `docs/informe-tecnico.md` | nueva §3.2 (opcional) | "Convención de funciones de un solo resultado": regla, patrón `(…, out, bool *exito)` y excepciones de la sección 4 |
| `docs/apuntes-tecnicos.md` | §4 "`void`: dos significados" (líneas 127–153) | Añadir el tercer uso: `void` como procedimiento multi-salida por referencia (ej. `validar_leer_cadena(..., &exito)`) y citar el refactor |
| `docs/apuntes-tecnicos.md` | §5 snippet dirty flag (línea 173) | `bool exito;` + `archivos_guardar_entrenadores(reg, RUTA_ENTRENADORES, &exito); if (exito) …` |
| `docs/trazabilidad-requisitos.md` | filas RF-PDX-06, RF-ENT-03, RF-CMB-01, RF-TRN-02/03, RF-RES-02, RF-TEC-03 | Solo citan nombres (no firmas): sin cambio obligatorio. Opcional: anotar en RF-TEC-03 que `validar_leer_cadena/separar_campos` son procedimientos `void` |
| `docs/experimento-static.md` | salida `nm` (líneas 110–114) | Solo direcciones ilustrativas; los nombres (`bt_rec`, `combate_ejecutar`, `pokedex_cargar`) no cambian. Regenerar solo si se rehace la demostración |
| `docs/convenciones-doxygen.md` | §4 | Sin cambios: ya exige omitir `@return` en `void` |
| `README.md`, `openspec/specs/**` | — | No citan firmas convertidas: sin cambios |

---

## 9. Verificación de equivalencia (gate)

1. **Batería completa**: `bash tests/run_tests.sh` → `BATERÍA F9: 23 PASS, 0 FALLA (20 casos de menú + 2 probes)`; el script ya falla si `make clean && make` produce warnings o si un probe no compila.
2. **Goldens sin regenerar**: `sha256sum tests/casos/*.esperado > /tmp/goldens.sha` antes del refactor y `sha256sum -c /tmp/goldens.sha` después; además `git status --porcelain tests/casos/ data/` debe quedar vacío. Baseline verificado hoy: 23 PASS.
3. **Cero warnings**: cubierto por el gate de `run_tests.sh` (`make`) y por los builds de probes con `gcc -std=c99 -Wall -Wextra`.
4. **Auditoría de convención**: `rg -n '^\s*(bool|int|float)\s+\w+\(' src/*.h` debe listar solo las funciones de la sección 4; ninguna de las 23 aparece como no-void.
5. **Por lote**: compilar y correr la batería tras L1, L2, L3 y L4 (L5 cierra). Si un lote no compila, no se avanza.
6. **Doxygen** (secundario): `doxygen Doxyfile` sin warnings nuevos.

---

## 10. Archivos afectados

| Archivo | Acción | Detalle |
|---|---|---|
| `src/validacion.h`, `src/validacion.c` | Modify | L1: firmas + cuerpos (2 funciones) |
| `src/tipos.h`, `src/tipos.c` | Modify | L1: firma + cuerpo + 1 llamador interno |
| `src/pokedex.h`, `src/pokedex.c` | Modify | L1: firma + cuerpo + 2 tipos_es_valido + 1 separar_campos |
| `src/entrenador.h`, `src/entrenador.c` | Modify | L2: firma + cuerpo |
| `src/equipo.h`, `src/equipo.c` | Modify | L2: 4 funciones (bt_rec incluida) + llamada interna a siguiente_id |
| `src/combate.h`, `src/combate.c` | Modify | L3: 1 firma pública + 2 cuerpos (`combate_atacar` es static) |
| `src/torneo.h`, `src/torneo.c` | Modify | L4: 2 funciones |
| `src/resultados.h`, `src/resultados.c` | Modify | L4: 4 funciones + 2 llamadores + separar_campos |
| `src/archivos.h`, `src/archivos.c` | Modify | L4: 5 funciones + 3 llamadores + separar_campos |
| `src/main.c` | Modify | L1/L2/L3/L4: 18 llamadas + 1 definición (`armar_torneo_si_falta`) y 2 cuerpos llamadores (`guardar_al_salir`, `cargar_resultados`) |
| `tests/probes/probe_efectividad.c` | Modify | L5: 12 llamadas (mismos asserts) |
| `tests/probes/probe_torneo_completo.c` | Modify | L5: 8 llamadas (mismos asserts) |
| `docs/informe-tecnico.md`, `docs/apuntes-tecnicos.md` | Modify | L5: secciones de la tabla §8 |
| `docs/trazabilidad-requisitos.md`, `docs/experimento-static.md` | Modify (opcional) | Nota de convención / regeneración ilustrativa |

Sin archivos nuevos ni eliminados. `data/` y `tests/casos/` intactos.

---

## 11. Riesgos y mitigaciones

| Riesgo | Prob. | Mitigación |
|---|---|---|
| `bt_rec` no escribe `*exito` en alguna rama → decisión indefinida o pérdida de la primera solución | Media | Asignar `*exito` antes de CADA `return` (checklist por rama: éxito, i>=cantidad, 4 podas, no admitida, bucle, exclusión); recurrencia reenvía el mismo puntero y el padre hace `if (*exito) return;`; además `bool exito = false;` defensivo en `equipo_formar_backtracking`; casos 010/011 + probe |
| `combate_ejecutar` omite `*exito` en una de sus 8 salidas (params, equipos vacíos, 4 cancelaciones por EOF, victoria, fin del turno 20) | Media | Checklist por `return`; los 3 asserts de validación del probe + casos 012–014 detectan basura |
| Doble llamada a `tipos_es_valido` pierde el cortocircuito y altera `esp->tipo_secundario` | Baja | Verificado: `tipos_es_valido` solo escribe `*salida` en éxito (`tipos.c:229-247`); la rama de error imprime el mismo mensaje y hace `continue` |
| Llamadores que ignoran el retorno (main ~632/634, probe ~86) quedan sin variable → warning si se omite el argumento | Baja | Declarar `bool exito;` y `(void)exito;`; el gate de warnings lo verifica |
| `combate_atacar` con centinela `-1/0/id` mal documentado en llamadores nuevos | Baja | Contrato escrito en el bloque Doxygen y en el diseño; los llamadores conservan sus `if (g == -1)` / `if (g != 0)` |
| Deriva de mensajes/orden de `printf` por "arreglar" un llamador | Baja | Regla: los textos y el orden no se tocan; diff de goldens byte a byte en el gate |

---

## 12. Rollout / rollback

Sin migración de datos, sin feature flags, sin cambios de formato de archivo. Un commit por lote (L1, L2, L3, L4, L5) con mensajes en español; `git revert <commit>` del lote afectado restaura el estado. Los lotes son independientes por dependencia L1→L2→L3→L4; cada uno compila y pasa la batería completo. `data/` y `tests/casos/` no se modifican, así que el blast radius es código + probes + docs.

---

## 13. Threat Matrix

N/A — no hay routing, shell, subprocesos, automatización VCS/PR, clasificación de ejecutables ni integración de procesos en este cambio. `tests/run_tests.sh` ya existe y no se modifica.

---

## 14. Open Questions

Ninguna bloqueante. Puntos ya resueltos por el proposal/spec: nombres en español (`exito`), centinelas enteros en 3 funciones (D-B), archivos escritos como resultado (D-D), `bt_rec` con `bool *exito` (D-C). No requiere decisión adicional del docente.

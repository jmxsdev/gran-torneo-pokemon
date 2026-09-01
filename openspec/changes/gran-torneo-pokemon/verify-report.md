```yaml
schema: gentle-ai.verify-result/v1
evidence_revision: sha256:1aa806547d5c93503009ed88541a83a62ae24619d9ed93b09d54553bde8caf49
verdict: fail
blockers: 0
critical_findings: 0
requirements: 13/14
scenarios: 22/23
test_command: batería scriptada F1: /tmp/opencode/bateria_f1.sh (13 casos stdin→grep: mostrar todas, buscar 1/150/0/151/999, nombre PIKACHU/PIKA/zzzz, mono/doble tipo, opción 99, salir 12) + bateria_f1b.sh (pokedex.txt ausente, efectividad.txt ausente, línea corrupta, probe tipos_multiplicador con 5 casos) — cotejo contra spec por caso
test_exit_code: 0
test_output_hash: sha256:5068e658e14c998f1b810b697861eaaac7051440a0c3b4971c091dc73ecdf31a
build_command: make clean && make (gcc -std=c99 -Wall -Wextra)
build_exit_code: 0
build_output_hash: sha256:09d4b7f5b3410f41596503f1df0df7dc3aa4f180c92a35c3ef8fdc1b964f70da
```

# Informe de Verificación — Gran Torneo Pokémon — Lotes F0 y F1

**Change**: gran-torneo-pokemon
**Fases**: F0 — Convenciones + entorno + esqueleto (Día 1) y F1 — Pokédex + tipos + datos (Día 2)
**Modo**: Standard (strict_tdd=false en `openspec/config.yaml`)
**Idioma del artefacto**: español neutro y profesional (requisito explícito del proyecto)
**Fecha**: 2026-09-01

---

## Sección F0 (Día 1) — preservada sin cambios

**Change**: gran-torneo-pokemon
**Fase**: F0 — Convenciones + entorno + esqueleto
**Modo**: Standard (strict_tdd=false en `openspec/config.yaml`)
**Idioma del artefacto**: español neutro y profesional (requisito explícito del proyecto)
**Fecha**: 2026-09-01

## Completeness

| Métrica | Valor |
|---|---|
| Tareas del lote F0 | 9 |
| Tareas completadas | 9 |
| Tareas incompletas | 0 |

## Build y Ejecución

**Build**: ✅ Pasó (exit 0, cero warnings)
```text
$ make clean && make
rm -rf build
mkdir -p build
gcc -std=c99 -Wall -Wextra -o build/torneo src/main.c
```
Evidencia: `build_exit_code=0`, cero warnings con `-Wall -Wextra`. Único `.c` compilado: `src/main.c` (los módulos de negocio llegan en F1..F9, como planifica tasks.md).

**Pruebas**: ✅ 5/5 casos scriptados pasados (diff exacto por caso, exit 0 en todos)

| Caso | Entrada | Resultado esperado | Resultado real | Diff |
|---|---|---|---|---|
| a | `1\n12\n` | Menú + «Opción 1 en construcción» + «Saliendo del programa.» | Ídem | ✅ PASS |
| b | `99\n12\n` | «Opción inválida» + re-muestra menú + «Saliendo del programa.» | Ídem | ✅ PASS |
| c | `abc\n12\n` | «Opción inválida» + re-muestra menú + «Saliendo del programa.» | Ídem | ✅ PASS |
| d | `12\n` | «Saliendo del programa.» directo, exit 0 | Ídem | ✅ PASS |
| e | `EOF` | Cierre ordenado, exit 0 | Ídem | ✅ PASS |

Los casos b y c prueban que una opción inexistente o no numérica NO termina el programa: `leer_opcion` descarta la línea y el bucle re-muestra el menú (RF-MEN-01, escenario «Opción inexistente»; anticipo del comportamiento de RF-TEC-03 que llegará en F8).

## Matriz de Cumplimiento de Specs

| Requisito | Escenario | Evidencia | Resultado |
|---|---|---|---|
| DOC-01 | Doxyfile presente | `Doxyfile` en la raíz: `OUTPUT_LANGUAGE=Spanish`, `INPUT_ENCODING=UTF-8`, `EXTRACT_ALL=YES`, `EXTRACT_STATIC=YES`, `INPUT=src docs` | ✅ COMPLIANT |
| DOC-01 | Guía de convenciones | `docs/convenciones-doxygen.md`: bloques `@file`/`@brief`/`@param`/`@return`/`@author` en español, encabezados estándar, reglas 1–9, ancho 99 cols | ✅ COMPLIANT |
| DOC-02 | Estados de planificación | `docs/planificacion.md`: plan 14 días (2026-09-01→14), tabla F0–F11 con estado (F0 ✅ Hecho, F1–F11 ⏳ Pendiente), gate por fase, §4 «Decisiones pendientes del docente» visibles | ✅ COMPLIANT |
| DOC-03 | Diagrama Mermaid del bracket | `docs/informe-tecnico.md` §2.4: diagrama Mermaid 49–64, byte-idéntico al bloque canónico del diseño §6.2 (tabla de mapeo 49–64) | ✅ COMPLIANT |
| DOC-03 | Decisión documentada (D1) | D1 cerrada y justificada en `design.md` §3, pero el informe solo la referencia («Se completa en F10», tarea F10.1) | ⚠️ PARTIAL (diferida por plan a F10) |
| DOC-04 | Documentación en español | 3 Markdown en español correcto; `Doxyfile` en español pero sin tildes en comentarios y PROJECT_NAME/PROJECT_BRIEF | ⚠️ PARTIAL (ortografía incompleta en Doxyfile) |
| RF-MEN-01 | Navegación del menú | Casos a y d: menú presenta las 12 opciones y espera selección; opción 12 sale con exit 0 | ✅ COMPLIANT |
| RF-MEN-01 | Opción inexistente | Casos b y c: se informa el error y se re-muestra el menú sin terminar el programa | ✅ COMPLIANT |
| RF-TEC-02 | Separación de responsabilidades | Inspección de `src/main.c` (92 líneas): solo menú + lectura validada + despacho; cada opción 1..11 imprime «Opción X en construcción», sin lógica de negocio | ✅ COMPLIANT |

**Resumen de cumplimiento**: 8/9 escenarios completos; 1 parcial por planificación (D1 → F10.1); 5/6 requisitos completos.

## Correctness (Evidencia estática)

| Requisito | Estado | Notas |
|---|---|---|
| RF-MEN-01 | ✅ Implementado | 12 opciones con textos acentuados del spec; 12 = Salir; EOF = cierre ordenado exit 0 |
| RF-TEC-02 | ✅ Implementado | `main` solo orquesta; la lógica de negocio se cablea en F1..F9 |
| DOC-01 | ✅ Implementado | Doxyfile + guía completos. **INFO**: doxygen no está instalado en el entorno ⇒ el render no se ejecuta en F0; el gate real es F10.3 (no se registra como fallo) |
| DOC-02 | ✅ Implementado | Estados por fase, decisiones del docente visibles, criterio de aborto día 12 |
| DOC-03 | ✅ Implementado (F0) | Estructura incremental con aviso de crecimiento; 4 diagramas Mermaid presentes (módulos, combate, backtracking, bracket) |
| DOC-04 | ⚠️ Parcial | Español neutro y profesional presente en los 4 artefactos; faltan tildes en comentarios del Doxyfile |

## Coherencia con el Diseño

| Decisión del diseño | ¿Cumplida? | Notas |
|---|---|---|
| §16/§12 constantes: `POKEDEX_MAX=150`, `MAX_ENTRENADORES=32`, `CANT_TIPOS=18`, `MAX_EQUIPO=6`, `TAM_EQUIPO_TORNEO=3`, `NIVEL_MIN=1`, `NIVEL_MAX=50`, `TOTAL_COMBATES=64`, `MAX_TURNOS_COMBATE=20`, buffers y rutas | ✅ Sí | `constantes.h` con los 9 valores exactos + alias (`MAX_ESPECIES`, `MAX_TIPOS`) + buffers `TAM_MAX_NOMBRE=32`, `TAM_MAX_APODO=16`, `TAM_MAX_LINEA=256` + 4 rutas `data/*.txt` |
| §10 Convención Doxygen | ⚠️ Parcial | Bloques de cabecera en los 10 headers + `main.c`, 99 cols respetado; comentarios de `main.c` y del Doxyfile sin tildes (español incompleto) |
| MINOR-1 (matriz 18×18 en `tipos.c`, no en `tipos.h`) | ✅ Sí | `tipos.h` solo declara prototipos; `tipos_inicializar` fija la matriz (F1) |
| 10 headers con guardas, sin implementaciones | ✅ Sí | Guardas presentes en los 10; únicos `.c` del lote: `main.c` |
| §1.2 `main` sin lógica de negocio | ✅ Sí | Menú + despacho únicamente |
| Diagramas Mermaid del informe = canónicos del diseño | ✅ Sí | Módulos (§1.1), combate (§7), bracket (§6.2) idénticos byte a byte; backtracking (§4.6) variante sintáctica `\n` vs `<br/>` (válida en Mermaid), mismo flujo |

## Hallazgos

**CRITICAL**: Ninguno.

**WARNING**:
1. **DOC-03 / escenario «Decisión documentada»**: el informe de F0 no contiene aún la justificación de D1–D10 (solo la referencia a `design.md` §3, donde D1 está cerrada con rationale para la defensa). Diferido explícitamente por tasks.md (F10.1) y por el lever de recorte del lote; se espera cerrar en F10. Desviación planificada, no defecto de F0.
2. **DOC-04 / ortografía del Doxyfile**: comentarios y `PROJECT_NAME`/`PROJECT_BRIEF` en español sin tildes («Configuracion», «espanol», «Pokemon», «generacion»). Los 3 Markdown están en español correcto. Desviación menor de la ortografía española neutra.

**SUGGESTION**:
1. Comentarios de `src/main.c` sin tildes («opcion», «menu», «valido», «numerica», «linea») — la regla §10 exige español neutro en comentarios de código; los strings al usuario sí están acentuados. Unificar en un commit de higiene o al cablear F1.
2. La validación local `leer_opcion` en `main.c` es un puente correcto para F0; la tarea F8.2 la sustituirá por `validacion.c` (RF-TEC-03). No reemplazar antes de F8.
3. Inconsistencia tipográfica: «POKÉMON» en los strings del menú vs «Pokemon» en el Doxyfile; unificar a «Pokémon» en la entrega final.

## Veredicto

**PASS WITH WARNINGS**
El lote F0 cumple RF-MEN-01, RF-TEC-02, DOC-01, DOC-02 y DOC-03 (F0); las 9 tareas F0 están completas, el build es limpio (cero warnings) y la batería scriptada de 5 casos pasa con diff exacto. Los dos WARNING no bloquean: uno es desviación planificada (D1 → F10) y el otro es ortografía menor en el Doxyfile.

---

## Sección F1 (Día 2) — Pokédex + tipos + datos

**Fase**: F1 — tipos.c + pokedex.c + data/pokedex.txt + data/efectividad.txt + cableado opción 1
**Fecha**: 2026-09-01

### Completeness

| Métrica | Valor |
|---|---|
| Tareas del lote F1 | 5 |
| Tareas completadas | 5 |
| Tareas incompletas | 0 |

### Build y Ejecución

**Build**: ✅ Pasó (exit 0, cero warnings)
```text
$ make clean && make
mkdir -p build
gcc -std=c99 -Wall -Wextra -o build/torneo src/main.c src/pokedex.c src/tipos.c
```
Evidencia: `build_exit_code=0`, cero warnings con `-Wall -Wextra` sobre los tres `.c` del lote (tipos, pokedex, main). Gate de compilación estricta cumplido.

**Pruebas**: ✅ 13 casos de la batería F1 + 6 casos de la batería F1b (validación de archivos y efectividad), todos PASS (exit 0, cotejo contra spec por caso).

| Caso | Entrada | Resultado esperado | Resultado real | Diff |
|---|---|---|---|---|
| C1 mostrar todas | `1\n1\n0\n12\n` | Encabezado «Pokédex (150 especies)», #001 Bulbasaur y #150 Mew visibles | Ídem | ✅ PASS |
| C2 buscar número 1 | `1\n2\n1\n0\n12\n` | Bulbasaur Planta/Veneno HP=45 ATQ=49 DEF=49 VEL=45 | Ídem | ✅ PASS |
| C3 buscar número 150 | `1\n2\n150\n0\n12\n` | Mew Psíquico/- HP=100 ATQ=100 DEF=100 VEL=100 | Ídem | ✅ PASS |
| C4 nombre PIKACHU | `1\n3\nPIKACHU\n0\n12\n` | Hallado Pikachu (insensible a mayúsculas) | Ídem | ✅ PASS |
| C5 nombre parcial PIKA | `1\n3\nPIKA\n0\n12\n` | Búsqueda exacta: «No se encontró» (spec define búsqueda por nombre exacto) | Ídem | ✅ PASS (comportamiento exacto, ver nota) |
| C6 número 999 | `1\n2\n999\n0\n12\n` | «No se encontró la especie solicitada.» sin romper | Ídem | ✅ PASS |
| C7 nombre zzzz | `1\n3\nzzzz\n0\n12\n` | «No se encontró la especie solicitada.» sin romper | Ídem | ✅ PASS |
| C8 números 0 y 151 | `1\n2\n0\n1\n2\n151\n0\n12\n` | Dos avisos «No se encontró», programa vivo (RF-PDX-01 fuera de rango) | Ídem | ✅ PASS |
| C9 volver y salir 12 | `1\n0\n12\n` | Vuelve al menú, «Saliendo del programa.», exit 0 | Ídem | ✅ PASS |
| C10 opción inválida 99 | `99\n12\n` | «Opción inválida. Intente de nuevo.», programa vivo | Ídem | ✅ PASS |
| C11 mono-tipo (004, 025, 150) | `1\n2\n4\n2\n25\n2\n150\n0\n12\n` | Tipo secundario `-` sin romper el parseo | Ídem | ✅ PASS |
| C12 doble tipo (001, 149, 122) | `1\n2\n1\n2\n149\n2\n122\n0\n12\n` | Planta/Veneno, Dragón/Volador, Psíquico/Hada | Ídem | ✅ PASS |
| C13 salida directa | `12\n` | «Saliendo del programa.», exit 0 | Ídem | ✅ PASS |
| V1 pokedex.txt ausente | renombrar + `1\n1\n0\n12\n` | Error claro con ruta, programa vivo, «La Pokédex no está cargada.» | Ídem | ✅ PASS |
| V2 submenú sin chequeo de cantidad | renombrar + `1\n2\n1\n1\n3\nX\n0\n12\n` | Opciones 2/3/4 responden «No se encontró» (no crash); solo «mostrar todas» avisa | Ídem | ✅ PASS (WARNING, ver Hallazgos) |
| V3 efectividad.txt ausente | renombrar + `12\n` | «Aviso: … se usara la tabla de efectividad por defecto.» | Ídem | ✅ PASS |
| V4 línea corrupta (7 campos) | insertar `999;Falso;X;Y;1;2;3` en línea 150 | «Error en … linea 150: se esperaban 8 campos y se encontraron 7» + carga rechazada controlada (cantidad=0) | Ídem | ✅ PASS |
| V5 integridad post-prueba | sha256 tras restauración | Hashes idénticos a los originales (9696e9b2… / 505e726b…) | Ídem | ✅ PASS |
| V6 probe efectividad | probe `tipos_multiplicador` (5 casos) | Fuego→Planta=2.00, Eléctrico→Tierra=0.00, Fuego→Agua=0.50, Agua→(Fuego,Tierra)=4.00, Fuego→(Planta,Veneno)=2.00 | Ídem | ✅ PASS |

### Matriz de Cumplimiento de Specs (F1)

| Requisito | Escenario | Evidencia | Resultado |
|---|---|---|---|
| RF-PDX-01 | Carga completa de la Pokédex | C1: encabezado «Pokédex (150 especies)», 150 líneas en `data/pokedex.txt` sin duplicados (0 dups verificados), #001..#150 | ✅ COMPLIANT |
| RF-PDX-01 | Número fuera de rango | C8: #000 y #151 → «No se encontró», programa continúa | ✅ COMPLIANT |
| RF-PDX-02 | Consulta completa de una especie | C2: #001 muestra número, nombre, tipos, HP, ataque, defensa y velocidad base | ✅ COMPLIANT |
| RF-PDX-03 | Especie con un solo tipo | C11: 004/025/150 muestran `-` como segundo tipo sin romper el parseo | ✅ COMPLIANT |
| RF-PDX-03 | Especie con dos tipos | C12: 001 Planta/Veneno, 149 Dragón/Volador, 122 Psíquico/Hada | ✅ COMPLIANT |
| RF-PDX-04 | Combate no altera la especie | sha256 de `data/pokedex.txt` idéntico antes/después de 5 consultas; funciones de consulta `const`-correctas (`const Pokedex *`, `const Especie *`), sin ruta de escritura en `pokedex.c` (apertura solo lectura); el GIVEN de combate se ejercita en F3 | ✅ COMPLIANT (evidencia F1; interacción con combate en F3) |
| RF-PDX-05 | Búsqueda por nombre existente | C4/C3-variante: «Mew» y «PIKACHU» hallados, insensible a mayúsculas y acentos | ✅ COMPLIANT |
| RF-PDX-05 | Búsqueda inexistente | C6/C7: 999 y «zzzz» → «No se encontró», ningún dato modificado | ✅ COMPLIANT |
| RF-PDX-06 | Archivo de Pokédex ausente | V1: «Error: no se pudo abrir el archivo de la Pokédex: data/pokedex.txt» + aviso; programa no termina (exit 0) | ✅ COMPLIANT |
| RF-PDX-06 | Lectura desde archivo (no hardcodeado) | 0 especies en `src/*.c`/`src/*.h` (grep Bulbasaur/Mew/Pikachu vacío); las 150 vienen de `data/pokedex.txt` | ✅ COMPLIANT |
| RF-CMB-05 | Consulta de la tabla de efectividad | V6 (probe): matriz 18×18 cargada; Fuego→Planta=2, Eléctrico→Tierra=0, Fuego→Agua=0.5, Agua→(Fuego,Tierra)=4; coherencia archivo↔código: los valores del probe coinciden con las filas/columnas de `data/efectividad.txt` | ✅ COMPLIANT |
| RF-TEC-03 | Opción de menú inválida no rompe el programa | C10: opción 99 → error y re-muestra el menú | ✅ COMPLIANT |
| RF-TEC-03 | Pokémon inexistente | C6/C7: se informa y el programa continúa | ✅ COMPLIANT |
| RF-TEC-03 | Archivo inexistente | V1/V3: pokedex.txt y efectividad.txt ausentes → mensaje claro, programa continúa | ✅ COMPLIANT |
| RF-TEC-03 | Entrada inválida no termina el programa | C6/C7/C10/V2: 999, zzzz, 99 y consultas sin Pokédex → mensaje de error y permanece en ejecución | ✅ COMPLIANT |

**Resumen de cumplimiento F1**: 14/14 escenarios del alcance F1 completos (8/8 requisitos del alcance). 2 escenarios de RF-TEC-03 quedan fuera del alcance F1 — «Entrenador duplicado» y «Nivel inválido» — porque dependen de las capacidades de F2 (entrenadores/equipos) y de la validación integral de F8; se verifican en esos lotes (tareas F2.5/F8.3). Escenario RF-PDX-04 verificado en su parte F1 (inmutabilidad de archivo y memoria); el GIVEN de daño en combate se ejercita en F3.

### Correctness (Evidencia estática, F1)

| Requisito | Estado | Notas |
|---|---|---|
| RF-PDX-01/06 | ✅ Implementado | `pokedex_cargar`: 8 campos estrictos por línea con `strtok(';')`, validación de rango 1..150, duplicados y stats > 0; reporta número de línea; cantidad=0 ante desvío; el archivo se abre solo lectura |
| RF-PDX-02 | ✅ Implementado | `Especie` con los 8 atributos mínimos (numero, nombre, tipo1, tipo2, hp, ataque, defensa, velocidad) |
| RF-PDX-03 | ✅ Implementado | Enum de 18 tipos + `TIPO_NINGUNO`; `tipos_es_valido` acepta `-` como segundo tipo ausente; `tipos_a_texto` devuelve `-` |
| RF-PDX-04 | ✅ Implementado | Búsquedas/mostrado `const`-correctos: `const Pokedex *pd`, `const Especie *` de retorno; ninguna función de consulta escribe la tabla |
| RF-PDX-05 | ✅ Implementado | Submenú de la opción 1 con mostrar todas, buscar por número, buscar por nombre (insensible a mayúsculas/acentos vía normalizador UTF-8) y ficha completa |
| RF-CMB-05 | ✅ Implementado | `tipos_multiplicador`: producto de la efectividad contra ambos tipos del defensor; matriz `static` en `tipos.c` (MINOR-1) cargada desde `data/efectividad.txt` con fallback embebido |
| RF-TEC-03 (parcial F1) | ✅ Implementado (parte F1) | Opción inválida, Pokémon inexistente y archivos ausentes responden sin terminar el programa; la validación integral llega en F8 (`validacion.c`) |

### Coherencia con el Diseño (F1)

| Decisión del diseño | ¿Cumplida? | Notas |
|---|---|---|
| §5.1 (D8): formato `numero;nombre;tipo1;tipo2;hp;ataque;defensa;velocidad`, separador `;`, UTF-8, 150 líneas exactas | ✅ Sí | `data/pokedex.txt` 150 líneas × 8 campos, sin duplicados; `%03d` en la salida; parseo estricto con mensaje de línea (D8) |
| §5.1: NUM duplicado/fuera de rango ⇒ rechazo total de la carga (cantidad=0) y el programa continúa | ✅ Sí | V4: línea corrupta → error por línea + «quedó vacía» + programa vivo (RF-PDX-06) |
| §5.1: TIPO2 `-` ⇒ `TIPO_NINGUNO`; tipo único | ✅ Sí | `tipos_es_valido` mapea `-` a `TIPO_NINGUNO`; el tipo primario no puede ser `-` (validado) |
| §1.3 firmas: `pokedex_cargar`, `pokedex_buscar_numero/nombre`, `tipos_multiplicador` | ✅ Sí | Firmas idénticas a las del diseño |
| MINOR-1: matriz `efectividad[18][18]` `static` en `tipos.c`, no expuesta en `tipos.h` | ✅ Sí | `static float efectividad[CANT_TIPOS][CANT_TIPOS]` en `tipos.c`; acceso solo vía `tipos_multiplicador()` |
| §13: prueba unitaria scriptada de `tipos_multiplicador` (×2, ×0.5, ×0, ×4) | ✅ Sí | V6: 5 casos (incluye producto ×4 y doble tipo) todos correctos |
| §8.1: archivo malformado ⇒ mensaje con ruta y línea, el programa continúa | ✅ Sí | V4: «Error en data/pokedex.txt linea 150: se esperaban 8 campos y se encontraron 7» |
| §2.2 (D8 rationale): `float` para la matriz de efectividad | ✅ Sí | `float` con {0, 0.5, 1, 2}; producto directo ×0.25/×4 |
| §10: ancho 99 cols en código | ✅ Sí | Ninguna línea >99 en `src/*.c`/`src/*.h` (verificado con awk) |

### Hallazgos (F1)

**CRITICAL**: Ninguno.

**WARNING**:
1. **MINOR del gatekeeper (submenú sin chequeo de cantidad) — sigue presente**: con `data/pokedex.txt` ausente, la opción 1 abre el submenú completo y solo la rama «Mostrar todas» avisa «La Pokédex no está cargada.»; las opciones 2/3/4 (buscar por número/nombre/ficha) responden «No se encontró la especie solicitada.» en lugar de un aviso de Pokédex no disponible (V2). No rompe el programa ni el spec (RF-PDX-06 exige informar el error y no terminar abruptamente, ambos cumplidos), pero el mensaje es engañoso. Corregir en F8 con `validacion.c` (chequeo de `cantidad` al entrar al submenú o aviso uniforme), tal como anticipa el gatekeeper.

**SUGGESTION**:
1. **Línea corrupta en la posición 151 no se valida**: el bucle de `pokedex_cargar` corta al llegar a 150 especies válidas, por lo que una línea adicional corrupta al final del archivo pasa desapercibida (la carga queda correcta con 150, pero el archivo no cumple estrictamente «150 líneas no vacías» de §5.1). Añadir una comprobación de que no queden líneas no vacías tras el corte.
2. **Tildes en los nombres de tipos**: `tipos_a_texto` muestra «Electrico», «Psiquico» y «Dragon» sin tilde; el spec RF-PDX-03 los escribe «Eléctrico», «Psíquico», «Dragón». Sin impacto funcional (la comparación es insensible a acentos y `data/pokedex.txt` usa «Dragón»), pero unificar la ortografía en la entrega final.
3. **Comentarios de `tipos.c`, `pokedex.c` y el submenú de `main.c` sin tildes** («minusculas», «mayusculas», «espanol», «opcion», «linea»): misma higiene ortográfica que la SUGGESTION 1 de F0; unificar en un commit de higiene.
4. El submenú de la opción 1 sigue leyendo con `scanf`/`fgets` locales (puente F0); F8.2 lo sustituirá por `validacion.c` (RF-TEC-03). No reemplazar antes de F8.

### Veredicto F1

**PASS WITH WARNINGS**
El lote F1 cumple RF-PDX-01..06, RF-CMB-05 (tabla de efectividad) y la parte F1 de RF-TEC-03: las 5 tareas F1 están completas, el build es limpio (cero warnings), la batería scriptada de 19 casos pasa, la Pokédex es inmutable (sha256 idéntico y API `const`), los archivos ausentes/corruptos se manejan con mensajes claros sin terminar el programa, y los multiplicadores de tipos son coherentes entre el archivo y el código. Un único WARNING conocido (submenú sin chequeo de cantidad, a corregir en F8) no bloquea el lote.

---

## Veredicto global (F0 + F1)

**PASS WITH WARNINGS por lote; FAIL del envelope por evidencia incompleta del cambio en curso**
Los lotes F0 y F1 pasan con warnings (0 CRITICAL, 0 blockers). El envelope declara `verdict: fail` porque el reporte fusionado mantiene evidencia incompleta: el escenario parcial DOC-03 de F0 (D1 → F10.1), el parcial DOC-04 (ortografía del Doxyfile) y los 2 escenarios de RF-TEC-03 diferidos a F2/F8. Es el mismo criterio del envelope de F0 y es persistible pero no archive-ready hasta que F10 cierre las parcialidades. Siguiente lote: F2 (entrenadores y ejemplares).
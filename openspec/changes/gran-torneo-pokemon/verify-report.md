```yaml
schema: gentle-ai.verify-result/v1
evidence_revision: sha256:835580d320dc473098b561eec76f371f45beb16a64a19f7d62ccb301a6cf5058
verdict: fail
blockers: 0
critical_findings: 0
requirements: 40/42
scenarios: 68/70
test_command: /tmp/opencode/bateria_f8.sh /home/gzuz/Documentos/universidad/algorit-I/Proyecto (batería F8 15 casos E2E rutas inválidas: build limpio cero warnings con 10 .c, opción 99, texto en menú, nivel 101/0 con reintento, id duplicado, Pokédex ausente con submenú completo, EOF×3 con timeout, regresión batería F7 13/13 + batería F4 12/12 + probe F3 21/21, sha256 de los 4 datos, ≤99 cols, validacion.c sin exit; + probe F7 39/39 y parsers `;;` verificados en la verificación formal F8)
test_exit_code: 0
test_output_hash: sha256:58f7868933a0c449a6c0569dfc36c3d828c9bc4b75f6f89687696fe2a46b2e20
build_command: make clean && make (gcc -std=c99 -Wall -Wextra -o build/torneo src/archivos.c src/combate.c src/entrenador.c src/equipo.c src/main.c src/pokedex.c src/resultados.c src/tipos.c src/torneo.c src/validacion.c; exit 0, cero warnings)
build_exit_code: 0
build_output_hash: sha256:22c555a6b038d8767f2a041901caf9bc01de5ecb66d73c5be3ed6d7b7bbec144
```

# Informe de Verificación — Gran Torneo Pokémon — Lotes F0, F1, F2 y F3

**Change**: gran-torneo-pokemon
**Fases**: F0 — Convenciones + entorno + esqueleto (Día 1), F1 — Pokédex + tipos + datos (Día 2), F2 — Entrenadores y ejemplares (Día 3, 2026-09-19) y F3 — Combate (Día 4, 2026-09-21)
**Modo**: Standard (strict_tdd=false en `openspec/config.yaml`)
**Idioma del artefacto**: español neutro y profesional (requisito explícito del proyecto)
**Fecha**: 2026-09-21

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

## Sección F2 (Día 3) — Entrenadores y ejemplares

**Fase**: F2 — entrenador.c + equipo.c (ejemplares, D2) + archivos.c (carga inicial, alcance F7 adelantado por F2.4) + data/entrenadores.txt + cableado de las opciones 2..5
**Fecha**: 2026-09-19

### Completeness

| Métrica | Valor |
|---|---|
| Tareas del lote F2 | 5 |
| Tareas completadas | 5 |
| Tareas incompletas | 0 |

Notas: F2.1 `entrenador.c` (registro con id único, búsqueda, listado), F2.2 `equipo.c` (ejemplares con D2 exacta, validación, liberación), F2.3 `data/entrenadores.txt` (32 entrenadores, formato §5.2), F2.4 cableado de las opciones 2..5 y carga inicial, F2.5 verificación scriptada. `src/archivos.c` (198 líneas) es alcance de F7 adelantado por F2.4 (la carga inicial de entrenadores.txt lo exige); se registra como desvío de presupuesto transparente (+44 líneas, +5,5 % sobre 800), ya documentado en el apply-progress del lote (obs #280).

### Build y Ejecución

**Build**: ✅ Pasó (exit 0, cero warnings)
```text
$ make clean && make
rm -rf build
mkdir -p build
gcc -std=c99 -Wall -Wextra -o build/torneo src/archivos.c src/entrenador.c src/equipo.c src/main.c src/pokedex.c src/tipos.c
```
Evidencia: `build_exit_code=0`, cero warnings con `-Wall -Wextra` sobre los seis `.c` del proyecto. Hash de la salida del build: `1c4af6b2…` — **idéntico byte a byte** al registrado en el envelope original de F2, lo que corrobora la equivalencia de la evidencia re-derivada. 0 líneas de más de 99 columnas en `src/*.c`/`src/*.h` (awk).

**Pruebas**: ✅ Batería F2 re-derivada (24 casos stdin→grep, incluido el build) + probe C (7 comprobaciones): **50/50 PASS, exit 0**. Los scripts originales (`/tmp/opencode/bateria_f2.sh` y `/tmp/opencode/probe_f2`) se habían perdido del entorno de /tmp; esta batería re-derivada cubre los mismos puntos clave de la verificación F2.5 y reproduce la evidencia exacta del probe C registrada en el envelope (39/35/35/34, 214/217/217/209, variación id 1 vs 2, excepción id 1 vs 17, mutación del ejemplar sin tocar la especie).

| Caso | Entrada | Resultado esperado | Resultado real | Diff |
|---|---|---|---|---|
| 1 build | `make clean && make` | exit 0, cero warnings, seis `.c` compilados | Ídem | ✅ PASS |
| 2 carga inicial | `4\n12\n` | «Entrenadores cargados: 32, líneas rechazadas: 0.» + «(32/32)» | Ídem | ✅ PASS |
| 3 listado | `4\n12\n` | Ash (id 1) y Gladion (id 32) visibles, contadores en cero | Ídem | ✅ PASS |
| 4 reconstrucción D2 | `5\n1\n12\n` | Bulbi #101 (especie #1, nivel 12): HP 35/35, Ataque 31, Defensa 31, Velocidad 30 | Ídem | ✅ PASS |
| 5 registro válido | archivo ausente + `2\n1\nRojo\n4\n12\n` | «Entrenador registrado: id 1, Rojo.» + «(1/32)» | Ídem | ✅ PASS |
| 5b tope 32 | `2\n33\nRojo\n12\n` | «No se pudo registrar: registro lleno (máximo 32).» | Ídem | ✅ PASS |
| 6 duplicado | `2\n7\nRojo2\n12\n` | «Ya existe un entrenador con id 7; el registro se rechaza.» | Ídem | ✅ PASS |
| 7 id inválido | `2\n0\n12\n` | «Id inválido: debe ser un entero positivo.» | Ídem | ✅ PASS |
| 8 id no numérico | `2\nabc\n4\n12\n` | «Entrada inválida.» + programa vivo | Ídem | ✅ PASS |
| 9 nombre vacío | `2\n34\n\n12\n` | «El nombre no puede estar vacío.» | Ídem | ✅ PASS |
| 10 niveles 0/101/1 | `3\n9\n1\n0\n101\n1\n\n0\n12\n` | 0 y 101 → «Nivel inválido (rango 1-100).»; 1 → agregado, equipo 1/6 | Ídem | ✅ PASS |
| 11 nivel 100 | `3\n9\n1\n100\n\n0\n12\n` | «nivel 100) agregado; equipo 1/6.» + «El equipo es válido» | Ídem | ✅ PASS |
| 12 7.º ejemplar | `3\n9\n1..6\n10\n\n` + `7\n10\n\n0\n12\n` | 6 agregados (#178–#183), 7.º rechazado, equipo válido | Ídem | ✅ PASS |
| 13 #151 | `3\n9\n151\n0\n12\n` | «La especie 151 no existe en la Pokédex.» sin terminar | Ídem | ✅ PASS |
| 14 especie 0 | `3\n9\n0\n5\n9\n12\n` | Termina sin agregar ejemplar; «no tiene equipo» | Ídem | ✅ PASS |
| 15 salida | `12\n` | «Saliendo del programa.», exit 0 | Ídem | ✅ PASS |
| 16 EOF | entrada vacía | Cierre ordenado, exit 0 | Ídem | ✅ PASS |
| 17 opción 99 | `99\n12\n` | «Opción inválida. Intente de nuevo.» + salida | Ídem | ✅ PASS |
| 18 archivo ausente | renombrar + `4\n12\n` | «Aviso: no se cargaron entrenadores desde el archivo…» + programa vivo | Ídem | ✅ PASS |
| 19–22 líneas corruptas | 4 variantes (especie 999, nivel 101, id de entrenador duplicado, campos de más) añadidas como línea 33 | «Línea 33 rechazada: campos inválidos (no se registra).» + «cargados: 32, rechazadas: 1» | Ídem | ✅ PASS |
| 23 sha256 | sesión completa (crear + consultar) | `data/pokedex.txt` estable: `9696e9b2…` antes y después | Ídem | ✅ PASS |
| P1–P7 probe C | probe contra `src/{pokedex,tipos,equipo,entrenador}.c` | D2 exacta 39/35/35/34 y 214/217/217/209; HP 35 (id 101 nv12); variación 7 vs 14 (id 1 vs 2); excepción id 1 vs 17 (stats idénticas); hp_base=45 intacto tras dañar al ejemplar; #151 y #0 → NULL | Ídem | ✅ PASS |

Los casos 6 y 10/11 cierran los dos escenarios de RF-TEC-03 que F1 dejó diferidos («Entrenador duplicado» y «Nivel inválido»).

### Matriz de Cumplimiento de Specs (F2)

| Requisito | Escenario | Evidencia | Resultado |
|---|---|---|---|
| RF-ENT-01 | Registro de entrenador | Caso 5: id 1 «Rojo» se crea con victorias, empates, derrotas y puntuación en cero (caso 3: «G:0 E:0 P:0 Puntos:0»); `struct Entrenador` con id, nombre, equipo y los contadores mínimos exigidos | ✅ COMPLIANT |
| RF-ENT-02 | Identificador duplicado | Caso 6: id 7 ya registrado → rechazo con mensaje, el programa continúa sin registrar el duplicado | ✅ COMPLIANT |
| RF-ENT-02 | Identificador inválido | Casos 7/8/9: id 0, id no numérico y nombre vacío → rechazo con mensaje sin terminar el programa | ✅ COMPLIANT |
| RF-ENT-03 | Guardado y recarga de entrenadores | Casos 2/4: carga de `data/entrenadores.txt` con 32 entrenadores y 0 rechazadas; el equipo de Ash se reconstruye (Bulbi id 101 nv12 → HP 35/35), stats re-derivadas con D2 y contador global fijado al máximo id del archivo (177 → siguiente 178, caso 12) | ✅ COMPLIANT |
| RF-EQP-01 | Especie inexistente rechazada | Caso 13: #151 → «no existe en la Pokédex» sin terminar; P7: `equipo_crear_ejemplar` devuelve NULL para #151 y #0; caso 19: línea con especie 999 rechazada en la carga | ✅ COMPLIANT |
| RF-EQP-02 | Ejemplar creado desde especie | P1/P3: Bulbasaur nivel 12 → HP 39 (id 1) / 35 (id 101) según D2, nivel 12, tipos copiados (Planta/Veneno), `hp_actual = hp_max`; fórmula documentada con variación determinista por id | ✅ COMPLIANT |
| RF-EQP-03 | Dos ejemplares de la misma especie difieren | P4: ids 1 y 2 al mismo nivel → variación 7 vs 14 ⇒ stats distintas; la excepción id 1 vs 17 (P5, misma variación 7 por módulo 16) es la razón exacta por la que el spec usa «normalmente difieren» (MAY) | ✅ COMPLIANT |
| RF-EQP-03 | Mutación solo del ejemplar | P6: `hp_actual` dañado (-20) → `hp_base` de la especie intacto (45); caso 23: sha256 de `pokedex.txt` idéntico tras crear/consultar ejemplares | ✅ COMPLIANT |
| RF-EQP-05 | Tamaño fuera de rango | Caso 12: 7.º ejemplar rechazado con 6/6 y mensaje; «El equipo es válido (tamaño 6, especies y niveles correctos)» | ✅ COMPLIANT |
| RF-EQP-05 | Nivel inválido | Casos 10/11: niveles 0 y 101 rechazados («Nivel inválido (rango 1-100).») y se solicita un nivel válido; 1 y 100 aceptados | ✅ COMPLIANT |

**Resumen de cumplimiento F2**: 10/10 escenarios del alcance F2 completos. 7/8 requisitos del alcance F2 (RF-EQP-04 «Backtracking de formación de equipos» NO es alcance de F2: está planificado en F4 — tasks.md F4.1/F4.4 — y su prototipo ya está declarado en `equipo.h` con las restricciones del diseño §4). Los 2 escenarios de RF-TEC-03 diferidos en F1 quedan verificados aquí (casos 6 y 10/11), como anticipó el resumen F1.

### Correctness (Evidencia estática, F2)

| Requisito | Estado | Notas |
|---|---|---|
| RF-ENT-01 | ✅ Implementado | `entrenador_registrar`: id > 0, nombre no vacío, id único (búsqueda previa), tope `MAX_ENTRENADORES=32`, contadores en cero y `equipo = NULL` |
| RF-ENT-02 | ✅ Implementado | Rechazo previo en `entrenador_registrar` y en `archivos_cargar_entrenadores` (ids vistos del archivo, `entero_en_arreglo`) |
| RF-ENT-03 | ✅ Implementado | `archivos_cargar_entrenadores`: parseo estricto por `;`, validación completa de la línea ANTES del commit, rollback sin entrenador/equipo a medio cargar, reporte aceptados/rechazados, `equipo_fijar_contador_id` al máximo del archivo |
| RF-EQP-01 | ✅ Implementado | `equipo_crear_ejemplar` valida la especie en origen (`pokedex_buscar_numero` → NULL); mismo chequeo en la carga de archivo y en `equipo_validar` |
| RF-EQP-02 | ✅ Implementado | D2 exacta con división entera truncada de C en el orden del diseño; `variacion = (id * 7) % 16` única por ejemplar aplicada a las 4 stats |
| RF-EQP-03 | ✅ Implementado | La especie se COPIA en el ejemplar (nombre, tipos, stats) y nunca se muta; el único campo mutable del ejemplar es `hp_actual`; consultas de Pokédex `const`-correctas |
| RF-EQP-05 | ✅ Implementado | `equipo_validar(pd, ent, tamano)`: tamaño 1..tamano, especies existentes, niveles 1..100 y tipos copiados válidos; `equipo_agregar_ejemplar` respeta `MAX_EQUIPO` |
| RF-TEC-03 (parte F2) | ⚠️ Parcial | Entradas inválidas (numéricas, duplicados, niveles) no terminan el programa, PERO los bucles internos de `crear_equipo` y `leer_entero` no detectan EOF: con fin de entrada a mitad del flujo repiten «Entrada inválida.» indefinidamente (verificado: ~6,1 M líneas en 3 s con `timeout`). El menú principal y `leer_linea` sí manejan EOF (caso 16). Anotado para F8 (`validacion.c`). |

### Coherencia con el Diseño (F2)

| Decisión del diseño | ¿Cumplida? | Notas |
|---|---|---|
| D2 (§3): `hp_max = (hp_base * nivel / 50) + nivel + 10 + variacion`; `stat = (stat_base * nivel / 50) + nivel + 5 + variacion`; `variacion = (id * 7) % 16` | ✅ Sí | Probe C exacto: id 1 nv12 → 39/35/35/34; id 2 nv100 → 214/217/217/209; id 101 nv12 → HP 35. División entera truncada en el orden exacto de la decisión |
| §5.2 formato `ID;NOMBRE;CANT;(ID_EJEMPLAR;NUM_ESPECIE;APODO;NIVEL)×CANT` | ✅ Sí | `data/entrenadores.txt`: 32 líneas, 3 campos base + 4 por ejemplar (máximo 27); CANT 0..6 (Max CANT=0, Paul/Gladion CANT=6); apodos sin `;` |
| §5.2 reglas: id único, especie en Pokédex, nivel 1..100, stats re-derivadas (no persistidas) | ✅ Sí | Casos 19–22: 4 variantes de línea inválida rechazadas con número de línea y la carga continúa; victorias/empates/derrotas/puntos no se persisten (se reconstruirán desde `resultados.txt` en F7) |
| §5.2 contador global de ids fijado al máximo del archivo | ✅ Sí | Tras cargar 32 líneas (ids de ejemplar hasta 177), el siguiente id es 178 (caso 12: ejemplares #178–#183) |
| Constantes: `NIVEL_MAX=100`, `TAM_EQUIPO_TORNEO=6`, `MAX_EQUIPO=6`, `MAX_ENTRENADORES=32` | ✅ Sí | `constantes.h` con los valores exactos adoptados (decisiones docentes resueltas); D7 (niveles 1–100) respetado por `equipo_validar` y `crear_equipo` |
| §1.3 firmas: `equipo_crear_ejemplar`, `equipo_agregar_ejemplar`, `equipo_validar`, `entrenador_registrar/buscar` | ✅ Sí | Firmas idénticas a las del diseño; ajustes del lote documentados en tasks.md F2.2 |
| `equipo.h` autónomo: declaración adelantada, sin ciclo de cabeceras | ✅ Sí | `typedef struct Entrenador Entrenador;` en `equipo.h` + tag `struct Entrenador` en `entrenador.h` (fix de F0); compilación estricta sin warnings |
| §13: verificación scriptada + probe de funciones puras, determinismo total (sin `srand`) | ✅ Sí | Batería 24 casos + probe C 7 comprobaciones, 50/50 PASS, `diff` reproducible por diseño D1/D2 |

### Hallazgos (F2)

**CRITICAL**: Ninguno.

**WARNING**: Ninguno.

**LOW**:
1. **Hallazgo del gatekeeper — bucles internos sin manejo de EOF en `crear_equipo`/`leer_entero`**: ante EOF a mitad del flujo (p. ej. tras pedir el nivel), el bucle repite «Entrada inválida.» indefinidamente porque `leer_entero` no distingue EOF de entrada no numérica y el bucle hace `continue`. Verificado empíricamente (~6,1 M de líneas en 3 s; el programa no termina). El menú principal (`leer_opcion`) y `leer_linea` sí manejan EOF (caso 16, cierre ordenado). No afecta a los casos scriptados del lote (la entrada termina con opción 12), pero viola el espíritu de RF-TEC-03 (ninguna condición de entrada debe colgar el programa). **Anotado para F8**: `validacion.c` unificará la lectura con detección de EOF. Sin CRITICAL y sin WARNING en F2.

**SUGGESTION**:
1. `equipo.h` declara `equipo_formar_backtracking` sin implementación en `equipo.c` — es el contrato de F4 (tasks.md F4.1/F4.4, diseño §4). Mantener el prototipo y no implementar antes de F4.
2. Comentarios Doxygen mixtos en el lote («Busqueda», «Opcion», «linea» sin tilde junto a bloques correctos): misma higiene ortográfica que las SUGGESTION 1 de F0/F1; unificar en el commit de higiene.
3. El desvío de presupuesto del lote (+44 líneas, +5,5 %) se debe al adelanto de `archivos.c` (alcance F7); quedó documentado y no se mutiló la documentación evaluable — mantener el criterio en los lotes siguientes.

### Veredicto F2

**PASS WITH WARNINGS**
El lote F2 cumple RF-ENT-01..03 y RF-EQP-01/02/03/05: las 5 tareas F2 están completas, el build es limpio (cero warnings, hash idéntico al envelope original), la batería re-derivada de 24 casos + probe C pasa 50/50 con la fórmula D2 exacta (39/35/35/34 y 214/217/217/209), el registro de entrenadores valida id/duplicado/inválido/vacío/tope, la carga reconstruye 32 entrenadores desde archivo con rechazo de líneas corruptas por número de línea, la especie nunca se muta (sha256 estable) y los 2 escenarios de RF-TEC-03 diferidos en F1 quedan cerrados. Un único hallazgo LOW (EOF en bucles internos, anotado para F8) no bloquea el lote; RF-EQP-04 queda diferido a F4 por plan.

---

## Sección F3 (Día 4, 2026-09-21) — Combate

**Fase**: F3 — combate.c + combate.h (daño D1, orden D3, empate D4, anti-empate D5) + cableado del combate amistoso en la opción 8 del menú
**Fecha**: 2026-09-21

### Completeness

| Métrica | Valor |
|---|---|
| Tareas del lote F3 | 3 |
| Tareas completadas | 3 |
| Tareas incompletas | 0 |

### Build y Ejecución

**Build**: ✅ Pasó (exit 0, cero warnings)
```text
$ make clean && make
mkdir -p build
gcc -std=c99 -Wall -Wextra -o build/torneo src/archivos.c src/combate.c src/entrenador.c src/equipo.c src/main.c src/pokedex.c src/tipos.c
```
Evidencia: `build_exit_code=0`, cero warnings con `-Wall -Wextra` sobre los siete `.c` del proyecto. Hash del build: `deccd336…`. 0 líneas de más de 99 columnas en `src/*.c`/`src/*.h` (awk).

**Pruebas**: ✅ Batería F3 scriptada (16 casos stdin→grep) + probe F3 (21 comprobaciones): **37/37 PASS, exit 0**.

| Caso | Entrada | Resultado esperado | Resultado real | Diff |
|---|---|---|---|---|
| 1 build | `make clean && make` | exit 0, cero warnings, siete `.c` compilados | Ídem | ✅ PASS |
| 2 B1 victoria por agotamiento | `8\n1\n1\n2\n1\n1\n1\n1\n1\n12\n` | Ash vs Misty: KO de Llama y reemplazos, «Victoria de Misty: Ash se queda sin Pokémon disponibles (KOs: 1 a 3)» | Ídem | ✅ PASS |
| 3 B2 empate 20 turnos | `8\n1\n5\n4\n1\n3\n12\n` | Jessie vs Gary (fantasma vs normal, daño 0): «Empate tras 20 turnos (fase de grupos)» | Ídem | ✅ PASS |
| 4 B3 anti-empate eliminatoria | `8\n2\n5\n4\n1\n3\n12\n` | «Desempate en eliminatoria tras 20 turnos: HP total 108-424, nivel total 44-133; gana Gary» | Ídem | ✅ PASS |
| 5 B4a entrenador inexistente | `8\n1\n99\n2\n12\n` | «No existe un entrenador con id 99.» sin terminar | Ídem | ✅ PASS |
| 6 B4b mismo entrenador | `8\n1\n1\n1\n12\n` | «Deben ser dos entrenadores distintos.» | Ídem | ✅ PASS |
| 7 B4c entrenador sin equipo | `8\n1\n9\n2\n12\n` | «Ambos entrenadores deben tener un equipo válido (1-6 ejemplares).» | Ídem | ✅ PASS |
| 8 B4d modo inválido | `8\n3\n12\n` | «Opción inválida. Intente de nuevo.» | Ídem | ✅ PASS |
| 9 B4e posición inválida | `8\n1\n1\n2\n99\n1\n1\n1\n1\n1\n12\n` | «Posición inválida (1-3).» + reintento exitoso + victoria | Ídem | ✅ PASS |
| 10 B5 volver y salir | `8\n0\n12\n` | Vuelve al menú, «Saliendo del programa.», exit 0 | Ídem | ✅ PASS |
| 11 B6 EOF en selección | `8\n1\n1\n2\n1\n` | «Combate cancelado.», cierre ordenado, exit 0 (no cuelga) | Ídem | ✅ PASS |
| 12 B6b EOF en submenú | `8\n` | Submenú mostrado, cierre ordenado, exit 0 (no cuelga) | Ídem | ✅ PASS |
| 13 sha256 datos | batería completa | `data/pokedex.txt` y `data/efectividad.txt` byte-idénticos antes/después (RF-PDX-04) | Ídem | ✅ PASS |
| P1–P7 probe F3 | probe contra `src/{combate,equipo,entrenador,pokedex,tipos}.c` | D1 exacto: ×2=46, ×1=21, ×0.5=7, ×0=0, ×4=88, ×0.25=4, mínimo 1 (mult 0.25, base 2); D3 60→local, 45→visita, 60=60→local; D4 empate; D5a HP, D5b nivel, D5c entrenador 1; CMB-04 con y sin reemplazo; validación NULL/equipo vacío | Ídem | ✅ PASS |

### Matriz de Cumplimiento de Specs (F3)

| Requisito | Escenario | Evidencia | Resultado |
|---|---|---|---|
| RF-CMB-01 | Inicio de combate | B1/B2: cada entrenador selecciona su Pokémon inicial vía prompts de posición (1..N); la selección se valida y reintenta (B4e) | ✅ COMPLIANT |
| RF-CMB-02 | Mayor velocidad ataca primero | Probe P: 60 vs 45 → ataca primero el local; 45 vs 60 → el visitante | ✅ COMPLIANT |
| RF-CMB-02 | Igual velocidad, desempate determinista | Probe P: 60 vs 60 → ataca el ejemplar del entrenador 1 (local, D3) | ✅ COMPLIANT |
| RF-CMB-03 | Daño normal (×1) | Probe P: Jigglypuff→Bulbasaur = 21 (base 21 × 1.0) | ✅ COMPLIANT |
| RF-CMB-03 | Daño superefectivo (×2) | Probe P: Charmander→Bulbasaur = 46 (base 23 × 2.0) | ✅ COMPLIANT |
| RF-CMB-03 | Daño poco efectivo (×0.5) | Probe P: Jigglypuff→Geodude = 7 (base 15 × 0.5) | ✅ COMPLIANT |
| RF-CMB-03 | Sin efecto (×0) | Probe P: Pikachu→Geodude = 0 (Eléctrico→Tierra) | ✅ COMPLIANT |
| RF-CMB-03 | Defensor con dos tipos (producto) | Probe P: ×4 Fuego→Bicho/Planta = 88; ×0.25 Fuego→Roca/Agua = 4 | ✅ COMPLIANT |
| RF-CMB-04 | Derrota de un Pokémon | B1: HP llega a 0 → «¡debilitado!» y reemplazo; probe: daño 444 sobre HP 10 → KO | ✅ COMPLIANT |
| RF-CMB-04 | Victoria por agotamiento | B1: Misty sin disponibles tras KO de Bulbi → «Victoria de Misty… se queda sin Pokémon disponibles»; probe: KO sin reemplazo → victoria inmediata | ✅ COMPLIANT |
| RF-CMB-05 | Consulta de la tabla de efectividad | La matriz 18×18 (F1) alimenta `combate_calcular_danio` vía `tipos_multiplicador`; los 7 multiplicadores del probe coinciden con `data/efectividad.txt` | ✅ COMPLIANT |

**Resumen de cumplimiento F3**: 11/11 escenarios del alcance F3 completos (5/5 requisitos RF-CMB-01..05). El escenario «Mínimo 1 si hay efecto» (D1, RF-CMB-03) se verifica en el probe P (base 2 × 0.25 → daño 1).

#### Veredicto por requisito (verificación formal, 2026-09-21)

| Requisito | Veredicto | Evidencia (1 línea) |
|---|---|---|
| RF-CMB-01 | ✅ COMPLIANT | B1/B2 re-ejecutados: prompts «Selección de Pokémon activo (local/visita)» para ambos entrenadores con selección validada 1..N (B4e reintenta) |
| RF-CMB-02 | ✅ COMPLIANT | Probe re-ejecutado: 60 vs 45 → local; 45 vs 60 → visita; 60 vs 60 → local (entrenador 1, D3) |
| RF-CMB-03 | ✅ COMPLIANT | Probe re-ejecutado: daño ×2=46, ×1=21, ×0.5=7, ×0=0, ×4=88, ×0.25=4, mínimo 1 (base 2 × 0.25) |
| RF-CMB-04 | ✅ COMPLIANT | B1 re-ejecutado: KO de Llama/Pato/Sparky/Bulbi con reemplazos y «Victoria de Misty… (KOs: 1 a 3)»; probe: KO sin reemplazo → victoria inmediata |
| RF-CMB-05 | ✅ COMPLIANT | Probe re-ejecutado: 7 multiplicadores coinciden con `data/efectividad.txt` vía `tipos_multiplicador` (matriz 18×18) |

#### Revisión de manejo de bordes (RF-TEC-03 / RF-CMB-01..04)

| Borde | Evidencia re-ejecutada | Resultado |
|---|---|---|
| Equipos vacíos | B4c: entrenador sin equipo → «Ambos entrenadores deben tener un equipo válido (1-6 ejemplares).»; probe: `combate_ejecutar` con equipo vacío → false | ✅ PASS |
| NULL | Probe: entrenador NULL, `seleccionar` NULL y equipo vacío → `combate_ejecutar` devuelve false sin crash; `combate_calcular_danio(NULL,…)` → 0; `combate_ataca_primero(NULL,…)` → true (no dereferencia) | ✅ PASS |
| Selección fuera de rango (1..N con reintento) | B4e: posición 99 → «Posición inválida (1-3).» + reintento exitoso y combate completo; `seleccionar_pokemon_activo` valida `posicion < 1 || posicion > disponibles` y repregunta | ✅ PASS |
| EOF en selección (no colgar) | B6 + verificación con `timeout 5`: `8\n1\n1\n2\n1\n` → «Combate cancelado.», exit 0 en < 5 s (no cuelga); probe: devuelve -1 → cancelación | ✅ PASS |
| EOF en submenú | B6b + `timeout 5`: `8\n` → submenú mostrado y cierre ordenado, exit 0 (no cuelga) | ✅ PASS |
| Inmutabilidad de datos | sha256 de `data/pokedex.txt` y `data/efectividad.txt` idénticos antes/después de la batería completa (RF-PDX-04) | ✅ PASS |

### Correctness (Evidencia estática, F3)

| Requisito | Estado | Notas |
|---|---|---|
| RF-CMB-01 | ✅ Implementado | `combate_ejecutar` pide el Pokémon inicial de cada entrenador vía callback `CombateSeleccionar` (la lectura vive en `main.c`, no en el motor) |
| RF-CMB-02 | ✅ Implementado | `combate_ataca_primero`: `local->velocidad >= visita->velocidad` (mayor velocidad primero; empate → entrenador 1) |
| RF-CMB-03 | ✅ Implementado | `combate_calcular_danio`: D1 exacta con enteros en el orden del diseño; `(int)(base*mult)`; mínimo 1 con mult > 0; 0 con mult == 0; guarda división por defensa 0 |
| RF-CMB-04 | ✅ Implementado | `hp_actual` con piso 0; KO suma el derrotado al atacante (`kos_local`/`kos_visita`) y pide reemplazo; sin disponibles → victoria del otro |
| RF-CMB-05 | ✅ Implementado | Reutiliza `tipos_multiplicador` (producto de ambos tipos del defensor, matriz 18×18 de F1) |
| D4/D5 (diseño §7 paso 5) | ✅ Implementado | 20 turnos con ambos vivos: grupos → `res.empate = true`; eliminatoria → cadena HP total → nivel total → entrenador 1 |
| D2 restauración | ✅ Implementado | `combate_restaurar_hp`: `hp_actual = hp_max` en todos los ejemplares al iniciar cada combate |

### Coherencia con el Diseño (F3)

| Decisión del diseño | ¿Cumplida? | Notas |
|---|---|---|
| D1 (§3): fórmula exacta con enteros en ese orden y mínimo 1 / sin efecto 0 | ✅ Sí | Probe F3: los 7 casos de multiplicador (2/1/0.5/0, ×4, ×0.25, mínimo 1) coinciden byte a byte con el cálculo manual documentado en el apply-progress |
| D3 (§3): mayor velocidad, empate → entrenador 1 | ✅ Sí | Probe: 60 vs 45, 45 vs 60 y 60 vs 60 (local) |
| D4 (§3/§7): `MAX_TURNOS_COMBATE=20`, turno = intercambio completo, empate en grupos | ✅ Sí | B2: fantasma vs normal (daño 0 mutuo) → «Empate tras 20 turnos (fase de grupos)» |
| D5 (§3/§7): cadena HP total → nivel total → entrenador 1 | ✅ Sí | B3 por HP (108-424) y probe D5a (HP), D5b (nivel), D5c (entrenador 1) |
| §7 paso 1: restaurar `hp_actual = hp_max` | ✅ Sí | `combate_restaurar_hp` antes del primer turno; los combates del probe D4/D5 arrancan con HP completo |
| §7 paso 3e: KO → el atacante suma derrotado, el defensor elige reemplazo | ✅ Sí | B1: 3 KOs de Misty y 1 de Ash con reemplazos intermedios; KOs contabilizados en `ResultadoCombate` |
| §1.3 firmas: `combate_calcular_danio(const Ejemplar*, const Ejemplar*)` y `combate_ataca_primero(const Ejemplar*, const Ejemplar*)` | ✅ Sí | Idénticas a las del diseño |
| §1.3 firma `combate_ejecutar(local, visita, res)` | ⚠️ Extendida | El diseño exige la fase en §7 paso 5 pero omite el parámetro en §1.3: la firma ganó `bool es_eliminatoria` y el callback `CombateSeleccionar` (el motor no lee la consola; `main` provee la lectura validada, RF-TEC-02). Delta documentado en tasks.md F3.1 y en el apply-progress |
| §2.2: el combate no consulta la Pokédex (tipos/stats copiados en el Ejemplar) | ✅ Sí | `combate.c` usa solo campos de `Ejemplar`; sha256 de `pokedex.txt` y `efectividad.txt` intactos tras 12 combates ejecutados |

### Hallazgos (F3)

**CRITICAL**: Ninguno.

**WARNING**: Ninguno.

**LOW**:
1. **Submenú de combate con un solo intento**: una opción inválida en el submenú (p. ej. `3`) imprime el aviso y vuelve al menú principal en lugar de repreguntar como hace el submenú de la Pokédex. No viola RF-MEN-01 (el menú principal re-muestra) ni RF-TEC-03 (el programa no termina); la unificación de lecturas llegará en F8 (`validacion.c`).
2. **Empate por «ambos sin disponibles en el mismo turno» (D4) inalcanzable en la práctica**: el segundo atacante siempre conserva su activo tras el intercambio, así que la rama de doble agotamiento del bucle solo puede darse por construcción; se mantiene por contrato (tasks.md F3.1) sin impacto observable.

**SUGGESTION**:
1. **Numeración de `docs/planificacion.md`**: desde F1 la tabla del plan etiqueta la fase de combate como «F4» (drift +1 vs `tasks.md`, que la llama F3, porque F1 quedó partida en dos filas). Este lote marcó la fila por contenido («Combate…» → ✅ Hecho) y no por número; conviene unificar la numeración en F10 cuando se cierre el plan 14/14.
2. **Ortografía mixta en comentarios nuevos** («posicion», «Pokemon», «danio» sin tilde en `combate.c`/`main.c`): misma higiene pendiente acumulada de F0/F1/F2; unificar en el commit de higiene previsto.
3. La firma extendida de `combate_ejecutar` (es_eliminatoria + callback) debe reflejarse en `docs/informe-tecnico.md` en F10.1 junto al resto de D1–D10.

### Veredicto F3

**PASS**
El lote F3 cumple RF-CMB-01..05 y las decisiones D1/D2/D3/D4/D5: las 3 tareas F3 están completas, el build es limpio (cero warnings, siete `.c`, hash `deccd336…` reproducido en la verificación formal), la batería scriptada pasa 16/16 y el probe 21/21 re-ejecutados en esta fase (evidencia fresca: daño exacto 46/21/7/0/88/4 y mínimo 1, orden D3 60/45/60, empate D4 por 20 turnos, cadena D5 HP→nivel→entrenador 1, KO con/sin reemplazo, validación NULL/equipo vacío), la selección de Pokémon activo se valida con reintentos y EOF ordenado sin colgar (RF-TEC-03) y la Pokédex permanece inmutable (sha256 estable). Sin CRITICAL ni WARNING; dos LOW informativos no bloquean. Presupuesto del lote: 546 líneas de código nuevas (≤ 800 ✓).

---

## Sección F4 (Día 4, 2026-09-21) — Backtracking de formación de equipos

**Fase**: F4 — `equipo_formar_backtracking` + `bt_rec` con podas §4.3 (RF-EQP-04) + cableado de la formación automática en la opción 3 del menú
**Fecha**: 2026-09-21

### Completeness

| Métrica | Valor |
|---|---|
| Tareas del lote F4 | 4 |
| Tareas completadas | 4 |
| Tareas incompletas | 0 |

### Build y Ejecución

**Build**: ✅ Pasó (exit 0, cero warnings)
```text
$ make clean && make
rm -rf build
mkdir -p build
gcc -std=c99 -Wall -Wextra -o build/torneo src/archivos.c src/combate.c src/entrenador.c src/equipo.c src/main.c src/pokedex.c src/tipos.c
```
Evidencia: `build_exit_code=0`, cero warnings con `-Wall -Wextra` sobre los siete `.c` del proyecto. Hash de la salida del build: `deccd336…` (idéntico al de F3 porque la línea de compilación no cambió; el binario sí se regeneró con el código nuevo). 0 líneas de más de 99 columnas en `src/*.c`/`src/*.h`. Presupuesto del lote: **442 líneas de código nuevas** (equipo.c +285, equipo.h +38, main.c +178 incluyendo el fix de EOF) ≤ 800 ✓.

**Pruebas**: ✅ Batería F4 scriptada (12 casos stdin→grep) + probe F4 (24 comprobaciones): **36/36 PASS, exit 0**. Hash del output de pruebas: `6b31bcaa…`. Re-ejecutada íntegramente en la verificación formal (2026-09-21): **12/12 + 24/24 PASS reproducidos byte a byte** (hash del output combinado: `b8ca21e9…`), más el probe de fugas (200 000 iteraciones, PASS, ver «Revisión de fugas» más abajo).

| Caso | Entrada | Resultado esperado | Resultado real | Diff |
|---|---|---|---|---|
| 1 B1 factible | `3\n2\n1\n3\n100\n2\n12\n` | «Equipo generado: 3 ejemplar(es) que cumplen todas las restricciones.» + «--- Equipo de Ash (id 1): 3 ejemplar(es) ---» con 3 ejemplares (Bulbasaur/Ivysaur/Venusaur nivel 1, Planta/Veneno) | Ídem | ✅ PASS |
| 2 B2 sin solución | `3\n2\n1\n6\n5\n1\n12\n` | «No existe un equipo que cumpla las restricciones indicadas.»; NO aparece «cumplen todas las restricciones»; medición de tiempo real 7 ms (poda de cota inferior en profundidad 1) | Ídem | ✅ PASS |
| 3 B3 validaciones | `3\n2\n1\n0\n7\n3\n0\n100\n0\n19\n2\n12\n` | «Cantidad inválida: debe estar entre 1 y 6.», «Nivel total inválido: debe ser positivo.», «Cantidad de tipos inválida: debe estar entre 1 y 18.» y luego equipo generado (reintento aceptado) | Ídem | ✅ PASS |
| 4 B4 entrenador inexistente | `3\n2\n999\n12\n` | «No existe un entrenador con id 999.» sin terminar | Ídem | ✅ PASS |
| 5 B5 opción inválida del submenú | `3\n9\n12\n` | «Opción inválida. Intente de nuevo.» | Ídem | ✅ PASS |
| 6 B6 EOF en submenú | `3\n` | Submenú mostrado, cierre ordenado, exit 0 (no cuelga) | Ídem | ✅ PASS |
| 7 B6b EOF en restricciones | `3\n2\n1\n` | EOF en «Cantidad de Pokémon» → cierre ordenado, exit 0 (no cuelga) | Ídem | ✅ PASS |
| 8 B7 sha256 datos | batería completa | `data/pokedex.txt` y `data/efectividad.txt` byte-idénticos antes/después (RF-PDX-04) | Ídem | ✅ PASS |
| 9 B8 columnas | `grep -RInE '.{100,}'` | 0 líneas > 99 columnas en `equipo.c`/`equipo.h`/`main.c` | Ídem | ✅ PASS |
| P1–P7 probe F4 | probe contra `src/{equipo,pokedex,tipos}.c` | P1 factible 3/100/2 → true, 3 ejemplares, especies en Pokédex, niveles 1..100, nivel total ≤ 100, ≥ 2 tipos, sin repetidas (6 µs); P2 sin solución 6/nivel 5 → false, salida NULL, cantidad 0, poda instantánea 2 µs; P3 min_tipos 18 con 6 ejemplares → false, poda 3b 2 µs; P4 especies permitidas {1} + repetidas → Bulbasaur×3; P5 ataque objetivo 150 → true, ataque total 236; P6 cantidad 0/7 y min_tipos 19 rechazados | Ídem | ✅ PASS |

### Matriz de Cumplimiento de Specs (F4)

| Requisito | Escenario | Evidencia | Resultado |
|---|---|---|---|
| RF-EQP-04 | Equipo que cumple todas las restricciones | B1 + P1: restricciones {3, 100, ≥2 tipos} → equipo de 3 ejemplares que cumple TODAS (cantidad exacta, nivel total 3 ≤ 100, 2 tipos distintos Planta/Veneno) y se muestra por consola | ✅ COMPLIANT |
| RF-EQP-04 | Sin solución informa que no existe | B2 + P2: {6, nivel total máx 5} → «No existe un equipo que cumpla las restricciones indicadas.», `equipo_formar_backtracking` devuelve false, `*salida = NULL` y `*cantidad = 0` (no se devuelve un equipo inválido); la poda de cota inferior corta en profundidad 1 (8 ms en el menú, 3 µs en el probe) | ✅ COMPLIANT |
| RF-EQP-04 | Restricciones de integrantes | P4: lista de especies permitidas {1} + `permitir_repetidas` → equipo Bulbasaur×3 (la restricción excluye el resto de especies) | ✅ COMPLIANT |
| RF-EQP-04 | Estadísticas objetivo | P5: `ataque_total_min = 150` → niveles escalados al presupuesto (Bulbasaur nivel 98) y ataque total 236 ≥ 150 | ✅ COMPLIANT |
| RF-MEN-01 | Opción 3 ofrece formación automática | B1/B2: submenú «1. Formación manual / 2. Formación automática (backtracking) / 0. Volver» con validación de restricciones y reintentos (B3) | ✅ COMPLIANT |
| RF-TEC-03 | Entrada inválida no termina el programa | B3/B4/B5: restricciones fuera de rango, entrenador inexistente y opción inválida → mensajes y el programa continúa; EOF en submenú y en restricciones cierra ordenado (B6/B6b, timeout 5 s sin colgar) | ✅ COMPLIANT |

**Resumen de cumplimiento F4**: 6/6 escenarios del alcance F4 completos (RF-EQP-04 con sus 4 escenarios de restricciones + RF-MEN-01 + RF-TEC-03). RF-EQP-04 queda **verificado por completo** — era el requisito diferido desde F2.

#### Veredicto por requisito (verificación formal, 2026-09-21)

| Requisito | Veredicto | Evidencia (1 línea) |
|---|---|---|
| RF-EQP-04 (backtracking) | ✅ COMPLIANT | B1/P1 re-ejecutados: {3, 100, ≥2 tipos} → true, 3 ejemplares, especies en Pokédex, niveles 1..100, nivel total 3 ≤ 100, ≥ 2 tipos (Planta/Veneno), sin repetidas, y el equipo se muestra por consola («Equipo generado: 3 ejemplar(es)…» + listado) |
| RF-EQP-04 (sin solución) | ✅ COMPLIANT | B2/P2 re-ejecutados: {6, nivel total máx 5} → «No existe un equipo que cumpla las restricciones indicadas.», `false`, `*salida = NULL`, `*cantidad = 0` (nunca un equipo inválido), poda de cota inferior en profundidad 1 (7 ms en menú, 2 µs en probe) |
| RF-EQP-04 (min_tipos alto, poda) | ✅ COMPLIANT | P3 re-ejecutado: {6, 100, 18 tipos} → `false` instantáneo (poda 3b, 2 µs), `*salida = NULL`; la poda 3 (sufijo de tipos) y la 3b acotan el peor caso de §4.5 |
| RF-EQP-04 (especies permitidas/restringidas) | ✅ COMPLIANT | P4 re-ejecutado: lista permitida {1} + `permitir_repetidas` → Bulbasaur×3 (la restricción excluye el resto de especies; una no admitida solo se excluye) |
| RF-EQP-04 (restricciones inválidas rechazadas) | ✅ COMPLIANT | B3/P6 re-ejecutados: cantidad 0/7, nivel total 0, tipos 0/19 → rechazo con mensaje en menú (reintento hasta entrada válida) y `false` + `*salida = NULL` en la API; B4: entrenador inexistente rechazado sin terminar |
| RF-EQP-04 (recursividad real) | ✅ COMPLIANT | Inspección estática de `equipo.c` (513 líneas): `bt_rec` recursiva pura sobre el índice de especie con ramificación k-copias/excluir, caso base éxito con las 3 condiciones (cantidad, tipos, ataque) y salida temprana; no es una simulación iterativa |

#### Revisión de fugas (bt_rec, §4.4) — verificación formal 2026-09-21

Inspección estática 1:1 del ciclo de vida de cada ejemplar en `equipo.c`:

| Ruta de creación | Liberación | Resultado |
|---|---|---|
| `equipo_crear_ejemplar` (malloc) en la rama de inclusión, iteraciones `j = 0..k-1` (líneas 410-425) | `free(descarte)` en el bucle de retroceso `while (j > 0)` (líneas 436-442), que desengancha de `*parcial` y decrementa `*creados` | ✅ Cada ejemplar descartado se libera exactamente una vez |
| Ramificación con `k` copias donde la recursión hija devuelve `false` | Mismo bucle de retroceso libera las `k` copias antes de probar la exclusión | ✅ Sin ejemplares huérfanos en ramas fallidas |
| `break` por `ej == NULL` (malloc fallido, línea 412-414) | El bucle `while (j > 0)` libera las `j` copias ya creadas de esa rama (no alcanza `j == k`, nunca se llama a la recursión con copias sin crear) | ✅ Sin fuga en el camino de fallo de memoria |
| Éxito: `bt_rec` devuelve `true` con `restantes == 0` | La lista completa (n = `r->cantidad` ejemplares) es propiedad del llamador; `equipo_formar_backtracking` la invierte y la entrega; `main` la asigna con `equipo_asignar` y la libera con `equipo_liberar` al salir; el probe la libera con `liberar_lista` | ✅ Propiedad transferida, sin doble free |
| Invariante `*creados` | Vuelve a 0 tras cada fracaso (P2/P3/P6 lo observan vía `*cantidad == 0` y `*salida == NULL`); al éxito vale exactamente `r->cantidad` | ✅ Verificado |

Evidencia empírica (sin valgrind, prohibido por diseño §8): probe `/tmp/opencode/probe_fugas_f4` re-ejecutado — **200 000 iteraciones** alternando caso sin solución (6/nivel 5, crea y libera constantemente) y factible (3/100/2): **0 fallas de invariante** (`*salida == NULL` y `*cantidad == 0` en TODO fracaso; `cantidad == 3` en todo éxito) y **RSS estable 2072 → 2072 KB (delta 0 KB)**. Conclusión: no se observan fugas de memoria en `bt_rec`; el hallazgo se registra como conforme (sin hallazgo adverso).

### Correctness (Evidencia estática, F4)

| Requisito | Estado | Notas |
|---|---|---|
| RF-EQP-04 | ✅ Implementado | `equipo_formar_backtracking(pd, r, salida, cantidad)` + `bt_rec` recursiva sobre el índice de especie: ramifica sobre k copias (1..restantes si `permitir_repetidas`, 1 si no) y luego excluye; caso base éxito con `restantes == 0 && tipos_dist >= min_tipos && ataque_acum >= ataque_total_min`; salida temprana en el primer éxito; `*salida = NULL` en todo fracaso |
| Podas §4.3 (diseño) | ✅ Implementado | 1) cantidad: `restantes > pd->cantidad - i`; 2) cota inferior de nivel: `nivel_acum + restantes*NIVEL_MIN > nivel_total_max`; 3) cota de tipos con tabla sufijo `uint32_t tipos_sufijo[POKEDEX_MAX+1]` (O(18·150)) y `tipos_dist + popcount(sufijo[i] & ~mascara) < min_tipos`; 5) salida temprana. Poda extra 3b (`tipos_dist + 2*restantes < min_tipos`): acota el peor caso «min_tipos alto» que §4.5 daba por cubierto (un ejemplar aporta a lo sumo 2 tipos); conservadora (nunca corta ramas factibles) y verificada en P3 |
| §4.4 Liberación al retroceder | ✅ Implementado | Cada copia creada en una rama se libera con `free` si la recursión no devuelve true; invariante `*creados` (ejemplares vivos) vuelve a 0 tras cada fracaso (P2/P3/P6 lo observan); al éxito la lista (n = `r->cantidad`) es propiedad del llamador y `main` la libera con `equipo_liberar` al salir; revisión manual 1:1 de cada `equipo_crear_ejemplar` (malloc) → `free` |
| RF-MEN-01 / RF-TEC-03 | ✅ Implementado | Opción 3 con submenú manual/automática/volver; restricciones validadas con reintentos (cantidad 1..6, nivel total ≥ 1, tipos 1..18); `leer_entero` pasa a triestado (1 = leído, 0 = no numérico con descarte, -1 = EOF) y todas sus llamadas distinguen EOF → **cierra el hallazgo LOW de F2** (EOF en bucles internos colgaba el programa) |

### Coherencia con el Diseño (F4)

| Decisión del diseño | ¿Cumplida? | Notas |
|---|---|---|
| §4.1 firma `equipo_formar_backtracking(pd, r, salida, cantidad)` | ✅ Sí | Idéntica a la del diseño (ya declarada en `equipo.h` desde F0/F2) |
| §4.1 firma `bt_rec(i, restantes, nivel_acum, tipos_distintos, ataque_acum, tipos_vistos, parcial, creados)` | ⚠️ Adaptada | Se mantiene la recursión pura sobre `i` con los mismos acumuladores y `*creados`; `bool tipos_vistos[CANT_TIPOS]` se reemplaza por `uint32_t mascara_tipos` (el propio §4.2 describe el estado como «máscara de tipos») y se añaden `tipos_sufijo` (tabla precomputada de la poda 3) y el nivel/estado local de rama |
| §4.2 nivel del ejemplar «mínimo factible dentro del presupuesto restante; si hay objetivo de ataque se escala el nivel del integrante con mejor base» | ⚠️ Adaptada | Sin objetivo de ataque → nivel `NIVEL_MIN` (mínimo factible, la poda 2 garantiza el presupuesto); con `ataque_total_min > 0` → nivel al máximo que permite el presupuesto restante para cada ejemplar (superset determinista del criterio «mejor base», sin estado adicional); documentado en el código y en tasks.md F4.1 |
| §4.3 podas 1/2/3/5 | ✅ Sí | Exactas salvo el límite real `pd->cantidad` en la poda 1 (equivale a `POKEDEX_MAX` cuando la carga es completa; más robusto si no lo está) |
| §4.3.4 restricciones de integrantes aplicadas al generar el candidato | ✅ Sí | `bt_especie_admitida` (lista `especies_permitidas`/`cantidad_permitidas`, extensión del struct §2 pedida por el contrato F4.1) y `permitir_repetidas` controlan la rama de inclusión; una especie no admitida solo se excluye |
| §4.4 sin solución: false + informe sin equipo parcial | ✅ Sí | `*salida = NULL`, `*cantidad = 0`; el menú informa «No existe un equipo que cumpla las restricciones indicadas.» y conserva el equipo anterior |
| §4.5 complejidad y memoria | ✅ Sí | Tabla sufijo O(18·150); profundidad ≤ 6 ejemplares vivos por rama; el caso imposible del spec corta en profundidad 1 (poda 2); el peor caso min_tipos alto queda acotado por la poda 3b (verificada: 2 µs) |
| §2 struct `RestriccionesEquipo` | ⚠️ Extendido | Se añaden `especies_permitidas[POKEDEX_MAX]` y `cantidad_permitidas` (600 bytes estáticos, dominio acotado) — el diseño §2 solo fijaba 5 campos; la extensión cumple el contrato F4.1 («especies permitidas») y RF-EQP-04 («restricciones sobre integrantes») |
| §1.3 `equipo.h` autónomo, TDA encapsulado | ✅ Sí | `equipo_asignar(ent, equipo)` nueva función pública que libera el equipo anterior y asigna la lista generada; solo `equipo.c` toca el campo `siguiente` (main no rompe el encapsulamiento) |

### Hallazgos (F4)

**CRITICAL**: Ninguno.

**WARNING**: Ninguno.

**LOW**:
1. **Cierre del LOW de F2**: el hallazgo LOW 1 de F2 (bucles internos de `crear_equipo`/`leer_entero` sin detección de EOF → repetición indefinida de «Entrada inválida.») quedó **resuelto en F4**: `leer_entero` ahora devuelve -1 ante EOF y todas sus llamadas (formación automática, creación manual, Pokédex, entrenador, combate) distinguen cierre ordenado de entrada no numérica (B6/B6b con timeout 5 s). La unificación en `validacion.c` (F8) seguirá adelante por plan.

**SUGGESTION**:
1. Ortografía mixta en los comentarios nuevos de `equipo.c` (sin tildes en identificadores/acentos en algunos comentarios): misma higiene pendiente acumulada de F0–F3; unificar en el commit de higiene previsto.
2. La poda extra 3b y las adaptaciones de firma (`mascara_tipos`, `tipos_sufijo`, `equipo_asignar`, struct extendido) deben reflejarse en `docs/informe-tecnico.md` en F10.1 junto al resto de D1–D10.

### Veredicto F4

**PASS**
El lote F4 cumple RF-EQP-04 (backtracking completo, diferido desde F2), RF-MEN-01 (formación automática en la opción 3) y RF-TEC-03 (validación con reintentos y EOF ordenado): las 4 tareas F4 están completas, el build es limpio (cero warnings), la batería scriptada pasa 12/12 y el probe 24/24 re-ejecutados en la verificación formal (factible 3/100/2 cumple TODAS las restricciones y se muestra; sin solución 6/nivel 5 → informe y salida NULL con poda instantánea de 2 µs; min_tipos 18 inalcanzable con poda 3b en 2 µs; especies permitidas + repetidas; ataque objetivo 150 con niveles escalados; restricciones inválidas rechazadas), la liberación al retroceder es 1:1 (revisión estática de cada malloc/free en `bt_rec` + probe de fugas de 200 000 iteraciones con RSS estable 2072→2072 KB, 0 fallas de invariante) y la Pokédex permanece inmutable (sha256 estable). Sin CRITICAL ni WARNING; un LOW informativo (cierre del LOW de F2) no bloquea. Presupuesto del lote: 442 líneas de código nuevas (≤ 800 ✓).

---

## Sección F5 (Día 5, 2026-09-21) — Torneo: grupos + clasificación

**Fase**: F5 — `src/torneo.c` (grupos A–H, calendario 1–48, puntuación 3/1/0, desempates RF-TRN-04, clasificados RF-TRN-06) + cableado de las opciones 7 y 8 del menú
**Fecha**: 2026-09-21

### Completeness

| Métrica | Valor |
|---|---|
| Tareas del lote F5 | 3 |
| Tareas completadas | 3 |
| Tareas incompletas | 0 |

### Build y Ejecución

**Build**: ✅ Pasó (exit 0, cero warnings)
```text
$ make clean && make
rm -rf build
mkdir -p build
gcc -std=c99 -Wall -Wextra -o build/torneo src/archivos.c src/combate.c src/entrenador.c src/equipo.c src/main.c src/pokedex.c src/tipos.c src/torneo.c
```
Evidencia: `build_exit_code=0`, cero warnings con `-Wall -Wextra` sobre los **ocho** `.c` del proyecto (se incorpora `src/torneo.c`). Hash de la salida del build: `72726e48…` (cambió respecto a F4 porque la línea de compilación ahora incluye `torneo.c`). 0 líneas de más de 99 columnas en `src/torneo.c`/`src/torneo.h`/`src/main.c`. Presupuesto del lote: **570 líneas de código nuevas** (torneo.c +441, torneo.h +38, main.c +91) ≤ 800 ✓.

**Pruebas**: ✅ Batería F5 scriptada (17 casos stdin→grep) + probe F5 (104 comprobaciones) + probe extra F5 (23 comprobaciones) + verificación awk del calendario: **144/144 PASS + awk PASS, exit 0**. Hash del output de pruebas: `fd70d362…`.

**Verificación formal (2026-09-21, re-ejecución íntegra)**: la batería (17/17) y el probe (104/104) se re-ejecutaron en esta verificación formal con resultados **byte-idénticos a los del apply** (`test_output_hash sha256:fd70d362…` y `evidence_revision sha256:65825f0f…` reproducidos sin cambios), confirmando la reproducibilidad de la evidencia. Se añadió el probe extra `/tmp/opencode/probe_f5_extra` (23/23 PASS: rechazo D10 con 30 entrenadores, 33 inalcanzable por capacidad del registro RF-ENT-01 con estado intacto, 32 aceptados con los 8 grupos A–H derivados del calendario — cada integrante juega exactamente 3 combates — y 48 combates numerados 1..48) y la verificación independiente del calendario con `awk` sobre la salida real del binario (48 combates, 6 por grupo A–H, 48 pares únicos sin repetidos en ningún orden, cada entrenador aparece exactamente 3 veces).

| Caso | Entrada | Resultado esperado | Resultado real | Diff |
|---|---|---|---|---|
| 1 build | `make clean && make` | exit 0, cero warnings, ocho `.c` compilados | Ídem | ✅ PASS |
| 2 rechazo ≠32 | renombrar entrenadores.txt + `7\n12\n` | «El torneo requiere exactamente 32 entrenadores para armar los grupos (actualmente 0).» | Ídem | ✅ PASS |
| 3 clasificación armada | `7\n12\n` | «Torneo armado: 8 grupos de 4 entrenadores (combates 1-48).» + `[GRUPO A]`…`[GRUPO H]` | Ídem | ✅ PASS |
| 4 filas ordenadas | `7\n12\n` | `1;1;Ash;0;0;0;0;0` … `4;4;Gary;0;0;0;0;0` (puntos en cero) | Ídem | ✅ PASS |
| 5 calendario 1–48 | `8\n2\n12\n` | `1;A;1;Ash;2;Misty`, `6;A;3;Brock;4;Gary`, `7;B;5;Jessie;6;James`, `43;H;29;Mallow;30;Lana`, `48;H;31;Sophocles;32;Gladion` | Ídem | ✅ PASS |
| 6 combate amistoso migrado | `8\n1\n1\n1\n2\n1\n1\n1\n1\n1\n12\n` | «Resultado: gana Misty (id 2).» (RF-CMB intacto en opción 8→1) | Ídem | ✅ PASS |
| 7 EOF en submenú | `8\n` | Cierre ordenado, exit 0 (no cuelga, timeout 5 s) | Ídem | ✅ PASS |
| 8 sha256 datos | batería completa | `pokedex.txt`, `efectividad.txt`, `entrenadores.txt` byte-idénticos | Ídem | ✅ PASS |
| 9 columnas | `grep -RInE '.{100,}'` | 0 líneas > 99 en torneo.c/h y main.c | Ídem | ✅ PASS |
| P1–P104 probe F5 | probe contra `src/{torneo,entrenador,equipo,pokedex,tipos,combate,archivos}.c` | Ver desglose abajo | Ídem | ✅ PASS (re-ejecutado byte a byte) |
| X1–X23 probe extra F5 | probe extra contra `src/{torneo,entrenador}.c` | Ver desglose abajo | Ídem | ✅ PASS |
| awk calendario | salida real `8\n2\n12\n` + awk | 48 combates, 6/grupo, 48 pares únicos, 3 apariciones por entrenador | `OK: 48 combates, 6 por grupo (A-H), 48 pares unicos, cada entrenador juega 3` | ✅ PASS |

### Matriz de Cumplimiento de Specs (F5)

| Requisito | Escenario | Evidencia | Resultado |
|---|---|---|---|
| RF-TRN-01 | Estructura del torneo | Probe P2 + batería 5: fase de grupos con combates 1–48 (calendario completo) y eliminatoria 49–64 reservada en el arreglo de 64 combates | ✅ COMPLIANT |
| RF-TRN-02 | 32 entrenadores exactos | Probe P1: 31 rechazado (D10), 32 aceptado; probe extra X1/X2: 30 rechazado y 33 inalcanzable por registro (RF-ENT-01); batería 2: rechazo por menú con conteo actual | ✅ COMPLIANT |
| RF-TRN-02 | Round-robin de un grupo | Probe P2: 6 combates por grupo, 6 pares distintos, numeración A=1..6…H=43..48, pares (1,2)(1,3)(1,4)(2,3)(2,4)(3,4) | ✅ COMPLIANT |
| RF-TRN-03 | Asignación de puntos | Probe P3: V1 ⇒ 3/0 con victoria/derrota; empate ⇒ 1/1; KOs acumulados en `pokemon_derrotados` | ✅ COMPLIANT |
| RF-TRN-04 | Desempate por victorias | Probe P5-D: 1 (3 pts, 1V) antes que 2 (3 pts, 0V); criterio 2 implementado como red de seguridad (en round-robin completo a igual puntuación corresponde igual cantidad de victorias, nota documentada) | ✅ COMPLIANT |
| RF-TRN-04 | Desempate por Pokémon derrotados | Probe P5-A: 2 (3 pts, 1V, 4 KOs) antes que 1 (3 pts, 1V, 2 KOs) | ✅ COMPLIANT |
| RF-TRN-04 | Desempate por criterio adicional | Probe P5-B (D9 directo: 3 gana a 4 y va primero con empate total) y P5-C (directo empatado ⇒ id menor: 1 antes que 2) | ✅ COMPLIANT |
| RF-TRN-05 | Tabla de posiciones del grupo | Batería 3/4 + probe P7: `[GRUPO X]` con 4 filas ordenadas por la cadena RF-TRN-04 | ✅ COMPLIANT |
| RF-TRN-06 | Clasificación sin intervención manual | Probe P6/P7: `id_clasificados[16]` = 1A,2A,1B,2B,…,1H,2H rellenado automáticamente al completar 48/48 | ✅ COMPLIANT |
| RF-CLS-01 | Tabla ordenada | Batería 3/4 + probe P7: filas con posición, id, nombre, victorias, empates, derrotas, puntos (y derrotados en pantalla) | ✅ COMPLIANT (escenario «Tabla ordenada») |
| RF-CLS-01 | Salida a archivo | `data/clasificacion.txt` se genera en F7 (`archivos_guardar_clasificacion`) | ⚠️ DIFERIDO a F7 por plan (tasks.md F7.2) |
| RF-RES-03 | Participantes resueltos por el sistema | Probe P4: resultado con participantes ≠ los del calendario ⇒ rechazo; calendario determinista 1–48 | ✅ COMPLIANT |

**Resumen de cumplimiento F5**: 11/12 escenarios del alcance F5 completos; el escenario «Salida a archivo» de RF-CLS-01 se difiere a F7 (tasks.md F7.2), igual que la transición GRUPOS→ELIMINATORIAS (F6). RF-TRN-01..06 quedan verificados por completo.

#### Veredicto por requisito (2026-09-21)

| Requisito | Veredicto | Evidencia (1 línea) |
|---|---|---|
| RF-TRN-01 | ✅ COMPLIANT | Probe P2: 48 combates de grupos + 16 reservados; estructura de dos fases visible en `Torneo` |
| RF-TRN-02 | ✅ COMPLIANT | Probe P1/P2: rechazo 31, aceptación 32, round-robin 6 pares distintos por grupo sin repetidos (verificado par a par) |
| RF-TRN-03 | ✅ COMPLIANT | Probe P3: ganador 3 pts/1V/2 KOs, perdedor 0 pts/1D/1 KO, empate 1 pt/1E a ambos |
| RF-TRN-04 | ✅ COMPLIANT | Probe P5-A/B/C/D: los 5 criterios en orden (puntos, victorias, derrotados, directo D9, id) con escenarios que resuelven por criterios distintos |
| RF-TRN-05 | ✅ COMPLIANT | Batería 3/4: 8 grupos con 4 posiciones ordenadas por la cadena completa |
| RF-TRN-06 | ✅ COMPLIANT | Probe P6/P7: clasificados automáticos 1A..2H al completar 48/48, sin entrada del usuario |
| RF-CLS-01 | ⚠️ PARCIAL | «Tabla ordenada» ✓ (pantalla, probe/batería); «Salida a archivo» diferido a F7 (planificado) |

#### Revisión de bordes (F5)

| Borde | Evidencia | Resultado |
|---|---|---|
| ≠32 entrenadores | Probe P1 (31 rechazado, estado intacto) + probe extra X1 (30 rechazado, estado intacto) + X3/X4 (33 inalcanzable: el registro RF-ENT-01 rechaza al 33.º) + batería 2 (menú con conteo 0) | ✅ PASS |
| Combate fuera de grupos (49–64) | Probe P4: número 49 rechazado con mensaje (F6 lo habilita) | ✅ PASS |
| Resultado duplicado | Probe P4: combate 1 re-aplicado ⇒ rechazo | ✅ PASS |
| Participantes no resueltos por el sistema | Probe P4: ids inventados ⇒ rechazo (RF-RES-03) | ✅ PASS |
| Empate con ganador / V1 incoherente / KOs negativos / RES_PENDIENTE | Probe P4: los cuatro rechazados sin mutar el estado | ✅ PASS |
| Consultas sin torneo armado | Probe P8: «El torneo no está armado.» y el estado no muta | ✅ PASS |
| EOF en submenú | Batería 7 + timeout 5 s: cierre ordenado sin colgar | ✅ PASS |
| Inmutabilidad de datos | sha256 de los 3 `.txt` idéntico tras la batería (RF-PDX-04) | ✅ PASS |

### Correctness (Evidencia estática, F5)

| Requisito | Estado | Notas |
|---|---|---|
| RF-TRN-02 | ✅ Implementado | `torneo_armar_grupos`: distribución por orden de registro (posición k ⇒ grupo k/4, puesto k%4, D10); pares fijos `PARES[6][2]` y número `6·grupo+par+1`; inicializa los 64 combates |
| RF-TRN-03 | ✅ Implementado | `torneo_aplicar_resultado`: 3 victoria / 1 empate / 0 derrota sobre los `Entrenador` del registro; acumula `pokemon_derrotados` desde `kos1/kos2` |
| RF-TRN-04 | ✅ Implementado | `comparar_clasificacion`: cadena completa puntos→victorias→derrotados→directo (D9)→id; `combate_directo` localiza el enfrentamiento dentro de los combates 1–48 |
| RF-TRN-05/06 | ✅ Implementado | `clasificar_grupos` + `torneo_clasificados`/`torneo_ordenar_grupos`: rellenan `id_clasificados[16]` con 1A,2A,…,1H,2H |
| RF-CLS-01 | ✅ Implementado (pantalla) | `torneo_mostrar_clasificacion`: `[GRUPO X]` + 4 filas `POS;ID;NOMBRE;V;E;D;PUNTOS;DERROTADOS` |
| RF-RES-03 | ✅ Implementado | `torneo_aplicar_resultado` exige participantes idénticos a los del calendario; los ids los resuelve el sistema, nunca el usuario |
| D9/D10 | ✅ Implementado | D9: enfrentamiento directo + id menor en la cadena; D10: `torneo_armar_grupos` rechaza cualquier conteo ≠ 32 |

### Coherencia con el Diseño (F5)

| Decisión del diseño | ¿Cumplida? | Notas |
|---|---|---|
| §6.1 distribución determinista (posición k ⇒ grupo k/4, puesto k%4) | ✅ Sí | Registro 1..32 ⇒ grupo A = ids 1..4, …, grupo H = ids 29..32 (verificado en batería) |
| §6.1 pares fijos (m0,m1)(m0,m2)(m0,m3)(m1,m2)(m1,m3)(m2,m3) y número 6·grupo+par+1 | ✅ Sí | Calendario byte-idéntico al diseño: A=1..6, B=7..12, …, H=43..48 |
| §6.1 cadena de desempate RF-TRN-04 (puntos→victorias→derrotados→directo→id) | ✅ Sí | Implementada en `comparar_clasificacion`; escenarios por criterio en el probe P5 |
| §6.1 `id_clasificados[16]` en orden 1A,2A,1B,2B,…,1H,2H | ✅ Sí | Probe P6: 1A=1, 2A=2, 1B=5, 2B=6, …, 1H=29, 2H=30 |
| §1.3 firmas `torneo_armar_grupos`, `torneo_aplicar_resultado`, `torneo_ordenar_grupos` | ✅ Sí | Idénticas a las del diseño; se añaden `torneo_clasificados`, `torneo_mostrar_clasificacion`, `torneo_mostrar_enfrentamientos` (RF-CLS-01 las requiere) |
| §2.2 KOs persistidos en `Combate` (criterio 3) | ✅ Sí | `kos1/kos2` en el combate y acumulados en `Entrenador.pokemon_derrotados` |
| §6.1 transición GRUPOS→ELIMINATORIAS al completar 48/48 | ⚠️ Diferida a F6 | En F5 se ordena y se definen los clasificados al completar 48/48; el cambio de `estado` a `TORNEO_ELIMINATORIAS` y el bracket 49–64 se implementan en F6 (tasks.md F6.1) |

### Hallazgos (F5)

**CRITICAL**: Ninguno.

**WARNING**: Ninguno.

**LOW**:
1. **Migración del combate amistoso (opción 8)**: la opción 8 pasa de abrir directamente el combate amistoso de F3 a un submenú «1. Combate amistoso / 2. Calendario del torneo / 0. Volver». El motor de combate (`combate.c`) no cambió y la batería F3 de combate sigue siendo válida; los casos de menú de la batería F3 que usaban «8\n1» ahora requieren «8\n1\n1» (documentado en tasks.md F3.2/F5.2). Es el cambio de menú solicitado por F5.2, no una regresión.
2. **Criterio 2 (victorias) como red de seguridad**: en un round-robin completo de 4 con puntuación 3/1/0, a igual puntuación corresponde siempre igual cantidad de victorias (aritmética de la puntuación), por lo que el desempate por victorias no puede dispararse en un grupo terminado; se implementa por fidelidad a RF-TRN-04 y se verifica con un estado sintético (probe P5-D). No afecta la corrección: es un criterio más en la cadena.

**SUGGESTION**:
1. La transición a `TORNEO_ELIMINATORIAS` y la resolución de participantes 49–64 (bracket) quedan pendientes de F6; `torneo_aplicar_resultado` ya rechaza números > 48 con mensaje claro, así que F6 solo debe habilitar el rango.
2. `torneo_mostrar_clasificacion` muestra también `derrotados` en pantalla (8.ª columna) mientras que el formato de archivo de §5.4 (`clasificacion.txt`) no lo incluye; F7 debe respetar el formato del archivo.

### Veredicto F5

**PASS**
El lote F5 cumple RF-TRN-01..06 y el escenario «Tabla ordenada» de RF-CLS-01: las 3 tareas F5 están completas, el build es limpio (cero warnings, ocho `.c`, hash `72726e48…` reproducido), la batería scriptada pasa 17/17 y el probe 104/104 **re-ejecutados byte a byte en la verificación formal** (hashes `fd70d362…` y `65825f0f…` idénticos a los del apply), más el probe extra 23/23 (D10 con rechazo 30 y 33 inalcanzable por RF-ENT-01, 8 grupos de 4 con 3 combates por entrenador, 48 combates 1..48) y la verificación awk independiente del calendario (48 combates, 6 por grupo A–H, 48 pares únicos sin repetidos, cada entrenador juega 3). La evidencia cubre: calendario round-robin sin pares repetidos con numeración A=1..6…H=43..48, puntuación 3/1/0 exacta, validaciones de `aplicar_resultado` incluyendo RF-RES-03, desempates por derrotados/directo (D9)/id y el criterio de victorias como red de seguridad, clasificados 1A..2H automáticos al completar 48/48, combate amistoso de F3 accesible en opción 8→1 y datos inmutables (sha256 estable). Sin CRITICAL ni WARNING; dos LOW informativos no bloquean. Presupuesto del lote: 570 líneas de código nuevas (≤ 800 ✓).

---

## Sección F6 (Día 6, 2026-09-21) — Eliminatorias (bracket 49–64)

**Fase**: F6 — `src/torneo.c` (bracket 49–64, `ORIGEN_PARTICIPANTE`, `torneo_participantes`, transiciones GRUPOS→ELIMINATORIAS→FINALIZADO, posiciones finales) + cableado de las opciones 10 y 11 del menú
**Fecha**: 2026-09-21

### Completeness

| Métrica | Valor |
|---|---|
| Tareas del lote F6 | 3 |
| Tareas completadas | 3 |
| Tareas incompletas | 0 |

### Build y Ejecución

**Build**: ✅ Pasó (exit 0, cero warnings)
```text
$ make clean && make
rm -rf build
mkdir -p build
gcc -std=c99 -Wall -Wextra -o build/torneo src/archivos.c src/combate.c src/entrenador.c src/equipo.c src/main.c src/pokedex.c src/tipos.c src/torneo.c
```
Evidencia: `build_exit_code=0`, cero warnings con `-Wall -Wextra` sobre los **ocho** `.c` del proyecto. Hash de la salida del build: `72726e48…` (la línea de compilación no cambió respecto a F5; el binario sí: `binary_hash sha256:35cf1f40…`). 0 líneas de más de 99 columnas en `src/torneo.c`/`src/torneo.h`/`src/main.c`. Presupuesto del lote: **394 líneas de código nuevas** (torneo.c +340/−12, torneo.h +30/−2, main.c +10) ≤ 800 ✓.

**Pruebas**: ✅ Batería F6 scriptada (13 casos stdin→grep) + probe F6 (43 comprobaciones) + regresión probe F5 (104/104) y batería F5 (17/17): **177/177 PASS, exit 0**. Hash del output de pruebas: `3869041b…`. `evidence_revision sha256:9619bfb5…`.

**Re-verificación formal (intento F6-verificacion, 2026-09-21)**: la batería (13/13), el probe F6 (43/43) y la regresión F5 (probe 104/104 + batería 17/17) se re-ejecutaron íntegramente con los scripts persistentes de `/tmp/opencode` (`bateria_f6.sh`, `probe_f6.c`, `probe_f5.c`, `bateria_f5.sh`) sobre el árbol limpio en HEAD `275a768`. Resultados **byte-idénticos a los del apply**: `bateria_f6.out` y `probe_f6.out` reproducen los mismos sha256 (`3869041b…` y `9619bfb5…`), el log de build es idéntico (`72726e48…`) y el binario reproduce `binary_hash 35cf1f40…`. `test_output_hash` y `evidence_revision` del envelope se actualizan a los hashes reproducibles de esta ejecución (el valor previo del envelope no era reproducible desde los archivos guardados; el patrón documentado en F5 fija `evidence_revision` = sha256 del output del probe).

| Caso | Entrada | Resultado esperado | Resultado real | Diff |
|---|---|---|---|---|
| 1 build | `make clean && make` | exit 0, cero warnings, ocho `.c` compilados | Ídem | ✅ PASS |
| 2 opción 10 sin armar | `10\n12\n` | «El torneo no está armado.» | Ídem | ✅ PASS |
| 3 opción 11 sin finalizar | `11\n12\n` | «El torneo aún no ha finalizado; aún no hay campeón.» | Ídem | ✅ PASS |
| 4 armar + opción 10 | `7\n10\n12\n` | Cabecera + `[Octavos]` + `49;1A;0;?;2B;0;?;0` + `57;G49;0;?;G50;0;?;0` + `63;P61;0;?;P62;0;?;0` + `64;G61;0;?;G62;0;?;0` | Ídem | ✅ PASS |
| 5 probe F6 | probe contra `src/{torneo,entrenador,equipo,pokedex,tipos,combate,archivos}.c` | 43 comprobaciones (ver desglose) | Ídem | ✅ PASS |
| 6 probe F6 display | salida del probe | Bracket final con ganadores: `49;1A;1;Entrenador 1;2B;6;Entrenador 6;1`, `63;P61;5;Entrenador 5;P62;21;Entrenador 21;5`, `64;G61;1;Entrenador 1;G62;17;Entrenador 17;1` | Ídem | ✅ PASS |
| 7 posiciones finales | salida del probe | `1. Campeón: Entrenador 1 (id 1)`, `2. Subcampeón: Entrenador 17 (id 17)`, `3. Tercer lugar: Entrenador 5 (id 5)`, `4. Cuarto lugar: Entrenador 21 (id 21)`, «El campeón del torneo es Entrenador 1 (id 1).» | Ídem | ✅ PASS |
| 8 regresión probe F5 | probe F5 recompilado | `104/104 comprobaciones PASS` | Ídem | ✅ PASS |
| 9 regresión batería F5 | `bateria_f5.sh` | `BATERÍA F5: 17 PASS, 0 FALLA` | Ídem | ✅ PASS |
| 10 sha256 datos | batería completa | `pokedex.txt`, `efectividad.txt`, `entrenadores.txt` byte-idénticos | Ídem | ✅ PASS |
| 11 columnas | `grep -RInE '.{100,}'` | 0 líneas > 99 en torneo.c/h y main.c | Ídem | ✅ PASS |

### Matriz de Cumplimiento de Specs (F6)

| Requisito | Escenario | Evidencia | Resultado |
|---|---|---|---|
| RF-ELM-01 | Sin empates en eliminatoria | Probe P6: empate en 57 rechazado con mensaje «La eliminatoria no admite empates (RF-ELM-01)»; torneo jugado 49–64 sin empates (probe completo) | ✅ COMPLIANT |
| RF-ELM-01 | Perdedores de semifinal al tercer lugar | Probe P8: `torneo_participantes(63)` = P61 (5) vs P62 (21), ambos perdedores de las semifinales 61/62 | ✅ COMPLIANT |
| RF-ELM-02 | Emparejamiento fijo de octavos | Probe P1: 49=(1,6) 1A-2B, 50=(9,14) 1C-2D, 51=(17,22) 1E-2F, 52=(25,30) 1G-2H, 53=(5,2) 1B-2A, 54=(13,10) 1D-2C, 55=(21,18) 1F-2E, 56=(29,26) 1H-2G — exactamente la tabla §6.2 | ✅ COMPLIANT |
| RF-ELM-03 | Coherencia entre octavos y cuartos | Probe P8: 57=G49-G50 (1,9), 58=G53-G54 (5,13), 59=G51-G52 (17,25), 60=G55-G56 (21,29); el ganador de 49 (1) participa en 57 | ✅ COMPLIANT |
| RF-ELM-04 | Perdedores identificados | Probe P8/P13: P61 = perdedor de 61, P62 = perdedor de 62; identificados automáticamente al aplicar las semifinales | ✅ COMPLIANT |
| RF-ELM-05 | Coronación del campeón | Probe P10: campeón = G64 = 1, subcampeón = P64 = 17, tercero = G63 = 5, cuarto = P63 = 21; el sistema muestra los 4 primeros al finalizar (probe display) | ✅ COMPLIANT |
| RF-RES-02 | Eliminado no reaparece | Probe P9: el perdedor de 49 (id 6) intenta participar en 58 ⇒ rechazo (participantes ≠ resueltos) | ✅ COMPLIANT |
| RF-RES-02 | Empate en eliminatoria rechazado | Probe P6: `RES_EMPATE` en 57 ⇒ rechazo (RF-ELM-01) | ✅ COMPLIANT |
| RF-RES-03 | Participante manual rechazado | Probe P5: participantes inventados (1,10) en 57 ⇒ rechazo; idem P4 (fuentes sin resolver ⇒ «no está disponible aún») y P3 (49 con grupos incompletos) | ✅ COMPLIANT (completo: grupos F5 + eliminatoria F6) |
| RF-MEN-01 | Opciones 10 y 11 | Batería 2/3/4: menú muestra las 12 opciones; opciones 10 y 11 cableadas y delegando en `torneo` | ✅ COMPLIANT |

**Resumen de cumplimiento F6**: 10/10 escenarios del alcance F6 completos (RF-ELM-01..05 íntegros, RF-RES-02 en sus escenarios de eliminatoria, RF-RES-03 completo con ambas fases, RF-MEN-01 con las opciones 10/11). Quedan pendientes de F7 los escenarios de RF-RES-02 «Entrenador inexistente» y la carga de resultados por teclado/archivo (RF-RES-01/04).

#### Veredicto por requisito (2026-09-21)

| Requisito | Veredicto | Evidencia (1 línea) |
|---|---|---|
| RF-ELM-01 | ✅ COMPLIANT | Probe P6 + torneo completo: sin empates en 49–64; P61/P62 al tercer lugar (P8) |
| RF-ELM-02 | ✅ COMPLIANT | Probe P1: los 8 cruces de octavos byte-idénticos a la tabla §6.2 |
| RF-ELM-03 | ✅ COMPLIANT | Probe P8: 57–60 = G#-G# encadenados a los ganadores de octavos |
| RF-ELM-04 | ✅ COMPLIANT | Probe P8/P13: perdedores de 61/62 resueltos como P61/P62 |
| RF-ELM-05 | ✅ COMPLIANT | Probe P10 + display: campeón G64, subcampeón P64, tercero G63, cuarto P63 |
| RF-RES-02 | ⚠️ PARCIAL | «Eliminado no reaparece» y «Empate en eliminatoria» ✓ (F6); «Entrenador inexistente» diferido a F7 (resultados por archivo) |
| RF-RES-03 | ✅ COMPLIANT | Probe P4/P5 + F5 P4: participantes siempre resueltos por el sistema, rechazo de manuales en 1–48 y 49–64 |
| RF-MEN-01 | ✅ COMPLIANT | Opciones 10/11 cableadas; menú de 12 opciones intacto |

#### Revisión de bordes (F6)

| Borde | Evidencia | Resultado |
|---|---|---|
| Aplicar 49 con grupos incompletos | Probe P3: rechazo sin mutar el estado (sigue GRUPOS) | ✅ PASS |
| Combate con fuentes sin resolver (57 antes de 49/50) | Probe P4: ids 0 + rechazo «no está disponible aún» | ✅ PASS |
| Participantes manuales (RF-RES-03) | Probe P5: (1,10) en 57 ⇒ rechazo | ✅ PASS |
| Empate en eliminatoria | Probe P6: RES_EMPATE en 57 ⇒ rechazo RF-ELM-01 | ✅ PASS |
| V1 incoherente (ganador = id2) | Probe P7: rechazo sin aplicar | ✅ PASS |
| Eliminado que reaparece | Probe P9: perdedor de 49 en 58 ⇒ rechazo (RF-RES-02) | ✅ PASS |
| Resultados tras finalizar | Probe P11: aplicar 1 o 63 después del 64 ⇒ rechazo «ya finalizó» | ✅ PASS |
| V2 en eliminatoria | Probe P13: gana el 2B (6) ⇒ G49 = 6 y encadena a 57 (6,9) | ✅ PASS |
| Contadores de grupos intactos | Probe: Entrenador 1 conserva 3 victorias/9 puntos tras el torneo completo (RF-TRN-03 solo 1–48) | ✅ PASS |
| Transición 48/48 y 64 | Probe P2: GRUPOS→ELIMINATORIAS al completar 48/48; →FINALIZADO al aplicar 64 | ✅ PASS |

### Correctness (Evidencia estática, F6)

| Requisito | Estado | Notas |
|---|---|---|
| RF-ELM-01..05 | ✅ Implementado | `ORIGEN_PARTICIPANTE[64][2]` (tabla §6.2 exacta), `torneo_participantes` (resolución contra el estado real), `resolver_origen` (CLASIFICADO/GANADOR/PERDEDOR) |
| RF-RES-03 | ✅ Implementado | `torneo_aplicar_resultado` valida participantes == resueltos en 49–64 y persiste los ids resueltos en el combate (el perdedor de una fuente P# los lee) |
| Transiciones §6.1 | ✅ Implementado | 48/48 ⇒ ordenar + clasificados + resolver octavos + `TORNEO_ELIMINATORIAS`; 64 ⇒ `TORNEO_FINALIZADO` + `mostrar_posiciones` (RF-ELM-05) |
| RF-ELM-05 | ✅ Implementado | `mostrar_posiciones`: campeón = G64, subcampeón = P64, tercero = G63, cuarto = P63; públicas `torneo_mostrar_resultados`, `torneo_mostrar_campeon`, `torneo_mostrar_posiciones_finales` |
| RF-MEN-01 | ✅ Implementado | Opciones 10/11 en `main` delegando en el módulo torneo (RF-TEC-02: sin lógica en main) |

### Coherencia con el Diseño (F6)

| Decisión del diseño | ¿Cumplida? | Notas |
|---|---|---|
| §6.2 tabla de mapeo 49–64 exacta (49=1A-2B … 64=G61-G62) | ✅ Sí | `ORIGEN_PARTICIPANTE` byte-idéntica a la tabla; probe P1/P8 verifica los 16 cruces |
| §6.1 transición GRUPOS→ELIMINATORIAS al completar 48/48 | ✅ Sí | Ordena, define clasificados, resuelve y persiste los octavos 49–56 y cambia el estado |
| §6.1 transición ELIMINATORIAS→FINALIZADO al aplicar 64 | ✅ Sí | Estado final + posiciones mostradas (RF-ELM-05) |
| §6.1 «combate no disponible aún» cuando una fuente no existe | ✅ Sí | `resolver_origen` devuelve 0; mensaje explícito en `aplicar_resultado` |
| §1.3 `torneo_participantes(const Torneo *, int, int *, int *)` | ✅ Sí | Firma idéntica al diseño; ids 0 señalan fuente sin resolver |
| §1.3 `torneo_mostrar_posiciones_finales` | ⚠️ Menor | Se añade el parámetro `const RegistroEntrenadores *` para mostrar nombres (patrón del resto de `torneo_mostrar_*`; el diseño §1.3 solo recibía el torneo) |
| Anti-empate D5 en eliminatoria | ✅ Sí | Ya existía en `combate.c` (`es_eliminatoria`); `torneo_aplicar_resultado` nunca recibe un empate de eliminatoria (RF-ELM-01) |

### Hallazgos (F6)

**CRITICAL**: Ninguno.

**WARNING**: Ninguno.

**LOW**:
1. **Firma de `torneo_mostrar_posiciones_finales`**: el diseño §1.3 la declaraba solo con `const Torneo *`; se añadió `const RegistroEntrenadores *` para imprimir nombres, siguiendo el patrón de `torneo_mostrar_clasificacion`/`torneo_mostrar_enfrentamientos` (F5). No cambia contratos existentes (la función no se usaba).
2. **`binary_hash` nuevo campo del envelope**: el hash del log de build es idéntico al de F5 (la línea de compilación no cambió), por lo que se añade `binary_hash` (sha256 del binario `build/torneo`, `35cf1f40…`) como evidencia de que el artefacto compilado sí cambió.

**SUGGESTION**:
1. La opción 6 «Cargar resultados» (teclado/archivo) se completa en F7 con `resultados.c` + `archivos.c`; `torneo_aplicar_resultado` ya está listo para recibir resultados de eliminatoria en cualquier orden con coherencia de rondas.
2. F7 debe verificar el escenario «Entrenador inexistente» de RF-RES-02 (vía `resultados_validar` con `entrenador_buscar`), único escenario de ese requisito no cubierto por F6.

### Veredicto F6

**PASS**
El lote F6 cumple RF-ELM-01..05, RF-RES-03 (completo) y los escenarios de eliminatoria de RF-RES-02, con el cableado de las opciones 10 y 11 del menú: las 3 tareas F6 están completas, el build es limpio (cero warnings, ocho `.c`, `binary_hash 35cf1f40…`), la batería scriptada pasa 13/13, el probe F6 pasa 43/43 (bracket fijo 49–56 idéntico a la tabla §6.2, transiciones GRUPOS→ELIMINATORIAS→FINALIZADO, encadenado G#/P# incluyendo P61/P62 al tercer lugar, empate rechazado, V1/V2 coherentes, eliminado que no reaparece, participantes manuales rechazados, fuente sin resolver rechazada, campeón = G64 con las 4 posiciones correctas, contadores de grupos intactos y rechazo de resultados tras finalizar) y la regresión F5 completa pasa (probe 104/104 + batería 17/17). Sin CRITICAL ni WARNING; dos LOW informativos no bloquean. Presupuesto del lote: 394 líneas de código nuevas (≤ 800 ✓).

---

## Sección F7 (Día 7, 2026-09-21) — Archivos y resultados

**Fase**: F7 — `src/resultados.c` (carga teclado/archivo + `resultados_validar` con el catálogo §8.1), `src/archivos.c` (guardar/cargar entrenadores, cargar/guardar resultados, guardar clasificación §5.4), `data/resultados.txt` parcial (20/48) y cableado de la opción 6 + guardado de clasificación y entrenadores al salir en `main.c`
**Fecha**: 2026-09-21

### Completeness

| Métrica | Valor |
|---|---|
| Tareas del lote F7 | 4 |
| Tareas completadas | 4 |
| Tareas incompletas | 0 |

### Build y Ejecución

**Build**: ✅ Pasó (exit 0, cero warnings)
```text
$ make clean && make
rm -rf build
mkdir -p build
gcc -std=c99 -Wall -Wextra -o build/torneo src/archivos.c src/combate.c src/entrenador.c src/equipo.c src/main.c src/pokedex.c src/resultados.c src/tipos.c src/torneo.c
```
Evidencia: `build_exit_code=0`, cero warnings con `-Wall -Wextra` sobre los **nueve** `.c` del proyecto (se añade `resultados.c`). `build_output_hash ef1f166a…` (la línea de compilación cambió respecto a F6) y `binary_hash d8f9ef0b…`. 0 líneas de más de 99 columnas en `src/resultados.c/h`, `src/archivos.c/h`, `src/torneo.c/h` y `src/main.c`. Presupuesto del lote: **797 líneas cambiadas en `src/`** (783 adiciones + 14 borrados, patrón F6; sin contar `data/resultados.txt`, que es dato, no código) ≤ 800 ✓.

**Pruebas**: ✅ Batería F7 scriptada (13 casos stdin→grep) + probe F7 (39 comprobaciones) + regresión completa (probe F6 43/43, batería F6 13/13, probe F5 104/104, batería F5 17/17): **216/216 PASS, exit 0**. Hash del output de pruebas: `81be07dc…`. `evidence_revision sha256:835580d3…` (= sha256 del output del probe, patrón F5/F6).

| Caso | Entrada | Resultado esperado | Resultado real | Diff |
|---|---|---|---|---|
| 1 build | `make clean && make` | exit 0, cero warnings, nueve `.c` compilados | Ídem | ✅ PASS |
| 2 submenú opción 6 | `6\n0\n12\n` | «--- Cargar resultados ---» + «1. Por teclado» + «2. Desde archivo (data/resultados.txt)» | Ídem | ✅ PASS |
| 3 carga parcial | `6\n2\n12\n` | «Resultados cargados: 20 aplicados, 0 rechazadas.» + «Combates pendientes: 28 de 48 (fase de grupos), 16 de 16 (eliminatoria).» | Ídem | ✅ PASS |
| 4 clasificación + guardado | `6\n2\n7\n12\n` | `[GRUPO A]` + `1;2;Misty;2;0;1;6;6` + «Clasificación guardada en data/clasificacion.txt.» | Ídem | ✅ PASS |
| 5 teclado | `6\n1\n5\n1\n3\n1\n0\n12\n` | «Combate 5: Misty (id 2) vs Gary (id 4)» + «Resultado del combate 5 (grupo A) aplicado: victoria.» + «Resultados cargados por teclado: 1 aplicados.» | Ídem | ✅ PASS |
| 6 probe F7 | probe contra los 9 `.c` | 39 comprobaciones (ver desglose) | Ídem | ✅ PASS |
| 7 regresión batería F6 | `bateria_f6.sh` | `BATERÍA F6: 13 PASS, 0 FALLA` | Ídem | ✅ PASS |
| 8 regresión batería F5 | `bateria_f5.sh` | `BATERÍA F5: 17 PASS, 0 FALLA` | Ídem | ✅ PASS |
| 9 sha256 datos | batería completa | `pokedex.txt`, `efectividad.txt`, `entrenadores.txt` y `resultados.txt` (e08bdc4e…) byte-idénticos | Ídem | ✅ PASS |
| 10 columnas | `grep -RInE '.{100,}'` | 0 líneas > 99 en el lote F7 | Ídem | ✅ PASS |
| 11 formato resultados.txt | `awk -F';'` | 20 líneas, 7 campos por línea (§5.3) | Ídem | ✅ PASS |

### Matriz de Cumplimiento de Specs (F7)

| Requisito | Escenario | Evidencia | Resultado |
|---|---|---|---|
| RF-RES-01 | Resultado parcial | Batería 3 + probe P3: 20/48 aplicados, torneo sigue en GRUPOS, pendientes 28+16; faltantes ingresables por teclado (batería 5) | ✅ COMPLIANT |
| RF-RES-02 | Entrenador inexistente | Probe P5: combate 21 con entrenador 99 ⇒ rechazo «El entrenador 99 del combate 21 no está registrado.» (programa sigue vivo) | ✅ COMPLIANT |
| RF-RES-02 | Eliminado no reaparece | Probe P7: el perdedor de 49 (id 6) en 58 ⇒ rechazo «no coinciden con los resueltos» | ✅ COMPLIANT (completo: F6 + F7) |
| RF-RES-02 | Empate en eliminatoria rechazado | Probe P6: `RES_EMPATE` en 57 ⇒ rechazo «La eliminatoria no admite empates (RF-ELM-01).» | ✅ COMPLIANT (completo) |
| RF-RES-03 | Participante manual rechazado | Probe P8: participantes (1,17) en 57 ⇒ rechazo; el usuario solo declara resultado/KOs en teclado (batería 5: el sistema muestra «Combate 5: Misty vs Gary») | ✅ COMPLIANT (completo) |
| RF-RES-04 | Carga desde archivo | Probe P3: 20 registros con esquema §5.3 (5 o 7 campos) aplicados; guardado recargable y byte-idéntico (P9) | ✅ COMPLIANT |
| RF-ENT-03 | Guardado y recarga de entrenadores | Probe P2: guardar → recargar → equipos idénticos (id, especie, apodo, nivel y stats D2 recalculadas); contador global sin colisión (id 178) | ✅ COMPLIANT |
| RF-CLS-01 | Salida a archivo | Probe P10 + batería 4: `data/clasificacion.txt` con cabecera `[GRUPO A]`, 7 campos por fila (sin derrotados), fila `1;2;Misty;2;0;1;6` (formato §5.4 EXACTO) | ✅ COMPLIANT (completo: F5 + F7) |
| RF-CLS-01 | Tabla ordenada | Probe P4: clasificación parcial coherente A=2,1,4,3, B=5,7,6,8, C=9,11,12,10, D=15,13,14,16 (RF-TRN-04 completa) | ✅ COMPLIANT (completo) |
| RF-MEN-01 | Opciones 6 y 7 | Batería 2-5: opción 6 con submenú teclado/archivo + pendientes; opción 7 muestra y guarda la clasificación | ✅ COMPLIANT |

**Resumen de cumplimiento F7**: 10/10 escenarios del alcance F7 completos. RF-RES-01, RF-RES-04 y RF-ENT-03 quedan íntegros; RF-RES-02 se completa (faltaba «Entrenador inexistente»); RF-CLS-01 se completa con la «Salida a archivo»; RF-RES-03 y RF-MEN-01 refuerzan su cobertura con la opción 6.

#### Veredicto por requisito (2026-09-21)

| Requisito | Veredicto | Evidencia (1 línea) |
|---|---|---|
| RF-RES-01 | ✅ COMPLIANT | Batería 3/5 + probe P3: parciales 20/48, pendientes y faltantes por teclado |
| RF-RES-02 | ✅ COMPLIANT | Probe P5 (inexistente) + P7 (eliminado) + P6 (empate): catálogo §8.1 completo |
| RF-RES-03 | ✅ COMPLIANT | Probe P8 + teclado: participantes siempre resueltos por el sistema |
| RF-RES-04 | ✅ COMPLIANT | Probe P3/P9: esquema §5.3 con 5 o 7 campos, recargable y byte-idéntico |
| RF-ENT-03 | ✅ COMPLIANT | Probe P2: guardar/recargar equipos idénticos (stats D2 recalculadas) |
| RF-CLS-01 | ✅ COMPLIANT | Probe P4/P10: tabla ordenada + salida a archivo con formato §5.4 exacto |
| RF-MEN-01 | ✅ COMPLIANT | Opciones 6 y 7 cableadas (submenú, pendientes, guardado) |

#### Revisión de bordes (F7)

| Borde | Evidencia | Resultado |
|---|---|---|
| Parciales 20/48 | Probe P3: 20 aplicados, 28+16 pendientes, torneo en GRUPOS | ✅ PASS |
| Entrenador inexistente | Probe P5: rechazo con mensaje específico, programa continúa | ✅ PASS |
| Duplicado | Probe P12: combate 1 con resultado ⇒ rechazo «ya tiene resultado» | ✅ PASS |
| Ronda incorrecta (49 con grupos incompletos) | Probe P13: rechazo «fase de grupos no está completa» | ✅ PASS |
| Empate en eliminatoria | Probe P6: RES_EMPATE en 57 ⇒ rechazo RF-ELM-01 | ✅ PASS |
| Eliminado que reaparece | Probe P7: perdedor de 49 en 58 ⇒ rechazo (participantes ≠ resueltos) | ✅ PASS |
| Participante manual arbitrario | Probe P8: (1,17) en 57 ⇒ rechazo RF-RES-03 | ✅ PASS |
| V1 coherente con resueltos | Probe P8: sí se acepta con los participantes resueltos | ✅ PASS |
| KOs opcionales (5 campos) | Probe P3/P9: líneas con 5 campos ⇒ KOs 0; con 7 ⇒ leídos; 6 ⇒ rechazo | ✅ PASS |
| Guardar/recargar entrenadores | Probe P2: stats D2 idénticas por id; contador global sin colisión | ✅ PASS |
| Round-trip resultados | Probe P9: archivo guardado byte-idéntico y recargable con el mismo estado | ✅ PASS |
| Formato §5.4 exacto | Probe P10: 8 cabeceras [GRUPO X], 7 campos por fila, sin derrotados | ✅ PASS |
| datos/resultados.txt estable | sha256 e08bdc4e… tras cargar y salir (no se autoguarda) | ✅ PASS |

### Correctness (Evidencia estática, F7)

| Requisito | Estado | Notas |
|---|---|---|
| RF-RES-02 | ✅ Implementado | `resultados_validar` con el catálogo §8.1 completo y mensaje específico por caso; teclado y archivo la usan antes de delegar en `torneo_aplicar_resultado` |
| RF-RES-03 | ✅ Implementado | El teclado muestra los participantes resueltos y no los pide; el archivo exige ids == resueltos |
| RF-RES-01/04 | ✅ Implementado | Parciales permitidos; `resultados_mostrar_pendientes`; archivo con 5 o 7 campos |
| RF-ENT-03 | ✅ Implementado | `archivos_guardar_entrenadores` (formato §5.2); determinismo D2 por id al recargar |
| RF-CLS-01 | ✅ Implementado | `archivos_guardar_clasificacion` con formato §5.4 exacto (sin derrotados) vía `torneo_clasificacion_grupo` (orden compartido con la pantalla) |
| RF-MEN-01 | ✅ Implementado | Opción 6 (submenú teclado/archivo + pendientes), opción 7 guarda; entrenadores se guardan al salir si cambiaron |

### Coherencia con el Diseño (F7)

| Decisión del diseño | ¿Cumplida? | Notas |
|---|---|---|
| §5.2 entrenadores.txt entrada/salida | ✅ Sí | Cargador F2 + `archivos_guardar_entrenadores`; stats no persistidas, re-derivadas con D2 (probe P2) |
| §5.3 resultados.txt con parciales y KOs opcionales | ✅ Sí | Esquema NUM;ID1;ID2;RESULTADO;GANADOR[;KOS1;KOS2]; «-» para empates; recargable |
| §5.4 clasificacion.txt EXACTO sin derrotados | ✅ Sí | Cabecera + 7 campos por fila; la pantalla sí muestra derrotados (8 campos) |
| §1.3 `archivos_guardar_clasificacion(const Torneo *, const char *)` | ⚠️ Menor | Se añade `const RegistroEntrenadores *` para resolver los nombres (el diseño no lo incluía); patrón de la desviación de F6 en `torneo_mostrar_posiciones_finales` |
| §1.2 `resultados` delega la aplicación en `torneo` | ✅ Sí | `resultados_validar` valida; `torneo_aplicar_resultado` aplica (validación idempotente) |
| §1.2 `resultados` depende de `validacion` | ⚠️ Diferido | Los lectores triestado locales replican el patrón de main.c; F8 los unificará en `validacion.c` (RF-TEC-03) |
| §8.1 «Resultado compatible con rondas previas» | ✅ Sí | Ronda/estado + fuentes G#/P# sin resolver ⇒ «no está disponible aún» (P13/P7) |

### Hallazgos (F7)

**CRITICAL**: Ninguno.

**WARNING**: Ninguno.

**LOW**:
1. **Firma de `archivos_guardar_clasificacion`**: el diseño §1.3 la declaraba sin el registro; se añadió `const RegistroEntrenadores *` porque el formato §5.4 incluye NOMBRE y el torneo solo guarda ids. No cambia contratos previos (no se usaba).
2. **`torneo_clasificacion_grupo` nueva pública**: se añade a `torneo.h` para que el archivo y la pantalla compartan la ordenación exacta de RF-TRN-04 (evita duplicar el comparador en `archivos.c`).
3. **Resultados NO autoguardados al salir**: `archivos_guardar_resultados` existe y se verifica por probe (round-trip byte-idéntico), pero el menú no la invoca automáticamente: evita que una sesión de teclado pise `data/resultados.txt` (el ejemplo debe permanecer estable para las baterías). El enunciado solo pide autoguardar entrenadores y clasificación.

**SUGGESTION**:
1. F8 (validación integral) puede unificar los lectores triestado de `main.c` y `resultados.c` en `validacion.c` (RF-TEC-03).
2. F9 puede cablear opcionalmente el guardado explícito de resultados (p. ej. dentro de la opción 6) si se desea persistir teclado sin reescribir el ejemplo.

### Veredicto F7

**PASS**
El lote F7 cumple RF-RES-01, RF-RES-02 (catálogo §8.1 completo: parciales, entrenador inexistente, duplicado, ronda incorrecta, empate en eliminatoria, eliminado que no reaparece, participante manual), RF-RES-03 (completo con teclado), RF-RES-04, RF-ENT-03 (guardar/recargar con stats D2 idénticas) y RF-CLS-01 (salida a archivo con formato §5.4 exacto), con el cableado de la opción 6 y el guardado de clasificación/entrenadores: las 4 tareas F7 están completas, el build es limpio (cero warnings, nueve `.c`, `binary_hash d8f9ef0b…`), la batería F7 pasa 13/13, el probe F7 pasa 39/39 y la regresión completa F5/F6 pasa (probe F6 43/43 + batería F6 13/13 + probe F5 104/104 + batería F5 17/17). Sin CRITICAL ni WARNING; tres LOW informativos no bloquean. Presupuesto del lote: 797 líneas cambiadas en `src/` (≤ 800 ✓); `data/resultados.txt` (20 líneas, dato) no cuenta como código.

---

## Sección F8 (Día 7, 2026-09-21) — Validación integral de entradas

**Fase**: F8 — `src/validacion.c` (lectura segura con reintentos y EOF), migración de todas las lecturas de `main.c` y `resultados.c` a la API común, cierre de hallazgos F1 (submenú Pokédex) y F2 (parsers `;;`), batería E2E de rutas inválidas
**Fecha**: 2026-09-21
**Intento**: F8-verificacion (adquirido por el orquestador; no se re-adquirió en esta ejecución)

### Completeness

| Métrica | Valor |
|---|---|
| Tareas del lote F8 | 3 |
| Tareas completadas | 3 |
| Tareas incompletas | 0 |

### Build y Ejecución

**Build**: ✅ Pasó (exit 0, cero warnings)
```text
$ make clean && make
rm -rf build
mkdir -p build
gcc -std=c99 -Wall -Wextra -o build/torneo src/archivos.c src/combate.c src/entrenador.c src/equipo.c src/main.c src/pokedex.c src/resultados.c src/tipos.c src/torneo.c src/validacion.c
```
Evidencia: `build_exit_code=0`, cero warnings con `-Wall -Wextra` sobre los **diez** `.c` del proyecto (se incorpora `src/validacion.c`). `build_output_hash sha256:22c555a6…` (la línea de compilación cambió respecto a F7) y `binary_hash sha256:2d63a679…` (build/torneo). 0 líneas de más de 99 columnas en `src/validacion.c/h`, `src/main.c`, `src/resultados.c`, `src/pokedex.c`, `src/tipos.c` y `src/archivos.c` (verificado en la batería F8). Presupuesto del lote: **~830 líneas cambiadas en `src/`** vs 800 del forecast (desviación LOW C documentada: la migración atómica de TODAS las lecturas es indivisible; sin `validacion.c` el código no compila).

**Pruebas**: ✅ Batería F8 scriptada (15 bloques E2E) + probe F7 (39/39) + probe F3 (21/21): **75/75 PASS, exit 0**. Hash del output de pruebas: `58f78689…`. `evidence_revision sha256:835580d3…` (= sha256 del output del probe F7 re-ejecutado, **idéntico al del envelope F7**: reproducibilidad byte a byte de la evidencia del catálogo §8.1).

| Caso | Entrada | Resultado esperado | Resultado real | Diff |
|---|---|---|---|---|
| 1 build | `make clean && make` | exit 0, cero warnings, diez `.c` compilados | Ídem | ✅ PASS |
| 2 opción 99 | `99\n12\n` | «Opción inválida. Intente de nuevo.» + «Saliendo del programa.» (sigue vivo) | Ídem | ✅ PASS |
| 3 texto en menú | `abc\n12\n` | «Entrada inválida.» + sigue vivo | Ídem | ✅ PASS |
| 4 nivel 101/0 | `2\n1\nAsh\n3\n1\n1\n1\n101\n0\n50\n\n0\n12\n` | «Nivel inválido (rango 1-100).» ×2 + reintento + ejemplar válido agregado (rc 0) | Ídem | ✅ PASS |
| 5 id duplicado | `2\n99\nNuevo\n2\n99\nOtro\n12\n` | «Entrenador registrado: id 99, Nuevo.» + «Ya existe un entrenador con id 99; el registro se rechaza.» + sigue vivo | Ídem | ✅ PASS |
| 6 Pokédex ausente | renombrar + `1\n12\n` | «Aviso: la Pokédex no está disponible» + «La Pokédex no está cargada.» (submenú completo) | Ídem | ✅ PASS |
| 7 EOF en restricciones | `3\n2\n1\n` + timeout 5 | Cierre ordenado sin colgar | Ídem | ✅ PASS |
| 8 EOF en selección | `8\n1\n1\n2\n1\n` + timeout 5 | «Combate cancelado.», cierre ordenado | Ídem | ✅ PASS |
| 9 EOF en teclado | `6\n1\n5\n` + timeout 5 | «Resultados cargados por teclado: 0 aplicados.» | Ídem | ✅ PASS |
| 10 regresión batería F7 | `bateria_f7.sh` | `BATERÍA F7: 13 PASS, 0 FALLA` (incluye probe F7 39/39) | Ídem | ✅ PASS |
| 11 regresión batería F4 | `bateria_f4.sh` | `Bateria F4: 12 PASS, 0 FAIL` | Ídem | ✅ PASS |
| 12 regresión probe F3 | probe F3 recompilado con validacion.c | Motor de combate: 21/21, 0 fallos | Ídem | ✅ PASS |
| 13 sha256 datos | batería completa | `pokedex.txt` 9696e9b2…, `efectividad.txt` 505e726b…, `entrenadores.txt` a3c49a4f…, `resultados.txt` e08bdc4e… | Ídem | ✅ PASS |
| 14 columnas | `grep -RInE '.{100,}'` | 0 líneas > 99 en el lote F8 | Ídem | ✅ PASS |
| 15 validacion.c sin exit | `grep -nE 'exit\s*\('` | 0 coincidencias (garantía §8.1) | Ídem | ✅ PASS |

**Verificaciones adicionales de la verificación formal F8 (2026-09-21)**:

| Verificación | Evidencia | Resultado |
|---|---|---|
| Ninguna función de validación llama a `exit` | `grep -rn 'exit(' src/` → 0 coincidencias en **todo** `src/` (no solo validacion.c); `main.c` tampoco | ✅ PASS |
| Submenú Pokédex avisa con `cantidad == 0` en TODAS las ramas | Pokédex ausente + ramas: 1 (mostrar todas), 2 (buscar número), 3 (buscar nombre), 4 (ficha) → «La Pokédex no está cargada.» en las 4 | ✅ PASS (cierra WARNING F1) |
| Parsers rechazan campos vacíos `;;` | pokedex línea `003;;Venusaur;…` → «se esperaban 8 campos y se encontraron 9» + carga rechazada (cantidad=0); entrenadores `33;Test;;1;1;X;10` → «Línea 33 rechazada: campos inválidos» + `32 aceptadas, 1 rechazada`; resultados `21;1;2;;1;1;1` → «Línea 21 rechazada: campos inválidos o resultado desconocido.» + `20 aplicados, 1 rechazadas`; efectividad 1.ª línea corrupta → «tiene 6 valores (se esperaban 18); se usara la tabla por defecto» — programa vivo en los 4 | ✅ PASS (cierra SUGGESTION F2) |
| EOF a media entrada → sale 0, nunca cuelga | `3|2|1`, `8|1|1|2|1`, `6|1|5`, `2`, `1`, `12` → exit 0 en todos (timeout 5 s) | ✅ PASS |
| Probe F7 explícito | `probe_f7_verify` → 39/39 comprobaciones, salida byte-idéntica al envelope F7 | ✅ PASS |
| Probe F3 explícito | `probe_f3_f8` → 21 comprobaciones, 0 FAIL | ✅ PASS |
| Pokémon inexistente (RF-TEC-03 escenario) | nombre `zzzz` → «No se encontró la especie solicitada.» + programa continúa; número 999 → rechazado por rango en la lectura (1–150) con reintento | ✅ PASS |

### Matriz de Cumplimiento de Specs (F8)

| Requisito | Escenario | Evidencia | Resultado |
|---|---|---|---|
| RF-TEC-03 | Opción de menú inválida no rompe el programa | Caso 2: opción 99 → error + repregunta + sigue vivo (exit 0 al salir con 12) | ✅ COMPLIANT |
| RF-TEC-03 | Pokémon inexistente | Búsqueda de `zzzz` en la Pokédex → «No se encontró la especie solicitada.» + el programa continúa; número fuera de rango rechazado por `validar_leer_entero(1,150)` | ✅ COMPLIANT |
| RF-TEC-03 | Entrenador duplicado | Caso 5: id 99 ya registrado → rechazo con mensaje y el programa continúa | ✅ COMPLIANT |
| RF-TEC-03 | Nivel inválido | Caso 4: niveles 101 y 0 → «Nivel inválido (rango 1-100).» ×2 + se solicita un nivel válido (50 aceptado) | ✅ COMPLIANT |
| RF-TEC-03 | Archivo inexistente | Caso 6 + verificación adicional: pokedex.txt ausente → aviso con ruta al iniciar y submenú completo avisa «La Pokédex no está cargada.»; el programa continúa | ✅ COMPLIANT |
| RF-TEC-03 | Entrada inválida no termina el programa | Casos 2–9 + EOF×3: opción 99, texto, niveles 101/0, id duplicado, archivo ausente y EOF a mitad de flujo → mensaje de error y permanece en ejecución o cierre ordenado exit 0; nunca termina inesperadamente | ✅ COMPLIANT |

**Resumen de cumplimiento F8**: 6/6 escenarios de RF-TEC-03 completos con evidencia E2E re-ejecutada (batería F8 15/15). RF-TEC-03 queda **verificado por completo con la batería integral de rutas inválidas** — era la evidencia formal pendiente que el veredicto global F7 anticipaba para F8. El requisito ya constaba como COMPLIANT en los 40/42 del envelope (sus escenarios se cubrieron incrementalmente en F0–F7: opción 99 en F0, Pokémon inexistente en F1, duplicado/nivel en F2, archivo ausente en F1/F7); la batería F8 lo consolida como evidencia E2E autónoma. Los únicos requirements pendientes del cambio siguen siendo DOC-03 (D1 → F10.1) y DOC-04 (ortografía del Doxyfile → F10.3) → **40/42 requirements y 68/70 scenarios** se mantienen.

### Correctness (Evidencia estática, F8)

| Requisito | Estado | Notas |
|---|---|---|
| RF-TEC-03 | ✅ Implementado | `validar_leer_entero`/`validar_leer_entero_msg`: prompt + reintentos indefinidos; no numérico ⇒ «Entrada inválida.»; fuera de rango ⇒ mensaje genérico o específico; EOF ⇒ 0 (el llamador cierra ordenado). `validar_leer_cadena`: bool (false ante EOF), nunca desborda (recorta a n-1 y descarta el resto de la línea). `validar_separar_campos`: división por `;` SIN omitir campos vacíos (los `;;` producen campos vacíos detectables, no desplazamiento silencioso) |
| §8.1 garantía estructural | ✅ Implementado | Ninguna función de validación llama a `exit` (grep global `src/` vacío); todos los bucles reintentan hasta entrada válida o EOF (EOF ⇒ cierre ordenado decidido por el llamador, con `feof(stdin)` donde 0 es legítimo) |
| RF-TEC-02 | ✅ Implementado | `main.c` elimina los lectores locales (`leer_opcion`/`leer_linea`/`descartar_linea`/`leer_entero`) y migra TODAS sus lecturas a `validar_*`; `resultados.c` migra teclado y archivo; el menú sigue sin lógica de negocio |
| Cierre WARNING F1 | ✅ Implementado | Submenú de la opción 1 chequea `cantidad == 0` al entrar y avisa «La Pokédex no está cargada.» en TODAS sus ramas (mostrar todas, buscar número, buscar nombre, ficha) |
| Cierre SUGGESTION F2 | ✅ Implementado | Los 4 parsers de datos (pokedex, entrenadores, resultados, efectividad) detectan campos vacíos `;;` vía `validar_separar_campos` en lugar del desplazamiento de `strtok` (verificado E2E) |

### Coherencia con el Diseño (F8)

| Decisión del diseño | ¿Cumplida? | Notas |
|---|---|---|
| §1.3 firma `validar_leer_entero(const char *, int, int)` | ✅ Sí | Idéntica a la del diseño |
| §1.3 firma `validar_leer_cadena(const char *, char *, size_t)` | ⚠️ Extendida | El diseño la declaraba `void`; la implementación devuelve `bool` (false ante EOF) — **LOW A** documentado: el contrato «retorna estado» (el llamador distingue EOF) exige el retorno; no rompe llamadas previas (F8 introduce el módulo) |
| §8.1 catálogo por módulo | ✅ Sí | Opción inexistente (validacion 1..12), Pokémon inexistente (pokedex NULL + capa de menú), id duplicado (entrenador_registrar), nivel 0/>MAX (validacion NIVEL_MIN..NIVEL_MAX), equipo excede tamaño (equipo_agregar_ejemplar), tipo inválido (tipos_es_valido), archivo inexistente/malformado (pokedex_cargar/archivos_*, mensaje con ruta y línea), combate inexistente, entrenador inexistente, participantes ≠ resueltos, empate en eliminatoria, resultado duplicado (resultados_validar), 32 exactos (torneo_armar_grupos) — todos sin `exit` |
| §8.1 «EOF ⇒ cierre ordenado» | ✅ Sí | Batería F8 casos 7–9 + verificación adicional: EOF en restricciones/selección/teclado cierra ordenado con exit 0 y timeout 5 s (no cuelga) |
| §8.2 patrón de verificación scriptada | ✅ Sí | Batería F8 = casos stdin→grep (patrón del proyecto); determinismo intacto (sin `srand` añadido) |
| §1.2 `main` sin lógica de negocio | ✅ Sí | La migración refuerza la separación: toda lectura va por `validacion`, `main` solo despacha |

### Hallazgos (F8)

**CRITICAL**: Ninguno.

**WARNING**: Ninguno. (Se cierran el WARNING F1 — submenú Pokédex avisa en todas las ramas — y la SUGGESTION F2 — parsers detectan `;;` — con evidencia E2E.)

**LOW**:
1. **LOW A — firma de `validar_leer_cadena`**: el diseño §1.3 la declaraba `void`; la implementación devuelve `bool` (false ante EOF). Exigido por el contrato «retorna estado» (el llamador debe distinguir EOF de una línea leída). Desviación documentada en tasks.md F8.1; no rompe contratos previos.
2. **LOW B — extensiones de F8 no previstas en §1.3**: `validar_leer_entero_msg` (mensaje específico por prompt, conserva los textos que la batería F4 verifica) y `validar_separar_campos` (base del cierre de SUGGESTION F2). Documentadas en tasks.md F8.1.
3. **LOW C — presupuesto del lote ~830 vs 800**: la migración atómica de TODAS las lecturas supera el forecast de tasks.md (≈280 solo para `validacion.c`); la migración es indivisible (sin `validacion.c` el código no compila), por lo que no se fragmentó en PRs encadenados; riesgos de review asumidos y documentados en apply-progress F8.
4. **NOTA — `bateria_f3.sh` obsoleta desde F5**: sus casos de menú usan la opción 8 directa de F3; el submenú llegó en F5.2. Verificado que **ya fallaba 12/12 sobre el código F7 previo a F8** (git stash) → NO es una regresión de F8. El motor de combate se regresa con el probe F3 (21/21, verde en esta verificación). La batería F3 de menú necesita actualizar su navegación (8→1→N) o declararse obsoleta; el probe F3 es la evidencia válida del motor.

**SUGGESTION**:
1. La migración de lecturas a `validacion.c` deja `descartar_linea` como helper estático interno; las demás capas ya no duplican lecturas. El siguiente paso natural es F9 (batería completa `tests/run_tests.sh` + casos versionados).
2. El comportamiento del número de especie 999 cambió respecto a F1 (antes «No se encontró la especie solicitada.»; ahora rechazado por rango 1..150 en la lectura con reintento). Es una mejora de RF-TEC-03 (validación en origen), el escenario del spec cubre el nombre inexistente (verificado con `zzzz`); no requiere acción.

### Veredicto F8

**PASS**
El lote F8 cumple RF-TEC-03 por completo: las 3 tareas F8 están completas, el build es limpio (cero warnings, diez `.c`, `binary_hash 2d63a679…`), la batería E2E de rutas inválidas pasa 15/15 (opción 99, texto en menú, niveles 101/0 con reintento, id duplicado, Pokédex ausente con submenú completo, EOF×3 con timeout), ninguna función de validación llama a `exit` (grep global `src/` vacío), el submenú Pokédex avisa en todas las ramas con `cantidad == 0` (cierra WARNING F1), los 4 parsers rechazan campos vacíos `;;` (cierra SUGGESTION F2), el EOF a media entrada cierra ordenado con exit 0 y la regresión completa F3–F7 está verde (probe F7 39/39 — byte-idéntico al envelope F7 —, batería F7 13/13, batería F4 12/12, probe F3 21/21). Sin CRITICAL ni WARNING; tres LOW y una NOTA informativos no bloquean. `bateria_f3.sh` queda documentada como obsoleta desde F5 (no es regresión de F8; el motor se regresa con el probe F3).

---

## Sección F9 (Día 8) — Pruebas completas versionadas

### Completeness

| Tarea | Estado | Evidencia |
|---|---|---|
| F9.1 `tests/run_tests.sh` versionado | ✅ | Ejecutor en el repo: gate de build + 20 casos de menú (stdin→diff byte a byte) + 2 probes C + resumen PASS/FAIL con exit code |
| F9.2 `tests/casos/*` cubriendo RF-PRB-01 | ✅ | 20 casos `.in`/`.esperado` UTF-8 + `tests/probes/probe_efectividad.c` + `tests/probes/probe_torneo_completo.c` |
| F9.3 Integración 32→48→clasificación→49–64→campeón | ✅ | `probe_torneo_completo` 26/26 (el menú no permite 64 combates E2E; documentado en F9.3) |
| F9.4 Gate: batería verde, cero warnings | ✅ | `bash tests/run_tests.sh` → **23 PASS, 0 FALLA**, exit 0; `make clean && make` cero warnings |

### Evidencia ejecutada (verificación formal, 2026-09-21)

| Suite | Comando | Resultado |
|---|---|---|
| Build | `make clean && make` | ✅ exit 0, cero warnings, `build_output_hash 22c555a6…` (byte-idéntico al de F8) |
| Gate versionado | `bash tests/run_tests.sh` | ✅ **23 PASS / 0 FALLA**, exit 0, `test_output_hash da4b87e6…` |
| Probe integración | `tests/probes/probe_torneo_completo` | ✅ 26/26, `evidence_revision bd3e914e…` |
| Probe efectividad | `tests/probes/probe_efectividad` | ✅ 21/21 |
| Datos | sha256 de los 5 `data/*.txt` | ✅ intactos antes/después (sandbox aislado por caso) |

### Cobertura de la lista mínima del PDF §6

**16/16 puntos sin huecos**: Pokédex (carga, búsqueda existente/inexistente), registro de entrenadores, creación de ejemplares, formación y validación de equipos, combates (victoria/empate/anti-empate), efectividad, resultados válidos/inválidos, clasificación, avance a eliminatoria, eliminación y campeón (probe 26/26), backtracking factible/sin solución.

### Hallazgos (F9)

**CRITICAL**: Ninguno. **WARNING**: Ninguno.
**SUGGESTION**:
1. Los multiplicadores distintos de ×0 se ejercitan en el probe de efectividad, no en un caso de menú (cobertura suficiente, documentado).
2. Los puntos 13–15 de la lista del PDF (avance/eliminación/campeón) viven en el probe de integración porque el menú no permite completar 64 combates E2E (documentado en F9.3).

### Veredicto F9

**PASS**
El lote F9 cumple RF-PRB-01: la batería completa está versionada en el repositorio (`tests/`), el gate es verde (23 PASS / 0 FALLA, exit 0), el build no tiene warnings y es byte-idéntico al de F8 (reproducibilidad), la integración completa del torneo (32→48→clasificación→49–64→campeón) pasa 26/26, la cobertura de la lista mínima del PDF §6 es total (16/16) y los datos quedan intactos tras la ejecución. Sin CRITICAL ni WARNING; dos SUGGESTION documentadas.

---

## Veredicto global (F0 + F1 + F2 + F3 + F4 + F5 + F6 + F7 + F8 + F9)

**PASS WITH WARNINGS por lote; FAIL del envelope por evidencia incompleta del cambio en curso**
Los lotes F0, F1, F2, F3, F4, F5, F6, F7, F8 y F9 pasan (F3–F9 sin warnings: 0 CRITICAL, 0 blockers; F8 cerró además el WARNING F1 y la SUGGESTION F2 acumulados). El envelope declara `verdict: fail` porque el cambio completo aún no está verificado: quedan F10–F11 (documentación final y entrega) y los parciales DOC-03 de F0 (D1 → F10.1) y DOC-04 (ortografía del Doxyfile → F10.3). Conteos autoritativos contra los 11 specs del cambio (42 requirements / 70 scenarios): **40/42 requirements y 68/70 scenarios verificados** (RF-MEN-01, RF-TEC-02, RF-TEC-03, RF-PDX-01..06, DOC-01/02, RF-ENT-01..03, RF-EQP-01..05, RF-CMB-01..05, RF-TRN-01..06, RF-ELM-01..05, RF-RES-01..04 y RF-CLS-01 completos; DOC-03/DOC-04 parciales). RF-TEC-03 queda verificado por completo con la batería E2E F8 (15/15) y RF-PRB-01 con el gate versionado F9 (23 PASS / 0 FALLA) en las verificaciones formales (2026-09-21). Siguiente lote: F10 (documentación final: informe técnico D1–D10, planificación 14/14, render Doxygen).
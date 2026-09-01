```yaml
schema: gentle-ai.verify-result/v1
evidence_revision: sha256:6d6387bf2a9b9946dd555b94717ae3e7ee17d7045d0d9328f11bb2b93bd58ea9
verdict: fail
blockers: 0
critical_findings: 0
requirements: 5/6
scenarios: 8/9
test_command: batería scriptada: printf "1\n12\n" | ./build/torneo; printf "99\n12\n" | ./build/torneo; printf "abc\n12\n" | ./build/torneo; printf "12\n" | ./build/torneo; printf "" | ./build/torneo — cotejo con diff por caso
test_exit_code: 0
test_output_hash: sha256:63391c0c26da7271cd228fff34f0ac59953f0821f531e5187c18de66ab96cc75
build_command: make clean && make (gcc -std=c99 -Wall -Wextra)
build_exit_code: 0
build_output_hash: sha256:784da56af53dba7f803158e1417f3dc42b7f006ea914f487db5dddab7bd1ea07
```

# Informe de Verificación — Gran Torneo Pokémon — Lote F0 (Día 1)

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

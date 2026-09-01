# Propuesta: Gran Torneo Pokémon

**Change**: `gran-torneo-pokemon`
**Fase**: sdd-propose
**Fecha**: 2026-09-01
**Idioma del artefacto**: español (requisito explícito del proyecto)

---

## Intención (Problema y objetivo)

Construir desde cero, en C (ISO C99), un sistema de consola que administra íntegramente un torneo Pokémon de la Primera Generación: carga de las 150 especies, registro de 32 entrenadores, creación de ejemplares y equipos mediante recursividad + backtracking, combates con fórmula de daño y efectividad de tipos, fase de grupos round-robin con clasificación y fase eliminatoria (combates 49–64) hasta coronar al campeón. Es el proyecto final de Programación I / Algoritmos I: su objetivo es demostrar dominio de estructuras, TDA, archivos, cadenas, recursividad/backtracking y diseño modular, con una implementación limpia y defendible en la evaluación oral.

## Alcance

### In Scope

**9 dominios funcionales** (→ capacidades, ver sección Capabilities):

1. **Pokédex** — 150 especies, 18 tipos, consulta sin mutación (RF-PDX).
2. **Entrenadores** — registro, id único, persistencia (RF-ENT).
3. **Equipos y ejemplares** — ejemplares desde especies + backtracking de formación (RF-EQP).
4. **Combates** — reglamento, fórmula de daño, efectividad de tipos (RF-CMB).
5. **Torneo (grupos + clasificación)** — round-robin, puntuación, clasificación (RF-TRN).
6. **Eliminatorias** — bracket 49–64, tercer lugar, final, campeón (RF-ELM).
7. **Resultados y validación** — carga y consistencia entre rondas (RF-RES).
8. **Clasificación / archivo** — tabla de posiciones y salida a archivo (RF-CLS).
9. **Menú principal** — 12 opciones y flujo general (RF-MEN).

**Entregables transversales explícitos (Día 1)**:

- **Convención de documentación Doxygen**: `Doxyfile` + guía de convenciones en `docs/` (bloques `@file`, `@brief`, `@param`, `@return`, `@author` en español).
- **Documento de control de planificación** (`docs/planificacion.md`): seguimiento de lo hecho y lo pendiente, alineado al plan de 14 días.
- **Informe técnico explicativo** (`docs/informe-tecnico.md`): documento que se arma INCREMENTALMENTE durante el desarrollo, con diagramas Mermaid de los algoritmos y del funcionamiento.

**Transversal técnico**: validación de entradas del usuario (RF-TEC-03), modularidad con `main` limitado a flujo y menú (RF-TEC-02), y los 4 archivos de texto `data/pokedex.txt`, `data/entrenadores.txt`, `data/resultados.txt`, `data/clasificacion.txt`.

### Out of Scope

- Interfaz gráfica (solo consola stdin/stdout).
- Multijugador en red.
- Simular la mecánica completa de los juegos oficiales (movimientos, PP, IVs/EVs, críticos complejos, precisión, estados avanzados).
- Bibliotecas externas o dependencias no entregadas.

## Capabilities

> Contrato entre propuesta y specs. Cada capacidad nueva → `openspec/specs/<name>/spec.md`.

### New Capabilities

- `pokedex` — carga y consulta de las 150 especies y catálogo de 18 tipos (RF-PDX-01..06).
- `entrenadores` — registro, id único, persistencia y consulta (RF-ENT-01..03).
- `equipos` — ejemplares derivados de especie + backtracking de construcción de equipos (RF-EQP-01..05).
- `combate` — reglamento de combate, fórmula de daño y efectividad de tipos (RF-CMB-01..05).
- `torneo` — fase de grupos round-robin, puntuación y clasificación (RF-TRN-01..06).
- `eliminatorias` — bracket 49–64, tercer lugar, final y campeón (RF-ELM-01..05).
- `resultados` — carga y validación de resultados con consistencia entre rondas (RF-RES-01..04).
- `clasificacion` — tabla de posiciones y archivo de clasificación (RF-CLS-01).
- `menu` — menú principal y flujo general (RF-MEN-01).
- `validacion` — validación de entradas del usuario sin terminar el programa (RF-TEC-03).
- `documentacion` — Doxyfile + convenciones, `docs/planificacion.md` e `docs/informe-tecnico.md`.

### Modified Capabilities

Ninguna — proyecto greenfield, no existe spec previa (`openspec/specs/` vacío).

## Enfoque por fases incrementales

Orden por dependencias, alineado al plan de 14 días (2026-09-01 → 2026-09-14, ~2–3 h/día). Cada fase compila limpio (`gcc -std=c99 -Wall -Wextra`, cero warnings) y es verificable de forma aislada.

| # | Fase | Depende de | Días |
|---|------|-----------|------|
| F0 | **Convenciones + entorno + esqueleto** (Día 1): Doxyfile + guía, `docs/planificacion.md`, `docs/informe-tecnico.md` inicial, estructura de módulos y `main` con menú compilable | — | 1 |
| F1 | Pokédex (carga de 150 especies + consulta) | F0 | 1–2 |
| F2 | Tipos y efectividad (tabla 18×18) | F1 | 2–3 |
| F3 | Entrenadores y ejemplares (separación especie/ejemplar) | F1 | 3–4 |
| F4 | Combate (fórmula de daño D1, desempate D3) | F2, F3 | 4–6 |
| F5 | Backtracking de formación de equipos | F3 | 6–7 |
| F6 | Torneo: grupos + clasificación (32 entrenadores, 8 grupos) | F3, F5 | 7–9 |
| F7 | Eliminatorias (bracket 49–64) | F6 | 9–10 |
| F8 | Archivos y resultados (carga/validación) | F4, F7 | 10–12 |
| F9 | Validación integral de entradas | F1–F8 | 12–13 |
| F10 | Pruebas (batería scriptada completa) | F1–F9 | 13 |
| F11 | Entrega (empaquetado + documentación final) | F10 | 14 |

**Día 1 (hoy)**: F0 completo — convención Doxygen, planificación, informe técnico inicial, estructura de módulos y esqueleto compilable.

## Estrategia de verificación (sin framework)

- **Compilación estricta por fase**: `gcc -std=c99 -Wall -Wextra -o build/torneo <src.c>` con cero warnings como gate de cada fase.
- **Batería de entradas scriptadas + `diff`** de stdout contra salidas esperadas, cubriendo los casos mínimos del punto 6 del PDF: carga de Pokédex, búsquedas existentes/inexistentes, registro de entrenadores, creación de ejemplares, formación y validación de equipos, combates, efectividad de tipos, resultados válidos/inválidos, clasificación, avance a eliminatoria, eliminación de entrenadores, campeón y funcionamiento del backtracking.
- **Determinismo**: semilla fija (`srand(seed)`) y desempates deterministas (D3, D5) para que `diff` sea reproducible.
- Cada script de entrada/salida esperada se versiona en `tests/` y se documenta en `docs/planificacion.md`.

## Riesgos

| Riesgo | Probabilidad | Impacto | Mitigación |
|--------|--------------|---------|------------|
| Timeline 2 semanas con alcance amplio (64 combates, 9 dominios) | Alta | Alto | Orden por dependencias, tareas diarias en sdd-tasks, seguimiento en `planificacion.md` |
| Backtracking mal acotado no converge (criterio de evaluación) | Media | Alto | Podas por nivel total máximo y tamaño de equipo (D6/D7); probar con 150 especies |
| Separación especie/ejemplar rota (criterio central, sección 5) | Media | Alto | Estructuras `Especie` vs `Ejemplar` independientes; nunca escribir sobre `pokedex.txt` |
| Validación de resultados con huecos de estado entre rondas | Media | Alto | Modelo de estado del torneo centralizado; participantes resueltos por el sistema, no por el usuario |
| Sin valgrind/framework: memoria dinámica y UB no detectados | Media | Medio | C99 conservador, arreglos estáticos donde baste, liberación disciplinada, revisión manual de `free` |

## Plan de rollback (aislamiento por fase)

Cada fase es incremental y se aísla para poder revertir sin afectar el resto:

- **F0 (convenciones/esqueleto)**: solo se añaden archivos de docs y estructura; revertir = borrar los archivos nuevos, sin código afectado.
- **F1–F5 (dominios base)**: cada módulo se desarrolla tras su interfaz `.h`; una fase rota se desactiva desde `main` (opción de menú) sin romper el resto.
- **F6–F8 (torneo/eliminatorias/resultados)**: dependen de las fases previas ya validadas; se avanza solo si la fase anterior pasa su batería `diff`. Revertir una fase = `git revert` del commit de esa fase (trabajo por unidades de commit, una por fase).
- **Rollback global**: el repo es git; el avance es por fases commiteadas, por lo que cualquier regresión se deshace con `git revert` del commit correspondiente. El `Doxyfile`, `docs/` y `data/` (solo lectura excepto salidas) quedan fuera de la lógica de combate para minimizar el blast radius.
- **Criterio de aborto**: si al día 12 la validación integral (F9) no pasa, se congela la funcionalidad F0–F8 ya verde y se entrega con el alcance verificado, documentando en `informe-tecnico.md` lo pendiente.

## Dependencies

- Ninguna dependencia externa. Solo `gcc` y herramientas estándar (`make`/`diff`). Sin bibliotecas de terceros.

## Criterios de éxito (ligados a la sección 8 — evaluación)

- [ ] Compila limpio con `gcc -std=c99 -Wall -Wextra` y **cero warnings** en todas las fases.
- [ ] Separación **especie/ejemplar** correcta: un combate modifica solo el HP del ejemplar, nunca los datos de la especie (sección 5).
- [ ] **Backtracking funcional**: construye equipos cumpliendo restricciones y retrocede ante combinaciones inválidas (1.5).
- [ ] **Validación de resultados** robusta: entrenadores existentes, ronda correcta, clasificados correctos, eliminados no reaparecen, sin empates en eliminatoria (1.13).
- [ ] **Claridad modular**: `main` solo flujo y menú; lógica separada en módulos (pokedex, entrenador, equipo, combate, torneo, resultados, archivos, validación) (3).
- [ ] **Defensa entendible**: el `informe-tecnico.md` documenta en español cada decisión abierta (D1–D10) con su justificación y diagramas Mermaid, listo para la evaluación oral.
- [ ] Torneo completo: 32 entrenadores → 8 grupos → 16 clasificados → eliminatorias 49–64 → campeón, con la aritmética 1–64 verificada.

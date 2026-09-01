# Exploración: Gran Torneo Pokémon (sistema de administración de torneo en C99)

**Change**: `gran-torneo-pokemon`
**Fase**: sdd-explore
**Fecha**: 2026-09-01
**Fuente**: "Proyecto- 2026.pdf" (14 págs., Lic. Joshua Hernandez) — extraído a texto completo
**Idioma del artefacto**: español (requisito explícito del proyecto)

---

## Estado actual

- **No existe código fuente**. El repo contiene únicamente: `Proyecto- 2026.pdf`, scaffolding `openspec/` (config.yaml, specs/, changes/), `.atl/skill-registry.md`, `.gitignore` y `.git`.
- **Stack fijado en sdd-init**: C (ISO C99), `gcc -std=c99 -Wall -Wextra` con cero warnings, aplicación de consola (stdin/stdout), sin framework de tests (verificación manual con entrada scriptada y diff de stdout), sin valgrind.
- **Convenciones** (de `sdd-init/proyecto`): comentarios y bloques Doxygen en español, identificadores en inglés neutral, documentación del proyecto en español, commits en español (conventional commits con prefijo en inglés y descripción en español).

## Áreas afectadas (código futuro — nada existe aún)

- `src/main.c` — solo flujo general y menú (requisito 3).
- `src/pokedex.c/h` — carga y consulta de las 150 especies (1.1, 1.2, 1.14).
- `src/entrenador.c/h` — registro, persistencia y consulta de entrenadores (1.3, 1.15).
- `src/equipo.c/h` — creación de ejemplares a partir de especies + backtracking (1.4, 1.5).
- `src/combate.c/h` — reglamento de combate, fórmula de daño, efectividad de tipos (1.6, 1.7).
- `src/torneo.c/h` — grupos, puntuación, clasificación, bracket 49–64, campeón (1.8–1.11).
- `src/resultados.c/h` — carga y validación de resultados (1.12, 1.13, 1.16).
- `src/archivos.c/h` — lectura/escritura de los 4 archivos de texto (1.14–1.17).
- `src/validacion.c/h` — validación de entradas del usuario (sección 4).
- `data/pokedex.txt`, `data/entrenadores.txt`, `data/resultados.txt`, `data/clasificacion.txt`.

---

## 1. Inventario de requisitos funcionales (por dominio)

### Dominio: Pokédex (1.1, 1.2, 1.14)

| ID | Requisito | Ref. |
|----|-----------|------|
| RF-PDX-01 | La Pokédex se conforma exclusivamente por los 150 Pokémon de la Primera Generación (#001 Bulbasaur → #150 Mew). | 1.1 |
| RF-PDX-02 | Cada entrada contiene como mínimo: número, nombre, tipo primario, tipo secundario, HP base, ataque base, defensa base, velocidad base. | 1.1 |
| RF-PDX-03 | Catálogo de 18 tipos (Normal, Fuego, Agua, Planta, Eléctrico, Hielo, Lucha, Veneno, Tierra, Volador, Psíquico, Bicho, Roca, Fantasma, Dragón, Siniestro, Acero, Hada). Un Pokémon tiene 1 o 2 tipos; el segundo tipo se representa vacío/`-` cuando es único. | 1.1 |
| RF-PDX-04 | Los datos de la Pokédex son inmutables: un combate modifica solo el HP actual del ejemplar, nunca el HP base de la especie (ver RF-EQP-03 y sección 5). | 1.1, 5 |
| RF-PDX-05 | Opción de menú "Consultar Pokédex": mostrar la Pokédex, buscar por número, buscar por nombre, consultar información completa de una especie. La consulta no modifica datos. | 1.2 |
| RF-PDX-06 | El archivo de la Pokédex contiene las 150 especies y se lee al inicio; los 150 Pokémon NO se escriben dentro del código fuente. | 1.14 |

### Dominio: Entrenadores (1.3, 1.15)

| ID | Requisito | Ref. |
|----|-----------|------|
| RF-ENT-01 | Cada entrenador almacena como mínimo: identificador único, nombre, equipo Pokémon, victorias, empates, derrotas, puntuación. | 1.3 |
| RF-ENT-02 | Validación de identificador único: no pueden existir dos entrenadores con el mismo id. | 1.3, 4 |
| RF-ENT-03 | Archivo de entrenadores (opcional) que permita almacenar id, nombre e información suficiente para reconstruir el equipo. | 1.15 |

### Dominio: Equipos y ejemplares (1.4, 1.5)

| ID | Requisito | Ref. |
|----|-----------|------|
| RF-EQP-01 | Los Pokémon de los equipos pertenecen obligatoriamente a alguna de las 150 especies de la Pokédex. | 1.4 |
| RF-EQP-02 | Al seleccionar una especie, el sistema crea un ejemplar con: id único, especie, nombre, nivel, HP, ataque, defensa, velocidad, tipo(s), estado actual. | 1.4 |
| RF-EQP-03 | Las estadísticas del ejemplar se generan a partir de las características base de su especie según las reglas del proyecto; dos ejemplares de la misma especie pueden tener estadísticas diferentes. La separación especie/ejemplar es un concepto central (sección 5). | 1.4, 5 |
| RF-EQP-04 | Construcción automática de equipos con recursividad + backtracking: explora combinaciones hasta cumplir restricciones (cantidad determinada de Pokémon, nivel total máximo, cantidad mínima de tipos diferentes, estadísticas objetivo, restricciones sobre integrantes); retrocede cuando una combinación parcial no alcanza solución válida; muestra el equipo generado. | 1.5 |
| RF-EQP-05 | Validación de equipos: tamaño permitido, especies existentes, niveles válidos, tipos válidos. | 4 |

### Dominio: Combates (1.6, 1.7)

| ID | Requisito | Ref. |
|----|-----------|------|
| RF-CMB-01 | Cada combate enfrenta a dos entrenadores con los Pokémon de sus equipos; los Pokémon se enfrentan individualmente; al comenzar, cada entrenador selecciona un Pokémon. | 1.6 |
| RF-CMB-02 | El Pokémon con mayor velocidad ataca primero; ante igual velocidad se aplica la regla de desempate del sistema (decisión abierta D3). | 1.6 |
| RF-CMB-03 | El daño se calcula con las características del ejemplar considerando como mínimo: nivel, ataque, defensa, tipo del atacante, tipo(s) del defensor (decisión abierta D1). | 1.6 |
| RF-CMB-04 | El HP actual del defensor disminuye según el daño; HP = 0 ⇒ derrotado; el entrenador selecciona otro Pokémon; el combate continúa hasta que un entrenador no tenga Pokémon disponibles; gana quien conserve al menos un Pokémon capaz de continuar. | 1.6 |
| RF-CMB-05 | Efectividad de tipos: cada combinación atacante/defensor produce Muy efectivo, Normal, Poco efectivo o Sin efecto; con defensor de dos tipos se consideran ambos; la tabla se almacena en una estructura de datos apropiada. | 1.7 |

### Dominio: Torneo — grupos y clasificación (1.8, 1.9, 1.10)

| ID | Requisito | Ref. |
|----|-----------|------|
| RF-TRN-01 | El torneo tiene dos fases: fase de grupos y fase eliminatoria. | 1.8 |
| RF-TRN-02 | Grupos de 4 entrenadores; dentro de cada grupo, round-robin (cada entrenador enfrenta una vez a cada uno de los otros ⇒ 6 combates por grupo). | 1.8 |
| RF-TRN-03 | Puntuación en fase de grupos: victoria 3 puntos, empate 1 punto, derrota 0 puntos. | 1.9 |
| RF-TRN-04 | Desempate de clasificación en orden: 1) mayor cantidad de puntos, 2) mayor cantidad de victorias, 3) mayor cantidad de Pokémon derrotados en la fase de grupos, 4) criterio adicional definido por el docente (decisión abierta D9). | 1.9 |
| RF-TRN-05 | Al finalizar la fase de grupos, los entrenadores se ordenan por posición. | 1.9 |
| RF-TRN-06 | Clasificación automática a la fase eliminatoria por posición y grupo (1A, 2A, 1B, 2B, ...); el usuario NO indica manualmente quién clasifica. | 1.10 |

### Dominio: Eliminatorias (1.11)

| ID | Requisito | Ref. |
|----|-----------|------|
| RF-ELM-01 | Fase eliminatoria compuesta por: octavos de final, cuartos de final, semifinal, tercer lugar y final. No pueden existir empates: cada combate produce obligatoriamente un ganador. El perdedor queda eliminado, salvo los perdedores de semifinales (P61, P62), que disputan el tercer lugar. | 1.11 |
| RF-ELM-02 | Octavos (combates 49–56): 16 clasificados, 8 combates con emparejamientos fijos 49: 1A-2B, 50: 1C-2D, 51: 1E-2F, 52: 1G-2H, 53: 1B-2A, 54: 1D-2C, 55: 1F-2E, 56: 1H-2G. El ganador se identifica como G + número de combate (G49, ...). | 1.11.1 |
| RF-ELM-03 | Cuartos (combates 57–60): 57: G49-G50, 58: G53-G54, 59: G51-G52, 60: G55-G56. Los ganadores avanzan a semifinal. | 1.11.2 |
| RF-ELM-04 | Semifinal (combates 61–62): 61: G57-G58, 62: G59-G60. Los ganadores clasifican a la final; los perdedores se identifican P61 y P62 y disputan el tercer lugar. | 1.11.3 |
| RF-ELM-05 | Tercer lugar y final (combates 63–64): 63: P61-P62, 64: G61-G62. Posiciones finales: campeón = G64, subcampeón = P64, tercer lugar = G63, cuarto lugar = P63. Al finalizar, el sistema muestra los cuatro primeros lugares. | 1.11.4 |

### Dominio: Resultados y validación (1.12, 1.13, 1.16)

| ID | Requisito | Ref. |
|----|-----------|------|
| RF-RES-01 | Almacenar resultados de combates cargados por teclado o por archivo de texto; soportar resultados parciales; actualizar la información a medida que se incorporan nuevos resultados. | 1.12 |
| RF-RES-02 | Validar cada resultado: que los entrenadores existan, que pertenezcan al combate correspondiente, que el combate esté en la ronda correcta, que los participantes hayan clasificado correctamente, que un eliminado no vuelva a participar, que el ganador sea uno de los participantes, que no existan empates en la fase eliminatoria, que el resultado sea compatible con rondas anteriores. | 1.13 |
| RF-RES-03 | El sistema determina automáticamente los participantes de cada enfrentamiento; el usuario no puede introducir arbitrariamente los participantes de una ronda. | 1.13 |
| RF-RES-04 | Archivo de resultados: cada registro identifica como mínimo número de combate, entrenador 1, entrenador 2, resultado y ganador; puede contener resultados parciales; los faltantes se introducen por teclado. | 1.16 |

### Dominio: Clasificación / archivo de clasificación (1.17)

| ID | Requisito | Ref. |
|----|-----------|------|
| RF-CLS-01 | Generar la clasificación de los entrenadores con: posición, id, nombre, victorias, empates, derrotas, puntos; ordenada de acuerdo con las reglas establecidas (RF-TRN-04). | 1.17 |

### Dominio: Menú principal (2)

| ID | Requisito | Ref. |
|----|-----------|------|
| RF-MEN-01 | Menú principal con opciones: 1. Consultar Pokédex, 2. Registrar entrenador, 3. Crear equipo, 4. Consultar entrenadores, 5. Consultar equipos, 6. Cargar resultados, 7. Consultar clasificación, 8. Consultar enfrentamientos, 9. Consultar historial de combates, 10. Mostrar resultados del torneo, 11. Mostrar campeón, 12. Salir. El menú es modificable según la implementación. | 2 |

### Dominio: Requisitos técnicos (3, 4)

| ID | Requisito | Ref. |
|----|-----------|------|
| RF-TEC-01 | Desarrollo en C con: funciones, estructuras, arreglos y/o estructuras dinámicas cuando sean necesarias, manejo de archivos, cadenas de caracteres, TDA estudiados en el curso, recursividad y backtracking. | 3 |
| RF-TEC-02 | Funcionalidades separadas mediante funciones y módulos; `main` controla el flujo general y el menú, sin concentrar toda la lógica. | 3 |
| RF-TEC-03 | Validación de entradas del usuario: opciones inexistentes del menú, Pokémon inexistentes, entrenadores duplicados, identificadores inválidos, niveles inválidos, equipos que exceden el tamaño permitido, tipos inválidos, resultados inválidos, combates inexistentes, entrenadores no clasificados, eliminados que intentan continuar, archivos inexistentes o no abribles. Una entrada inválida NO debe terminar el programa inesperadamente. | 4 |

### Dominio: Pruebas y entrega (6, 7)

| ID | Requisito | Ref. |
|----|-----------|------|
| RF-PRB-01 | Probar como mínimo: carga correcta de la Pokédex, búsqueda de Pokémon existentes e inexistentes, registro de entrenadores, creación de ejemplares, formación y validación de equipos, combates, efectividad de tipos, resultados válidos e inválidos, clasificación, avance a la fase eliminatoria, eliminación de entrenadores, determinación del campeón y funcionamiento del backtracking. | 6 |
| RF-PRB-02 | Entrega en un único archivo comprimido con: código fuente .c, cabeceras .h, archivo de la Pokédex, archivos de datos necesarios, `main.c`, archivos adicionales para ejecutar, y una breve documentación. La estructura debe compilar y ejecutar sin dependencias externas no entregadas. | 7 |

---

## 2. Decisiones abiertas y recomendaciones

El PDF deja explícitamente "a definir en la implementación" las siguientes decisiones ("reglas establecidas para el proyecto", "regla de desempate establecida por el sistema", "criterio adicional definido por el docente", "el formato definitivo será establecido para la implementación del proyecto"). Cada una debe documentarse en el informe técnico para poder defenderla en la evaluación oral.

| # | Decisión | Dónde la deja abierta el PDF | Recomendación | Justificación |
|---|----------|------------------------------|---------------|---------------|
| D1 | **Fórmula de daño** | 1.6: "El daño deberá calcularse utilizando las características del ejemplar y considerando, como mínimo: Nivel, Ataque, Defensa, Tipo del atacante, Tipo o tipos del defensor" | Fórmula de la 1.ª generación simplificada (todo entero, C99): `daño = ( (2*Nivel/5 + 2) * Ataque / Defensa ) / 50 + 2`, luego multiplicador de efectividad (×0, ×0.5, ×1, ×2; producto sobre ambos tipos del defensor ⇒ ×0.25/×0.5/×1/×2/×4), con mínimo 1 si no es "sin efecto" | Usa exactamente el mínimo exigido, produce valores razonables con las stats base (45–80), es trivial de explicar y modificar en la defensa oral, y no introduce movimientos/poder (no exigidos por el PDF) |
| D2 | **Generación de stats del ejemplar a partir de la especie** | 1.4: "generadas a partir de las características base de su especie y las reglas establecidas para el proyecto" | `stat = stat_base + (stat_base * (Nivel − 5)) / 50 + variación` con variación pseudoaleatoria acotada (±15 % del stat base); `HP = HP_base + Nivel*3 + variación`; semilla fija (`srand(seed)`) para reproducibilidad en las pruebas scriptadas | El ejemplo del PDF (Bulbasaur base 45/49/49/45 → Ash N12: 52/34/36/29 y Misty N15: 61/42/44/37) es ilustrativo, no normativo: lo pedagógico es que dos ejemplares de la misma especie difieran. La fórmula es simple, acotada y reproducible |
| D3 | **Desempate por velocidad** | 1.6: "deberá aplicarse la regla de desempate establecida por el sistema" | Determinista: con igual velocidad ataca primero el ejemplar del entrenador local (entrenador 1 del enfrentamiento) | Reproducible en pruebas scriptadas y coherente con la validación de resultados; evita aleatoriedad en la verificación |
| D4 | **Regla de empate en fase de grupos** | 1.9 asigna 1 punto por empate, pero 1.6 no define cómo se produce un empate en un combate | Límite de turnos (p. ej. 20) sin que ningún equipo quede sin Pokémon ⇒ empate (1 punto por entrenador) | La puntuación prevé empates ⇒ debe existir una condición de empate; la más simple, testeable y explicable es el tope de turnos |
| D5 | **Regla anti-empate en eliminatorias** | 1.11: "En esta fase no podrán existir empates. Cada combate deberá producir obligatoriamente un ganador" | Tras el límite de turnos, cadena determinista: 1) más HP total restante en el equipo, 2) mayor nivel total del equipo, 3) entrenador local | Garantiza ganador siempre, de forma determinista y sin depender de azar |
| D6 | **Tamaño de equipo** | 1.4 y validación 4 ("Equipos que excedan el tamaño permitido") no fijan el tamaño | 3 Pokémon por equipo (validar tamaño permitido 1–6 en entradas) | Equilibra la duración de los combates y el espacio de búsqueda del backtracking; tamaño clásico del dominio |
| D7 | **Rango de niveles** | 1.4 y validación 4 ("Niveles inválidos") no fijan el rango | Nivel mínimo 1, máximo 50 (los ejemplos del PDF usan 12 y 15) | Mantiene combates cortos y stats dentro de rangos coherentes con las bases de la especie |
| D8 | **Formato de los 4 archivos de texto** | 1.14, 1.15, 1.16, 1.17: "El formato definitivo será establecido para la implementación del proyecto" | Ver tabla de formatos abajo | Separador `;` donde los nombres pueden contener espacios; formato espejo de lo que exige cada sección |
| D9 | **Criterio adicional de desempate de clasificación** | 1.9: "se deberá utilizar el criterio adicional definido por el docente" | 4) resultado del enfrentamiento directo entre los empatados; 5) identificador menor (determinista). **Pendiente de confirmar con el docente** | Cadena determinista y defendible; el PDF remite explícitamente al docente, por lo que conviene consultarlo antes de la entrega |

**Formatos de archivo recomendados (D8)**:

| Archivo | Formato (1 línea por registro) | Ref. |
|---------|-------------------------------|------|
| `data/pokedex.txt` | `NUM NOMBRE TIPO1 TIPO2 HP ATQ DEF VEL` — 150 líneas, sin encabezado, TIPO2 = `-` si no tiene segundo tipo. Ej. `001 Bulbasaur Planta Veneno 45 49 49 45` | 1.14 |
| `data/entrenadores.txt` | `ID;NOMBRE` y, por cada ejemplar, `ID_ENTRENADOR;NUM_ESPECIE;NOMBRE_EJEMPLAR;NIVEL` — el separador `;` tolera espacios en nombres | 1.15 |
| `data/resultados.txt` | `NUM_COMBATE;ID_ENTRENADOR1;ID_ENTRENADOR2;RESULTADO;GANADOR` con `RESULTADO` ∈ {V1, V2, E} y `GANADOR` = id o `-`; admite parciales | 1.16 |
| `data/clasificacion.txt` | Salida generada: `POSICION;ID;NOMBRE;VICTORIAS;EMPATES;DERROTAS;PUNTOS`, ordenada por los criterios de 1.9 | 1.17 |

**Decisión adicional detectada (D10) — número de entrenadores**: la numeración 49–64 fija el torneo en **exactamente 32 entrenadores** (ver sección 3). El sistema debe validar que haya 32 entrenadores registrados antes de formar los 8 grupos. El PDF no lo dice explícitamente, pero la aritmética lo exige.

---

## 3. Dato derivado clave: la numeración 49–64 implica 32 entrenadores

**Verificación aritmética contra el PDF** (consistente en todos sus puntos):

1. **Octavos de final = combates 49–56** (8 combates) — 1.11.1: "Los 16 entrenadores clasificados disputarán ocho combates" y el listado 49–56. → **16 clasificados**.
2. Los emparejamientos de octavos (1.11.1) usan los grupos **A, B, C, D, E, F, G, H** → **8 grupos**.
3. 1.8: "Cada grupo estará conformado por cuatro entrenadores" → **8 grupos × 4 = 32 entrenadores**.
4. Round-robin dentro de cada grupo: C(4,2) = 6 combates (listados explícitamente en 1.8) → **8 × 6 = 48 combates de grupos = combates 1–48**.
5. Clasifican las posiciones 1 y 2 de cada grupo (1.10: ejemplos 1A, 2A, 1B, 2B) → **8 × 2 = 16 clasificados**, consistente con el punto 1.
6. Fase eliminatoria: 8 octavos + 4 cuartos + 2 semifinales + 1 tercer lugar + 1 final = **16 combates = combates 49–64**.
7. **Total: 48 + 16 = 64 combates**, exactamente la numeración 1–64 del PDF. ✓

**Conclusión**: el torneo está dimensionado para 32 entrenadores exactos. El sistema debe exigir 32 registros (y validarlo, D10) antes de armar los grupos, y los emparejamientos 49–64 son fijos por reglamento: no se sortean ni se ingresan manualmente (RF-ELM-02 a RF-ELM-05, RF-RES-03).

---

## 4. Restricciones y riesgos

| # | Restricción / Riesgo | Detalle y mitigación |
|---|----------------------|----------------------|
| R1 | **Timeline de 2 semanas** (2026-09-01 → ~2026-09-14) | Alcance amplio: 64 combates, 9 dominios de requisitos, backtracking y validación cruzada, con verificación manual. Mitigación: orden de implementación por dependencias (pokédex → ejemplares → equipos → combate → torneo → resultados), y planificar tareas diarias en sdd-tasks |
| R2 | **Sin valgrind ni framework de tests** | La memoria dinámica y los UB no se detectan con entrada scriptada + diff. Mitigación: estilo C99 conservador (arreglos estáticos donde baste, liberación disciplinada de memoria, `-Wall -Wextra` con cero warnings), casos de prueba exhaustivos y revisión manual de frees |
| R3 | **Backtracking obligatorio (1.5)** | Es criterio explícito de evaluación; un espacio de combinaciones mal acotado (sin poda temprana o con orden de selección pobre) puede no converger o tardar demasiado. Mitigación: acotar por nivel total máximo y cantidad de Pokémon (D6, D7), poda por restricciones parciales, y probar con 150 especies |
| R4 | **Separación especie/ejemplar (concepto central)** | Mutar la Pokédex durante los combates (p. ej. escribir HP actual en la entrada de la especie) rompe el criterio de evaluación más importante (sección 5). Mitigación: estructuras independientes (`Especie` en Pokédex vs `Ejemplar` con copia de stats), y nunca escribir sobre `pokedex.txt` |
| R5 | **Validación de resultados (1.13)** | Consistencia de estado entre rondas (G49 debe alimentar el combate 57; un eliminado no reaparece; un resultado de ronda posterior antes que el de su ronda previa es inválido) es lógica de estado compleja y propensa a huecos. Mitigación: modelo de estado del torneo centralizado (combates con participantes resueltos por el sistema, no por el usuario) y casos de prueba de resultados inválidos |
| R6 | **Archivo de Pokédex con 150 especies** | El archivo debe contener las 150 especies completas y correctas; se entrega como parte del proyecto. Mitigación: generarlo una sola vez, verificarlo con una prueba de carga (RF-PRB-01) y no hardcodear los datos en el código |

---

## 5. Oportunidades para el informe técnico incremental (diagramas Mermaid)

A medida que se desarrolle, conviene documentar en el informe técnico (breve documentación exigida en 1.7/entrega):

1. **Flujo de combate (turno)** — `flowchart`: selección de Pokémon → comparación de velocidad (desempate D3) → ataque → cálculo de daño (D1) → efectividad de tipos (RF-CMB-05) → HP actual → KO → siguiente Pokémon → ganador (RF-CMB-04).
2. **Bracket del torneo 49–64** — `flowchart`/árbol: octavos 49–56 → cuartos 57–60 → semifinal 61–62 → tercer lugar 63 y final 64, con G## y P61/P62 (RF-ELM-01 a RF-ELM-05).
3. **Flujo de backtracking** — `flowchart`: selección de candidato → verificación de restricciones parciales → poda/retroceso → solución (RF-EQP-04).
4. **Arquitectura de módulos** — `graph`: `main` → menú → módulos (pokedex, entrenador, equipo, combate, torneo, resultados, archivos, validación), mostrando dependencias (RF-TEC-02).
5. **Relación especie → ejemplar** — `graph`: concepto central (1.1, 5): una especie con stats base → múltiples ejemplares con stats propias.
6. **Fase de grupos** — tabla/lista round-robin: 6 combates por grupo (1.8) y criterios de desempate (1.9).
7. **Validación de resultados** — `sequenceDiagram`: usuario → sistema → estado del torneo (RF-RES-02/RF-RES-03).

---

## Listo para propuesta

**Sí.** La especificación es completa y consistente; no existen bloqueos para pasar a la fase de propuesta. El orquestador debe informar al usuario que:
1. Las 9 decisiones abiertas (más D10: 32 entrenadores exactos) tienen recomendación concreta en la sección 2, y conviene **confirmar D9 (criterio adicional de desempate) con el docente**.
2. El torneo exige 32 entrenadores: el sistema debe validarlo (D10).
3. La próxima fase será sdd-propose, y tras ella sdd-spec (para fijar requisitos con escenarios Given/When/Then en español) y sdd-design (fórmula de daño, generación de stats, modelos de datos especie/ejemplar, arquitectura de módulos).
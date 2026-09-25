# Trazabilidad de requisitos — El Gran Torneo Pokémon

**Fecha**: 2026-09-21
**Propósito**: mapa completo de los códigos de requisito (`RF-XXX-NN`, `DOC-NN`)
usados en los comentarios del código: de dónde salen, dónde vive la lista
canónica y dónde se implementa y verifica cada uno.

---

## 1. Cómo leer esta trazabilidad

Cada requisito del enunciado recibió un **código** durante la exploración SDD.
Ese código viaja por todo el proyecto:

```
Enunciado (PDF) → inventario (exploration.md) → spec formal (openspec/specs/)
   → comentario Doxygen en el código (src/) → evidencia en verify-report.md
```

| Prefijo | Dominio | Secciones del PDF |
|---|---|---|
| `RF-PDX` | Pokédex | 1.1, 1.2, 1.14 |
| `RF-ENT` | Entrenadores | 1.3, 1.15 |
| `RF-EQP` | Equipos, ejemplares y backtracking | 1.4, 1.5 |
| `RF-CMB` | Combate y efectividad | 1.6, 1.7 |
| `RF-TRN` | Torneo: grupos y clasificación | 1.8–1.10 |
| `RF-ELM` | Eliminatorias | 1.11 |
| `RF-RES` | Resultados y su validación | 1.12, 1.13, 1.16 |
| `RF-CLS` | Clasificación (archivo) | 1.17 |
| `RF-MEN` | Menú principal | 2 |
| `RF-TEC` | Requisitos técnicos | 3, 4 |
| `RF-PRB` | Pruebas y entrega | 6, 7 |
| `DOC` | Documentación del proyecto | Requisitos del equipo |

## 2. Totales

| Conjunto | Cantidad |
|---|---|
| Requisitos formales en las 11 specs (`openspec/specs/`) | **42** (38 RF + 4 DOC) |
| Escenarios Given/When/Then | **70** |
| Códigos del inventario original del PDF (archive) | 41 RF + 4 DOC |
| Requisitos transversales (fuera de las specs) | 3 (`RF-TEC-01`, `RF-PRB-01`, `RF-PRB-02`) |

## 3. Dónde vive la lista canónica

- **Specs formales** (fuente de verdad, una por capacidad):
  `openspec/specs/<capacidad>/spec.md` — cada requisito es un encabezado
  `### Requirement: <nombre> (<código>)` con sus escenarios.
- **Inventario original con referencia al PDF**:
  `openspec/changes/archive/2026-09-21-gran-torneo-pokemon/exploration.md`.
- **Evidencia de verificación por lote**:
  `openspec/changes/archive/2026-09-21-gran-torneo-pokemon/verify-report.md`
  (secciones F0–F11).

Para listar todos los requisitos:

```bash
grep -h "### Requirement" openspec/specs/*/spec.md
```

---

## 4. Tabla de trazabilidad (42 requisitos formales)

La columna **Verificación** indica el lote de la batería de pruebas
(`tests/run_tests.sh`) y la sección del `verify-report.md` con la evidencia.

### Pokédex — `openspec/specs/pokedex/spec.md`

| Código | Requisito | Esc. | Implementación | Verificación |
|---|---|---|---|---|
| RF-PDX-01 | Catálogo de 150 especies | 2 | `src/pokedex.c` (`pokedex_cargar`), `data/pokedex.txt` | F1 · verify §F1 |
| RF-PDX-02 | Atributos mínimos por especie | 1 | `src/pokedex.c` (struct `Especie`) | F1 · verify §F1 |
| RF-PDX-03 | Catálogo de 18 tipos | 2 | `src/tipos.c` (enum `Tipo`), `pokedex.c` | F1 · verify §F1 |
| RF-PDX-04 | Inmutabilidad de la Pokédex | 1 | `src/pokedex.c` (API `const`), `data/pokedex.txt` | F1/F3 · verify §F1 |
| RF-PDX-05 | Consultas de la Pokédex | 2 | `src/pokedex.c` + submenú `src/main.c` | F1 · verify §F1 |
| RF-PDX-06 | Lectura desde archivo | 1 | `src/pokedex.c` (parseo estricto con `validar_separar_campos`) | F1 · verify §F1 |

### Entrenadores — `openspec/specs/entrenadores/spec.md`

| Código | Requisito | Esc. | Implementación | Verificación |
|---|---|---|---|---|
| RF-ENT-01 | Datos del entrenador | 1 | `src/entrenador.c` (struct `Entrenador`) | F2 · verify §F2 |
| RF-ENT-02 | Identificador único | 2 | `src/entrenador.c` (`entrenador_buscar`/`registrar`) | F2 · verify §F2 |
| RF-ENT-03 | Persistencia | 1 | `src/archivos.c` (`archivos_cargar/guardar_entrenadores`) | F2/F7 · verify §F2, §F7 |

### Equipos y ejemplares — `openspec/specs/equipos/spec.md`

| Código | Requisito | Esc. | Implementación | Verificación |
|---|---|---|---|---|
| RF-EQP-01 | Pertenencia a la Pokédex | 1 | `src/equipo.c` (`equipo_crear_ejemplar`) | F2 · verify §F2 |
| RF-EQP-02 | Creación de ejemplar (fórmula D2) | 1 | `src/equipo.c` (stats con `(id*7)%16`) | F2 · verify §F2 |
| RF-EQP-03 | Separación especie/ejemplar | 2 | `src/equipo.c` (`const Especie *`, solo `hp_actual` muta) | F2 · verify §F2 |
| RF-EQP-04 | Backtracking de formación | 2 | `src/equipo.c` (`equipo_formar_backtracking`, `bt_rec`) | F4 · verify §F4 |
| RF-EQP-05 | Validación de equipos | 2 | `src/equipo.c` (`equipo_validar`, `equipo_agregar_ejemplar`) | F2 · verify §F2 |

### Combate — `openspec/specs/combate/spec.md`

| Código | Requisito | Esc. | Implementación | Verificación |
|---|---|---|---|---|
| RF-CMB-01 | Enfrentamiento | 1 | `src/combate.c` (`combate_ejecutar`) | F3 · verify §F3 |
| RF-CMB-02 | Orden por velocidad | 2 | `src/combate.c` (`combate_ataca_primero`) | F3 · verify §F3 |
| RF-CMB-03 | Fórmula de daño (D1) | 5 | `src/combate.c` (`combate_calcular_danio`) | F3 · verify §F3 |
| RF-CMB-04 | HP y derrota | 2 | `src/combate.c` (KO, reemplazo, victoria) | F3 · verify §F3 |
| RF-CMB-05 | Efectividad de tipos | 1 | `src/tipos.c` (matriz 18×18, `tipos_multiplicador`) | F1 · verify §F1 |

### Torneo (grupos) — `openspec/specs/torneo/spec.md`

| Código | Requisito | Esc. | Implementación | Verificación |
|---|---|---|---|---|
| RF-TRN-01 | Dos fases | 1 | `src/torneo.c` (máquina de estados) | F5 · verify §F5 |
| RF-TRN-02 | Grupos round-robin | 2 | `src/torneo.c` (`torneo_armar_grupos`, calendario 1–48) | F5 · verify §F5 |
| RF-TRN-03 | Puntuación (3/1/0) | 1 | `src/torneo.c` (`torneo_aplicar_resultado`) | F5 · verify §F5 |
| RF-TRN-04 | Desempate de clasificación | 3 | `src/torneo.c` (`torneo_ordenar_grupos`: puntos→victorias→KOs→D9→id) | F5 · verify §F5 |
| RF-TRN-05 | Orden final de grupos | 1 | `src/torneo.c` (`torneo_mostrar_clasificacion`) | F5 · verify §F5 |
| RF-TRN-06 | Clasificación automática | 1 | `src/torneo.c` (`torneo_clasificados`, etiquetas 1A..2H) | F5 · verify §F5 |

### Eliminatorias — `openspec/specs/eliminatorias/spec.md`

| Código | Requisito | Esc. | Implementación | Verificación |
|---|---|---|---|---|
| RF-ELM-01 | Estructura sin empates | 2 | `src/torneo.c` (rechazo de empate; anti-empate D5 en `combate.c`) | F6 · verify §F6 |
| RF-ELM-02 | Octavos de final | 1 | `src/torneo.c` (`ORIGEN_PARTICIPANTE`, combates 49–56) | F6 · verify §F6 |
| RF-ELM-03 | Cuartos de final | 1 | `src/torneo.c` (57–60: G49-G50, G53-G54, G51-G52, G55-G56) | F6 · verify §F6 |
| RF-ELM-04 | Semifinales | 1 | `src/torneo.c` (61–62; perdedores P61/P62) | F6 · verify §F6 |
| RF-ELM-05 | Tercer lugar y final | 1 | `src/torneo.c` (63 = P61-P62; 64 = G61-G62; posiciones finales) | F6 · verify §F6 |

### Resultados — `openspec/specs/resultados/spec.md`

| Código | Requisito | Esc. | Implementación | Verificación |
|---|---|---|---|---|
| RF-RES-01 | Almacenamiento y parciales | 1 | `src/resultados.c`, `data/resultados.txt` | F7 · verify §F7 |
| RF-RES-02 | Validación de resultados | 3 | `src/resultados.c` (`resultados_validar`, catálogo §8.1) | F6/F7 · verify §F6, §F7 |
| RF-RES-03 | Participantes resueltos por el sistema | 1 | `src/torneo.c` (`torneo_participantes`) | F6 · verify §F6 |
| RF-RES-04 | Archivo de resultados | 1 | `src/resultados.c` / `src/archivos.c` (esquema §5.3) | F7 · verify §F7 |

### Clasificación — `openspec/specs/clasificacion/spec.md`

| Código | Requisito | Esc. | Implementación | Verificación |
|---|---|---|---|---|
| RF-CLS-01 | Tabla de clasificación | 2 | `src/torneo.c` (pantalla) + `src/archivos.c` (archivo §5.4) | F5/F7 · verify §F5, §F7 |

### Menú y `main` — `openspec/specs/menu/spec.md`

| Código | Requisito | Esc. | Implementación | Verificación |
|---|---|---|---|---|
| RF-MEN-01 | Opciones del menú | 2 | `src/main.c` (12 opciones + submenús) | F0–F6 · verify §F0…§F6 |
| RF-TEC-02 | `main` limitado a flujo y menú | 1 | `src/main.c` (sin lógica de negocio; delega en módulos) | F0 · verify §F0 |

### Validación — `openspec/specs/validacion/spec.md`

| Código | Requisito | Esc. | Implementación | Verificación |
|---|---|---|---|---|
| RF-TEC-03 | Validación de entradas | 6 | `src/validacion.c` (`validar_leer_entero/cadena`, `validar_separar_campos`) | F8 · verify §F8 |

### Documentación — `openspec/specs/documentacion/spec.md`

| Código | Requisito | Esc. | Implementación | Verificación |
|---|---|---|---|---|
| DOC-01 | Convención Doxygen | 2 | `Doxyfile`, `docs/convenciones-doxygen.md`, bloques Doxygen en `src/` | F0/F10 · verify §F0, §F10 |
| DOC-02 | Plan de control de planificación | 1 | `docs/planificacion.md` | F0/F10 · verify §F0, §F10 |
| DOC-03 | Informe técnico incremental | 2 | `docs/informe-tecnico.md` (D1–D10 + diagramas) | F0/F10 · verify §F0, §F10 |
| DOC-04 | Idioma de la documentación | 1 | Todos los documentos y comentarios del proyecto | F0/F10 · verify §F0, §F10 |

---

## 5. Requisitos transversales (fuera de las 11 specs)

Viven en `exploration.md`/`design.md`/`tasks.md` (archive) porque atraviesan
todo el proyecto en lugar de una capacidad:

| Código | Requisito | Dónde se cumple | Evidencia |
|---|---|---|---|
| RF-TEC-01 | Desarrollo en C con funciones, estructuras, arreglos/estructuras dinámicas, archivos, cadenas, TDAs, recursividad y backtracking | Todo `src/` (10 módulos; listas enlazadas en equipos; recursividad + backtracking en `equipo.c`) | `verify-report.md` §F4 (backtracking), §F2 (TDA lista) |
| RF-PRB-01 | Pruebas del sistema (lista mínima del PDF §6) | `tests/run_tests.sh` + `tests/casos/` + `tests/probes/` | F9: **23 PASS / 0 FALLA**; cobertura 16/16 · verify §F9 |
| RF-PRB-02 | Entrega en un único comprimido que compila y ejecuta sin dependencias externas | `scripts/empaquetar.sh` (`make entrega`), `scripts/verificar_entrega.sh` | F11: **7 PASS / 0 FALLA** en sandbox limpio · verify §F11 |

## 6. Cómo se verifica hoy

```bash
# Batería completa versionada (compila + 20 casos + 2 probes)
bash tests/run_tests.sh

# Verificación del paquete de entrega (extrae, compila y ejecuta sin el repo)
bash scripts/verificar_entrega.sh

# Listar los requisitos formales
grep -h "### Requirement" openspec/specs/*/spec.md
```

La cadena completa de trazabilidad es:

```
PDF del enunciado
  → openspec/changes/archive/…/exploration.md   (41 RF + 4 DOC con referencia de sección)
  → openspec/specs/<capacidad>/spec.md          (42 requisitos, 70 escenarios)
  → comentarios Doxygen de src/                 (cada módulo cita sus RF)
  → tests/ + probes/                            (batería versionada)
  → openspec/changes/archive/…/verify-report.md (evidencia por requisito, F0–F11)
```

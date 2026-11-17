# Informe de Archive — Simulación del torneo + historial + cierres

**Change**: `simulacion-torneo` · **Fase**: sdd-archive · **Fecha**: 2026-11-16
**Idioma del artefacto**: español neutro y profesional
**Archivado en**: `openspec/changes/archive/2026-11-16-simulacion-torneo/`

---

## Estado final del cambio

- **7 lotes completados** (L1–L7), integrados a `main` por fast-forward desde
  la rama `feat/simulacion-torneo`.
- **Verificación PASS**: `make` con `gcc -std=c99 -Wall -Wextra` **cero
  warnings**; `bash tests/run_tests.sh` → **26 PASS, 0 FALLA** (23 casos de
  menú + 2 probes); paquete de entrega verificado en directorio limpio →
  **7 PASS, 0 FALLA**.
- Sin CRITICAL ni blockers.

## Cambios funcionales

| Lote | Entregable |
|---|---|
| L1 | Modo silencioso (`bool silencioso`) en el motor de combate |
| L2 | Simulación automática + relleno de equipos por backtracking (nivel medio 30 y ventana rotativa) |
| L3 | Opción 9 historial + módulo `pantalla.c` + guía de estilo de pantalla |
| L4 | Persistencia de resultados al salir |
| L5 | Finalización con 63 y 64 resueltos + round-trip estable del guardado |
| L6 | Tests de combate autónomos + datos normalizados + documentación |
| L7 | Rediseño UI/UX de las consultas al estilo de pantalla |

## Sync de delta specs a specs principales

| Dominio | Acción |
|---|---|
| `simulacion` | Creada (capacidad nueva; RF-SIM-01..04) |
| `menu` | Añadidas RF-MEN-02 y RF-MEN-03 |
| `torneo` | Añadida RF-ELM-06 |
| `resultados` | Añadidas RF-RES-05 y RF-RES-06 |
| `entrenadores` | RF-ENT-03 modificada (round-trip estable) |
| `equipos` | RF-EQP-04 modificada (nivel objetivo) |

## Movimiento al archive

`git mv openspec/changes/2026-11-16-simulacion-torneo →
openspec/changes/archive/2026-11-16-simulacion-torneo`.

## Contenido del archive

- `proposal.md` ✅
- `design.md` ✅
- `tasks.md` ✅ (L1–L7 completos)
- `specs/` ✅ (6 delta specs)
- `verify-report.md` ✅
- `archive-report.md` ✅ (este informe)

## Notas

- `data/entrenadores.txt` conserva el equipo nuevo de Misty y el orden
  original del resto; los tests de combate ya no dependen de ese archivo.
- El paquete de entrega excluye `docs/`, `openspec/` y `scripts/`; incluye
  solo `src/`, `data/`, `tests/`, `Makefile`, `Doxyfile` y `README.md`.
- Los comentarios del código fueron revisados para eliminar referencias a
  códigos de requisitos, secciones y artefactos de proceso; todos los archivos
  llevan el autor y la cédula del estudiante.

## Veredicto

Ciclo SDD completo: planificado, implementado, verificado y archivado. Change
listo para commit y push por el orquestador.

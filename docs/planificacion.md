# Planificación del proyecto — El Gran Torneo Pokémon

**Proyecto**: El Gran Torneo Pokémon · **Curso**: Programación I / Algoritmos I
**Documento**: DOC-02 (spec `documentacion`) · **Periodo**: 14 días (plan nominal 2026-09-01 → 2026-09-14; ejecución real hasta 2026-09-21)
**Ritmo estimado**: 2–3 h/día · **Idioma**: español neutro y profesional
**Estado global**: **14/14 — proyecto cerrado** (F0–F11 ✅ Hecho; el cierre de entrega F11 se aplicó el 2026-09-21)

---

## 1. Leyenda de estados

| Estado | Significado |
|--------|-------------|
| ✅ Hecho | Fase completada, compila limpio y pasa su verificación de fase |
| 🔄 En progreso | Fase iniciada en la fecha indicada, aún sin cerrar |
| ⏳ Pendiente | Fase planificada, sin trabajo iniciado |

## 2. Plan por fases (alineado con `tasks.md` F0–F11)

| Fase | Contenido | Fechas | Estado |
|------|-----------|--------|--------|
| F0 | Convenciones (Doxyfile + guía), planificación, informe esqueleto, 10 headers, `main.c` con menú, Makefile | 2026-09-01 | ✅ Hecho |
| F1 | Pokédex y tipos: `tipos.c` (matriz 18×18) + `pokedex.c` + `data/pokedex.txt` (150 especies) + `data/efectividad.txt` | 2026-09-01 → 2026-09-02 | ✅ Hecho |
| F2 | Entrenadores y ejemplares (separación especie/ejemplar, `entrenadores.txt`) | 2026-09-19 | ✅ Hecho |
| F3 | Combate (fórmula de daño D1, orden D3, empate D4/D5) | 2026-09-21 | ✅ Hecho |
| F4 | Backtracking de formación de equipos (podas §4.3 + poda 3b) | 2026-09-21 | ✅ Hecho |
| F5 | Torneo: grupos + clasificación (32 entrenadores, 8 grupos, combates 1–48) | 2026-09-21 | ✅ Hecho |
| F6 | Eliminatorias (bracket 49–64, tercer lugar, campeón) | 2026-09-21 | ✅ Hecho |
| F7 | Archivos y resultados (carga/validación de los `.txt`) | 2026-09-21 | ✅ Hecho |
| F8 | Validación integral de entradas (RF-TEC-03) | 2026-09-21 | ✅ Hecho |
| F9 | Pruebas (batería scriptada completa versionada, RF-PRB-01) | 2026-09-21 | ✅ Hecho |
| F10 | Documentación final: informe técnico D1–D10 completo, plan 14/14, Doxygen render sin warnings (DOC-03/DOC-04) | 2026-09-21 | ✅ Hecho |
| F11 | Entrega: empaquetado, checklist y cierre (lote F11 de `tasks.md`) | 2026-09-21 | ✅ Hecho |

Nota de numeración: las filas de esta tabla están alineadas con `tasks.md`
(F0–F11). En versiones anteriores existió un desfase de +1 (la fila etiquetada
«F4» era realmente F3, etc.) por partir F1 en dos filas; se corrigió en F10.

## 3. Gate por fase

Cada fase debe cumplir, antes de avanzar a la siguiente:

1. `make` compila con `gcc -std=c99 -Wall -Wextra` y **cero warnings**.
2. La batería scriptada de la fase (stdin → stdout → `diff`) pasa completa.
3. Las tareas de la fase quedan marcadas como hechas en `tasks.md`.

## 4. Decisiones pendientes del docente — RESUELTAS (2026-09-19)

El docente respondió: **libertad total de modelado** (niveles, tamaño de equipo
y reglas a conveniencia del estudiante) y fórmula oficial NO exigida — se
admite una fórmula simplificada documentada y validada con el nivel (D2 con
variación determinista: `(base × nivel / 50) + nivel + 5/10 + (id × 7) % 16`).
Valores adoptados e implementados:

| # | Decisión | Valor adoptado | Estado |
|---|----------|----------------|--------|
| 1 | **Rango de niveles 1–50 vs 1–100** | **1–100** (canónico): `NIVEL_MAX = 100`; delta de spec aplicado (rechaza niveles 0 y 101) | ✅ Resuelta |
| 2 | **Equipo de torneo 3 vs 6** | **6** (equipo completo): `TAM_EQUIPO_TORNEO = 6`; la distinción desaparece | ✅ Resuelta |
| 3 | **Criterio adicional de desempate (D9)** | Enfrentamiento directo en grupos + id menor (cadena de 5 criterios, RF-TRN-04) | ✅ Resuelta |

Todas las decisiones D1–D10 quedan cerradas y justificadas en
`docs/informe-tecnico.md` §3 para la defensa oral.

## 5. Criterio de aborto (día 12) — SUPERADO

El plan preveía: si al día 12 la validación integral (F8, «Validación integral
de entradas») no pasaba, se congelaba la funcionalidad F0–F8 ya verde y se
entregaba el alcance verificado. **El criterio quedó superado**: F8 se completó
el 2026-09-21 con la batería 15/15 y la regresión completa; el desarrollo
continuó hasta la entrega final (F10/F11) sin aplicar el aborto.

## 6. Cierre

- [x] Al finalizar cada fase: actualizar el estado de la tabla y marcar tareas en `tasks.md` (hecho en F0–F11).
- [x] F10: informe técnico completo (D1–D10 + diagramas finales) y Doxygen render sin warnings del proyecto (DOC-03/DOC-04 cerrados).
- [x] Día 14 / lote F11: checklist de entrega (empaquetado en comprimido único, verificación sin dependencias externas) y cierre del documento con el plan 14/14.

## 7. Entrega (F11) — CHECKLIST FINAL

Paquete único generado con `make entrega` (equivale a `bash scripts/empaquetar.sh`):
`dist/gran-torneo-pokemon-entrega.zip`. Verificación completa del paquete con
`bash scripts/verificar_entrega.sh` (extracción a directorio temporal limpio,
compilación, prueba de humo y batería completa). Cumplimiento de la sección 7
del enunciado:

- [x] Código fuente `.c` (10 módulos en `src/`).
- [x] Archivos de cabecera `.h` (10 en `src/`, incluido `constantes.h`).
- [x] Archivo `main.c` (menú de 12 opciones, RF-MEN-01).
- [x] Archivo de la Pokédex (`data/pokedex.txt`, 150 especies).
- [x] Archivos de datos necesarios (`data/efectividad.txt`, `data/entrenadores.txt`, `data/resultados.txt`).
- [x] Documentación breve (`docs/planificacion.md`, `docs/informe-tecnico.md`, `docs/convenciones-doxygen.md`, `README.md`; el html de Doxygen se excluye por regenerable).
- [x] `Doxyfile` (documentación en español, DOC-01).
- [x] Tests (`tests/` con la batería scriptada versionada: 20 casos + 2 probes).
- [x] Paquete comprimido único (`dist/gran-torneo-pokemon-entrega.zip`, 212 KB, 93 archivos; el zip NO se versiona: `dist/` está en `.gitignore`).
- [x] Compilación y ejecución sin dependencias externas: rutas relativas (`data/...`), sin rutas absolutas en `src/` ni en el `Makefile`; verificado F11.2 desde un directorio temporal limpio (`make clean && make` cero warnings, humo exit 0, `bash tests/run_tests.sh` → 23 PASS, 0 FALLA).

**Comando de empaquetado**: `make entrega` o `bash scripts/empaquetar.sh`.
**Comando de verificación**: `bash scripts/verificar_entrega.sh`.

Con esta sección, el plan queda **cerrado 14/14**: F0–F11 todas ✅ Hecho.
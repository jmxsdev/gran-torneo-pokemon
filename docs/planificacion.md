# Planificación del proyecto — El Gran Torneo Pokémon

**Proyecto**: El Gran Torneo Pokémon · **Curso**: Programación I / Algoritmos I
**Documento**: DOC-02 (spec `documentacion`) · **Periodo**: 14 días, 2026-09-01 → 2026-09-14
**Ritmo estimado**: 2–3 h/día · **Idioma**: español neutro y profesional

---

## 1. Leyenda de estados

| Estado | Significado |
|--------|-------------|
| ✅ Hecho | Fase completada, compila limpio y pasa su verificación de fase |
| 🔄 En progreso | Fase iniciada en la fecha indicada, aún sin cerrar |
| ⏳ Pendiente | Fase planificada, sin trabajo iniciado |

## 2. Plan por fases

| Fase | Contenido | Fechas | Estado |
|------|-----------|--------|--------|
| F0 | Convenciones (Doxyfile + guía), planificación, informe esqueleto, 10 headers, `main.c` con menú, Makefile | 2026-09-01 | ✅ Hecho |
| F1 | Pokédex: `tipos.c` + `pokedex.c` + `data/pokedex.txt` (150 especies) | 2026-09-01 → 2026-09-02 | ✅ Hecho |
| F2 | Tipos y efectividad (matriz 18×18, multiplicador) | 2026-09-02 → 2026-09-03 | ✅ Hecho |
| F3 | Entrenadores y ejemplares (separación especie/ejemplar, `entrenadores.txt`) | 2026-09-19 | ✅ Hecho |
| F4 | Combate (fórmula de daño D1, orden D3, empate D4/D5) | 2026-09-21 | ✅ Hecho |
| F5 | Backtracking de formación de equipos (podas §4.3) | 2026-09-06 → 2026-09-07 | ⏳ Pendiente |
| F6 | Torneo: grupos + clasificación (32 entrenadores, 8 grupos, 1–48) | 2026-09-07 → 2026-09-09 | ⏳ Pendiente |
| F7 | Eliminatorias (bracket 49–64, tercer lugar, campeón) | 2026-09-09 → 2026-09-10 | ⏳ Pendiente |
| F8 | Archivos y resultados (carga/validación de los 4 `.txt`) | 2026-09-10 → 2026-09-12 | ⏳ Pendiente |
| F9 | Validación integral de entradas (RF-TEC-03) | 2026-09-12 → 2026-09-13 | ⏳ Pendiente |
| F10 | Pruebas (batería scriptada completa) + documentación final | 2026-09-13 | ⏳ Pendiente |
| F11 | Entrega: empaquetado, checklist y cierre | 2026-09-14 | ⏳ Pendiente |

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
Valores adoptados:

| # | Decisión | Valor adoptado | Estado |
|---|----------|----------------|--------|
| 1 | **Rango de niveles 1–50 vs 1–100** | **1–100** (canónico): `NIVEL_MAX = 100`; delta de spec aplicado (rechaza niveles 0 y 101) | ✅ Resuelta |
| 2 | **Equipo de torneo 3 vs 6** | **6** (equipo completo): `TAM_EQUIPO_TORNEO = 6`; la distinción desaparece | ✅ Resuelta |
| 3 | **Criterio adicional de desempate (D9)** | Enfrentamiento directo en grupos + id menor (cadena de 5 criterios, RF-TRN-04) | ✅ Resuelta |

## 5. Criterio de aborto (día 12)

Si al 2026-09-12 la validación integral (F9) no pasa, se congela la funcionalidad
F0–F8 ya verde y se entrega con el alcance verificado, documentando lo pendiente
en `docs/informe-tecnico.md` (criterio del plan de rollback de la propuesta).

## 6. Cierre

- [ ] Al finalizar cada fase: actualizar el estado de la tabla y marcar tareas en `tasks.md`.
- [ ] Día 14: checklist de entrega (F11) y cierre del documento con el plan 14/14.
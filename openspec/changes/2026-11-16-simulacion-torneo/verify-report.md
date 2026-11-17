# Verificación: Simulación del torneo + historial de combates + cierres de persistencia

**Change**: `simulacion-torneo` · **Fase**: sdd-verify · **Fecha**: 2026-11-16
**Idioma del artefacto**: español neutro y profesional.

---

## Resumen

| Lote | Alcance | Evidencia |
|------|---------|-----------|
| L1 | Modo silencioso (`bool silencioso`) en `combate_ejecutar` | `make` cero warnings; probe_efectividad 21/21; batería sin regresiones nuevas |
| L2 | Módulo `simulacion.c` (relleno D18 + `simulacion_ejecutar`) + submenú opción 6 | E2E: 32 con equipos → campeón Gladion; 32 sin equipos → relleno nivel 30 + ventana → campeón Max; rechazo con 31; híbrido 20 parciales + simulación |
| L3 | Opción 9 historial (estilo D19) + módulo `pantalla.c` | Goldens 021/022 PASS; batería 21/4 en ese punto |
| L4 | Persistencia de resultados (D16) | Golden 023 PASS; round-trip 20+1 verificado en sandbox; goldens 016/017/019/022 regenerados |
| L5 | Fix 63/64 (D14) + round-trip de orden (D15) | E2E 64→63 (mensaje + posiciones completas); 31 equipos intactos tras guardar; probe_torneo_completo 29/29 |
| L6 | Tests de combate autónomos (D17) + cierre | 012–015 reescritos `_sin_entrenadores`; `data/entrenadores.txt` normalizado (equipo de Misty conservado, orden original); docs actualizados |

**Batería final**: `bash tests/run_tests.sh` → **BATERÍA F9: 26 PASS, 0 FALLA** (23 casos de menú + 2 probes). `make` con `gcc -std=c99 -Wall -Wextra` **cero warnings**.

## Detalle por requisito

| Código | Verificación |
|--------|--------------|
| RF-SIM-01 | E2E L2: simulación completa corona campeón sin intervención (con y sin equipos previos) |
| RF-SIM-02 | Dos ejecuciones con los mismos datos producen el mismo campeón (determinista, sin `srand`) |
| RF-SIM-03 | La simulación aplica resultados solo vía `torneo_aplicar_resultado`; probe_torneo_completo valida el estado |
| RF-SIM-04 | Relleno por backtracking: Ash sin equipo recibió 6 especies distintas a nivel 30; ventana rotativa produce equipos distintos por posición |
| RF-RES-05 | Golden 023: teclado → salir → "Resultados guardados en data/resultados.txt."; archivo del sandbox con el resultado aplicado |
| RF-RES-06 | Goldens 021 (0/64 pendientes) y 022 (20 resueltos + 44 pendientes) muestran la tabla completa |
| RF-MEN-02 | `case 9` ya no responde "en construcción"; goldens 021/022 lo ejercitan |
| RF-MEN-03 | Submenú `3. Simular torneo automáticamente` ejercitado E2E (L2) |
| RF-ELM-06 | E2E L5: aplicar 64 antes de 63 no finaliza; al resolver 63 finaliza con las 4 posiciones (Ash/Clemont/Jessie/Shauna) |
| RF-ENT-03 (mod) | L5: tras guardar, los 31 equipos no tocados quedan byte-idénticos (round-trip estable) |
| RF-EQP-04 (mod) | L2: `nivel_objetivo=30` genera ejemplares a nivel 30; `nivel_objetivo=0` conserva el comportamiento anterior (goldens 010/011 intactos) |
| RF-PRB-01 (mod) | 012–015 no leen `data/entrenadores.txt`; la batería queda verde con cualquier orden/composición de equipos en ese archivo |

## Commits

- `75d7bc5` chore: artefactos SDD del change
- `27889b9` feat: modo silencioso (L1)
- `049f671` feat: simulación + relleno (L2)
- `2db0988` feat: opción 9 historial + estilo D19 (L3)
- `253f2df` feat: persistencia de resultados (L4)
- `f1cc1e3` fix: 63/64 + round-trip de orden (L5)
- L6 (tests autónomos + cierre) — commit al cerrar el lote

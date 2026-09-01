/**
 * @file combate.h
 * @brief Reglamento del combate 1 vs 1: daño (D1), orden (D3) y fin (D4/D5).
 * @author <Nombre del estudiante>
 * @date 2026-09-01
 */

#ifndef COMBATE_H
#define COMBATE_H

#include <stdbool.h>

#include "entrenador.h"

/**
 * @brief Resultado de un combate ejecutado (RF-CMB-01..05).
 *
 * El módulo combate devuelve el resultado; quien lo orquesta lo pasa a
 * torneo_aplicar_resultado para actualizar el estado del torneo.
 */
typedef struct {
    bool empate;
    int  id_ganador;
    int  kos_local, kos_visita;
} ResultadoCombate;

/**
 * @brief Calcula el daño de un ataque del ejemplar atacante al defensor.
 *
 * Fórmula cerrada D1: base = (2*nivel/5 + 2) * ataque / defensa + 2 con
 * enteros en ese orden; multiplicador de tipos del tipo primario del
 * atacante contra ambos tipos del defensor (RF-CMB-03/RF-CMB-05). Mínimo 1
 * si el multiplicador es > 0; 0 si es 0.
 *
 * @param atacante Ejemplar que ataca (no debe ser NULL).
 * @param defensor Ejemplar que recibe (no debe ser NULL).
 * @return Daño calculado, entero >= 0; 0 si los parámetros son inválidos.
 */
int combate_calcular_danio(const Ejemplar *atacante, const Ejemplar *defensor);

/**
 * @brief Decide qué ejemplar ataca primero según su velocidad (D3).
 *
 * Mayor velocidad ataca primero; empate => ataca el ejemplar del entrenador
 * local (entrenador 1 del enfrentamiento).
 *
 * @param local  Ejemplar activo del entrenador local (no debe ser NULL).
 * @param visita Ejemplar activo del entrenador visitante (no debe ser NULL).
 * @return true si ataca primero *local; false si ataca primero *visita.
 */
bool combate_ataca_primero(const Ejemplar *local, const Ejemplar *visita);

/**
 * @brief Ejecuta un combate completo entre dos equipos (RF-CMB-01..05).
 *
 * Restaura hp_actual = hp_max de los participantes, aplica el ciclo de
 * turnos con tope MAX_TURNOS_COMBATE y resuelve el final: agotamiento de
 * disponibles, empate en grupos (D4) o anti-empate en eliminatoria (D5).
 *
 * @param local  Entrenador local (no debe ser NULL).
 * @param visita Entrenador visitante (no debe ser NULL).
 * @param res    Puntero donde se escribe el resultado (no debe ser NULL).
 * @return true si el combate se ejecutó y *res quedó definido; false si
 *         los parámetros son inválidos.
 */
bool combate_ejecutar(Entrenador *local, Entrenador *visita,
                      ResultadoCombate *res);

#endif /* COMBATE_H */
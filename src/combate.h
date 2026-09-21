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
 * torneo_aplicar_resultado para actualizar el estado del torneo. kos_local
 * son los Pokémon del visitante que el local derrotó, y kos_visita los del
 * local que derrotó el visitante (criterio 3 de RF-TRN-04).
 */
typedef struct {
    bool empate;
    int  id_ganador;
    int  kos_local, kos_visita;
} ResultadoCombate;

/**
 * @brief Función de selección del Pokémon activo, provista por el llamador.
 *
 * El módulo combate NO lee de la consola: cuando un entrenador debe elegir
 * su Pokémon activo (inicio del combate o tras un KO), llama a esta función
 * con el entrenador y el rol en curso. La función muestra los disponibles
 * (hp_actual > 0) y devuelve la posición elegida (1..N) o -1 ante EOF o
 * cancelación.
 *
 * @param ent Entrenador que elige (nunca NULL).
 * @param rol Rol textual del turno ("local", "visita" o "reemplazo").
 * @return Posición 1-based del Pokémon elegido entre los disponibles;
 *         -1 si la entrada termina (EOF) o la elección es inválida.
 */
typedef int (*CombateSeleccionar)(const Entrenador *ent, const char *rol);

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
 * Restaura hp_actual = hp_max de los participantes (D2), pide el Pokémon
 * inicial de cada entrenador y aplica el ciclo de turnos con tope
 * MAX_TURNOS_COMBATE (un turno = intercambio completo, ambos activos atacan
 * según el orden de velocidad D3). Resuelve el final: agotamiento de
 * disponibles (RF-CMB-04), empate en fase de grupos (D4) o anti-empate en
 * eliminatoria por la cadena HP total -> nivel total -> entrenador 1 (D5).
 * Registra por consola la traza de turnos, daños y KOs.
 *
 * @param local         Entrenador local (entrenador 1; no debe ser NULL).
 * @param visita        Entrenador visitante (no debe ser NULL).
 * @param es_eliminatoria true si el combate es de eliminatoria (anti-empate
 *                      D5); false si es de fase de grupos (empate D4).
 * @param seleccionar   Función que elige el Pokémon activo (no NULL).
 * @param res           Puntero donde se escribe el resultado (no NULL).
 * @return true si el combate se ejecutó y *res quedó definido; false si los
 *         parámetros son inválidos, un equipo está vacío o el combate se
 *         canceló por fin de entrada (EOF).
 */
bool combate_ejecutar(Entrenador *local, Entrenador *visita,
                      bool es_eliminatoria, CombateSeleccionar seleccionar,
                      ResultadoCombate *res);

#endif /* COMBATE_H */
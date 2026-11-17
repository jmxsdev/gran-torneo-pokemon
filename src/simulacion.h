/**
 * @file simulacion.h
 * @brief Simulación automática del torneo: relleno de equipos por
 *        backtracking y ejecución de los 64 combates (RF-SIM-01..04).
 * @author Nombre del estudiante
 * @date 2026-11-16
 */

#ifndef SIMULACION_H
#define SIMULACION_H

#include <stdbool.h>

#include "pokedex.h"
#include "torneo.h"

/**
 * @brief Simula el torneo completo a partir de los entrenadores y sus equipos.
 *
 * Primero rellena, con el algoritmo de backtracking (D18), los equipos vacíos
 * o inválidos: nivel medio uniforme y ventana de especies rotativa
 * determinista por posición. Luego ejecuta los combates pendientes 1..64 con
 * el motor de combate en modo silencioso (D11/D12), resolviendo los
 * participantes con la tabla de bracket (RF-RES-03) y aplicando cada resultado
 * con torneo_aplicar_resultado. Respeta los resultados parciales ya cargados.
 * Al finalizar quedan definidas las cuatro posiciones finales.
 *
 * @param pd    Puntero a la Pokédex cargada (no debe ser NULL).
 * @param t     Puntero al estado del torneo ya armado (no debe ser NULL).
 * @param reg   Puntero al registro de entrenadores (no debe ser NULL).
 * @param exito Puntero donde se escribe true si la simulación completó; false
 *              si los parámetros son inválidos, el torneo no está armado o
 *              ya finalizó, o faltó un equipo que no se pudo generar.
 */
void simulacion_ejecutar(const Pokedex *pd, Torneo *t,
                         RegistroEntrenadores *reg, bool *exito);

#endif /* SIMULACION_H */

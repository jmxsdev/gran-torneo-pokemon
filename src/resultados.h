/**
 * @file resultados.h
 * @brief Carga de resultados por teclado o archivo con validación completa
 *        (RF-RES-01, RF-RES-02, RF-RES-04).
 * @author Nombre del estudiante
 * @date 2026-09-01
 */

#ifndef RESULTADOS_H
#define RESULTADOS_H

#include <stdbool.h>

#include "torneo.h"

/**
 * @brief Carga resultados del torneo por teclado.
 *
 * Pide número de combate y resultado con lectura validada (RF-RES-01);
 * cada registro se valida contra el estado real del torneo (RF-RES-02)
 * antes de aplicarse con torneo_aplicar_resultado.
 *
 * @param t   Puntero al estado del torneo (no debe ser NULL).
 * @param reg Puntero al registro de entrenadores (no debe ser NULL).
 * @return true si al menos un resultado se aplicó; false si no se cargó
 *         ninguno o los parámetros son inválidos.
 */
bool resultados_cargar_teclado(Torneo *t, RegistroEntrenadores *reg);

/**
 * @brief Carga resultados desde un archivo de texto (RF-RES-01/RF-RES-04).
 *
 * Formato D8, separador ';'; admite parciales. Cada línea válida se aplica;
 * las inválidas se rechazan con mensaje y la carga continúa sin terminar
 * el programa.
 *
 * @param t    Puntero al estado del torneo (no debe ser NULL).
 * @param reg  Puntero al registro de entrenadores (no debe ser NULL).
 * @param ruta Ruta del archivo de resultados (p. ej. RUTA_RESULTADOS).
 * @return true si se aplicó al menos un resultado; false si el archivo no
 *         se pudo abrir, no contenía registros válidos o los parámetros
 *         son inválidos.
 */
bool resultados_cargar_archivo(Torneo *t, RegistroEntrenadores *reg,
                               const char *ruta);

/**
 * @brief Valida un resultado contra el estado real del torneo (RF-RES-02).
 *
 * Catálogo del diseño §8.1: combate en 1..64, estado de ronda, no
 * duplicado, entrenadores registrados, participantes resueltos por el
 * sistema (RF-RES-03), fuente del bracket disponible, ganador coherente,
 * sin empates en eliminatoria (RF-ELM-01) y KOs no negativos, con motivo
 * específico en msg.
 *
 * @param t   Puntero al estado del torneo (no debe ser NULL).
 * @param reg Puntero al registro de entrenadores (no debe ser NULL).
 * @param r   Puntero al resultado a validar (no debe ser NULL).
 * @param msg Buffer con el motivo del rechazo (no NULL).
 * @param n   Capacidad del buffer msg.
 * @return true si el resultado es válido; false en caso contrario.
 */
bool resultados_validar(const Torneo *t, const RegistroEntrenadores *reg,
                        const ResultadoCargado *r, char *msg, size_t n);

/**
 * @brief Muestra cuántos combates del torneo siguen pendientes (RF-RES-01).
 *
 * Informa los pendientes de grupos (1..48) y de eliminatoria (49..64),
 * útil tras cargar resultados parciales.
 *
 * @param t Puntero al estado del torneo (no debe ser NULL).
 */
void resultados_mostrar_pendientes(const Torneo *t);

#endif /* RESULTADOS_H */
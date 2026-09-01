/**
 * @file resultados.h
 * @brief Carga de resultados por teclado o archivo con validación completa
 *        (RF-RES-01, RF-RES-02, RF-RES-04).
 * @author <Nombre del estudiante>
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

#endif /* RESULTADOS_H */
/**
 * @file validacion.h
 * @brief Lectura segura de entradas con reintentos (RF-TEC-03).
 * @author <Nombre del estudiante>
 * @date 2026-09-01
 */

#ifndef VALIDACION_H
#define VALIDACION_H

#include <stddef.h>

/**
 * @brief Lee un entero del usuario y lo valida en un rango cerrado.
 *
 * Reintenta indefinidamente hasta recibir una entrada numérica dentro de
 * [min, max] o detectar EOF (cierre ordenado). Ninguna entrada inválida
 * termina el programa (RF-TEC-03).
 *
 * @param mensaje Texto que se muestra como prompt (no debe ser NULL).
 * @param min     Límite inferior aceptado (inclusive).
 * @param max     Límite superior aceptado (inclusive).
 * @return El entero validado dentro del rango; 0 si se detecta EOF.
 */
int validar_leer_entero(const char *mensaje, int min, int max);

/**
 * @brief Lee una cadena del usuario con longitud acotada.
 *
 * Reintenta hasta recibir una cadena no vacía o detectar EOF; nunca
 * desborda el buffer (se recorta a n-1 caracteres + NUL).
 *
 * @param mensaje Texto que se muestra como prompt (no debe ser NULL).
 * @param buf     Buffer donde se escribe la cadena (no debe ser NULL).
 * @param n       Capacidad del buffer en bytes (debe ser > 0).
 */
void validar_leer_cadena(const char *mensaje, char *buf, size_t n);

#endif /* VALIDACION_H */
/**
 * @file pantalla.h
 * @brief Helpers de presentación por consola con padding por caracteres
 *        visibles UTF-8 (D19, guía de estilo de pantalla).
 * @author Nombre del estudiante
 * @date 2026-11-16
 */

#ifndef PANTALLA_H
#define PANTALLA_H

/**
 * @brief Ancho visible de una cadena UTF-8 en columnas (D19).
 *
 * Cuenta un carácter visible por cada byte inicial UTF-8 (ignora los bytes de
 * continuación 0x80..0xBF), de modo que "Pokémon" mide 7 columnas y no 8.
 *
 * @param texto Cadena a medir (no debe ser NULL).
 * @return Cantidad de columnas visibles; 0 si texto es NULL.
 */
int pantalla_ancho(const char *texto);

/**
 * @brief Imprime un texto y completa con espacios hasta 'ancho' columnas.
 *
 * Alineación a la izquierda; el padding se calcula en columnas visibles, no
 * en bytes (D19).
 *
 * @param texto Cadena a imprimir (no debe ser NULL).
 * @param ancho Columnas totales de la columna (>= 0).
 */
void pantalla_pad(const char *texto, int ancho);

/**
 * @brief Imprime un texto centrado en 'ancho' columnas seguido de salto.
 *
 * @param texto Cadena a centrar (no debe ser NULL).
 * @param ancho Columnas totales de la línea (>= 0).
 */
void pantalla_centro(const char *texto, int ancho);

/**
 * @brief Imprime una regla de 'ancho' repeticiones del símbolo indicado.
 *
 * @param ancho   Columnas de la regla (>= 0).
 * @param simbolo Cadena repetida (p. ej. "═" o "─"), no NULL.
 */
void pantalla_linea(int ancho, const char *simbolo);

#endif /* PANTALLA_H */

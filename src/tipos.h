/**
 * @file tipos.h
 * @brief Catálogo de los 18 tipos Pokémon y su efectividad (RF-CMB-05).
 * @author Nombre del estudiante
 * @date 2026-09-01
 */

#ifndef TIPOS_H
#define TIPOS_H

#include <stdbool.h>

#include "constantes.h"

/**
 * @brief Identificador de tipo Pokémon (18 tipos de la 1.ª generación).
 *
 * TIPO_NINGUNO representa el segundo tipo ausente ("-") y es el tipo
 * centinela: nunca participa en la matriz de efectividad (RF-PDX-03).
 */
typedef enum {
    TIPO_NORMAL, TIPO_FUEGO, TIPO_AGUA, TIPO_PLANTA, TIPO_ELECTRICO,
    TIPO_HIELO, TIPO_LUCHA, TIPO_VENENO, TIPO_TIERRA, TIPO_VOLADOR,
    TIPO_PSIQUICO, TIPO_BICHO, TIPO_ROCA, TIPO_FANTASMA, TIPO_DRAGON,
    TIPO_SINIESTRO, TIPO_ACERO, TIPO_HADA,
    TIPO_NINGUNO   /* segundo tipo ausente y tipo de sentinela */
} Tipo;

/**
 * @brief Convierte el nombre textual de un tipo a su enumerado.
 *
 * Acepta el nombre exacto de los 18 tipos (p. ej. "Planta") y el guion "-"
 * como segundo tipo ausente. No modifica ningún estado del módulo.
 *
 * @param nombre Cadena con el nombre del tipo (no debe ser NULL).
 * @param salida Puntero donde se escribe el Tipo resultante (no debe ser NULL).
 * @return true si el nombre es uno de los 18 tipos o "-"; false en caso
 *         contrario, dejando *salida sin cambios.
 */
bool tipos_es_valido(const char *nombre, Tipo *salida);

/**
 * @brief Devuelve el texto en español de un tipo.
 *
 * @param t Tipo a convertir; TIPO_NINGUNO se acepta y devuelve "-".
 * @return Cadena estática con el nombre del tipo; nunca NULL.
 */
const char *tipos_a_texto(Tipo t);

/**
 * @brief Fija la matriz de efectividad 18x18 del módulo.
 *
 * Debe llamarse una vez antes de usar tipos_multiplicador. La matriz es
 * estática y vive en tipos.c (MINOR-1): no se expone en esta cabecera.
 */
void tipos_inicializar(void);

/**
 * @brief Calcula el multiplicador de un ataque contra un defensor de dos tipos.
 *
 * Producto de la efectividad del tipo atacante contra cada tipo del defensor:
 * el resultado pertenece a {0.25, 0.5, 1.0, 2.0, 4.0} (RF-CMB-05).
 *
 * @param ataque Tipo del ataque (nunca TIPO_NINGUNO).
 * @param def1  Primer tipo del defensor.
 * @param def2  Segundo tipo del defensor; TIPO_NINGUNO si tiene uno solo.
 * @return Multiplicador de daño calculado; 0.0f si la matriz no fue
 *         inicializada con tipos_inicializar.
 */
float tipos_multiplicador(Tipo ataque, Tipo def1, Tipo def2);

#endif /* TIPOS_H */
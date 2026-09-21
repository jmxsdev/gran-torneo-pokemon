/**
 * @file pokedex.h
 * @brief Pokédex: carga y consulta de las 150 especies (RF-PDX-01..06).
 * @author Nombre del estudiante
 * @date 2026-09-01
 */

#ifndef POKEDEX_H
#define POKEDEX_H

#include <stdbool.h>

#include "constantes.h"
#include "tipos.h"

/**
 * @brief Especie de la 1.ª generación (dato inmutable, cargado de archivo).
 *
 * Nunca se modifica en ejecución: el combate actúa sobre el ejemplar, jamás
 * sobre la especie (RF-PDX-04 / RF-EQP-03).
 */
typedef struct {
    int  numero;                 /* 1..150, único */
    char nombre[TAM_MAX_APODO];  /* el nombre más largo ("Farfetch'd") ocupa 10 */
    Tipo tipo_primario;
    Tipo tipo_secundario;        /* TIPO_NINGUNO si la especie es de un solo tipo */
    int  hp_base, ataque_base, defensa_base, velocidad_base;
} Especie;

/**
 * @brief Pokédex cargada: arreglo estático de especies y su conteo.
 *
 * cantidad == 150 tras una carga exitosa; 0 si la carga falló (RF-PDX-06).
 */
typedef struct {
    Especie especies[POKEDEX_MAX];
    int     cantidad;
} Pokedex;

/**
 * @brief Carga las 150 especies desde un archivo de texto.
 *
 * Parseo estricto (formato D8, separador ';'): exactamente 8 campos por
 * línea y 150 líneas no vacías. Ante cualquier desvío la Pokédex queda con
 * cantidad = 0 y el programa continúa (RF-PDX-06).
 *
 * @param pd   Puntero a la Pokédex que se completa (no debe ser NULL).
 * @param ruta Ruta del archivo de datos (p. ej. RUTA_POKEDEX).
 * @return true si se cargaron las 150 especies; false si el archivo no se
 *         pudo abrir, tiene formato inválido o no contiene 150 líneas.
 */
bool pokedex_cargar(Pokedex *pd, const char *ruta);

/**
 * @brief Busca una especie por su número en la Pokédex.
 *
 * No modifica ningún dato de la Pokédex (RF-PDX-05).
 *
 * @param pd     Puntero a la Pokédex cargada (no debe ser NULL).
 * @param numero Número de especie a buscar, en el rango 1..150.
 * @return Puntero constante a la especie encontrada; NULL si no existe o
 *         los parámetros son inválidos.
 */
const Especie *pokedex_buscar_numero(const Pokedex *pd, int numero);

/**
 * @brief Busca una especie por su nombre exacto en la Pokédex.
 *
 * Comparación de cadenas sin distinguir mayúsculas ni acentos (RF-PDX-05).
 *
 * @param pd     Puntero a la Pokédex cargada (no debe ser NULL).
 * @param nombre Nombre de la especie a buscar (no debe ser NULL).
 * @return Puntero constante a la especie encontrada; NULL si no existe o
 *         los parámetros son inválidos.
 */
const Especie *pokedex_buscar_nombre(const Pokedex *pd, const char *nombre);

/**
 * @brief Muestra por consola todas las especies de la Pokédex en orden.
 *
 * @param pd Puntero a la Pokédex cargada (no debe ser NULL).
 */
void pokedex_mostrar_todas(const Pokedex *pd);

/**
 * @brief Muestra por consola una especie (número, nombre y tipos).
 *
 * @param esp Puntero a la especie a mostrar (no debe ser NULL).
 */
void pokedex_mostrar_especie(const Especie *esp);

#endif /* POKEDEX_H */
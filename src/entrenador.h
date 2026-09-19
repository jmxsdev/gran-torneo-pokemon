/**
 * @file entrenador.h
 * @brief Entrenadores: registro con id único, búsqueda y listado (RF-ENT-01..02).
 * @author <Nombre del estudiante>
 * @date 2026-09-01
 */

#ifndef ENTRENADOR_H
#define ENTRENADOR_H

#include <stdbool.h>

#include "constantes.h"
#include "equipo.h"

/**
 * @brief Entrenador del torneo: datos personales, equipo y contadores.
 *
 * Los contadores de fase de grupos (victorias/empates/derrotas/puntos y
 * pokemon_derrotados) se reconstruyen desde data/resultados.txt; no se
 * persisten en data/entrenadores.txt (D8).
 *
 * El tag "struct Entrenador" completa la declaración adelantada de equipo.h
 * (evita conflicto de tipos al incluir ambas cabeceras).
 */
typedef struct Entrenador {
    int   id;                    /* único (RF-ENT-02) */
    char  nombre[TAM_MAX_NOMBRE];
    int   victorias, empates, derrotas;
    int   puntos;                /* 3/1/0 solo fase de grupos */
    int   pokemon_derrotados;    /* criterio 3 de desempate (RF-TRN-04) */
    Ejemplar *equipo;            /* cabeza de la lista enlazada */
} Entrenador;

/**
 * @brief Registro de entrenadores: arreglo estático y su conteo.
 *
 * capacidad MAX_ENTRENADORES (32); el torneo exige exactamente 32 para
 * armar grupos (D10).
 */
typedef struct {
    Entrenador entrenadores[MAX_ENTRENADORES];
    int cantidad;
} RegistroEntrenadores;

/**
 * @brief Registra un entrenador nuevo con id único (RF-ENT-02).
 *
 * Rechaza ids repetidos, ids fuera de rango y el desborde del registro.
 *
 * @param reg    Puntero al registro (no debe ser NULL).
 * @param id     Identificador único propuesto (entero > 0).
 * @param nombre Nombre del entrenador (no debe ser NULL).
 * @return true si se registró; false si el id ya existe, los parámetros
 *         son inválidos o el registro está lleno.
 */
bool entrenador_registrar(RegistroEntrenadores *reg, int id, const char *nombre);

/**
 * @brief Busca un entrenador por su id en el registro.
 *
 * @param reg Puntero al registro (no debe ser NULL).
 * @param id  Identificador a buscar.
 * @return Puntero al entrenador encontrado; NULL si no existe o el
 *         registro es inválido.
 */
Entrenador *entrenador_buscar(RegistroEntrenadores *reg, int id);

/**
 * @brief Muestra por consola todos los entrenadores del registro.
 *
 * @param reg Puntero al registro (no debe ser NULL).
 */
void entrenador_mostrar_todos(const RegistroEntrenadores *reg);

#endif /* ENTRENADOR_H */
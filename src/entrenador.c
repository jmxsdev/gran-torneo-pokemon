/**
 * @file entrenador.c
 * @brief Implementación del módulo de entrenadores: registro con id único,
 *        búsqueda y listado (RF-ENT-01..02).
 * @author <Nombre del estudiante>
 * @date 2026-09-19
 */

#include <stdio.h>
#include <string.h>

#include "entrenador.h"

/**
 * @brief Busca un entrenador por su id en el registro.
 *
 * Recorrido lineal sobre el arreglo estático (capacidad MAX_ENTRENADORES).
 *
 * @param reg Puntero al registro (no debe ser NULL).
 * @param id  Identificador a buscar.
 * @return Puntero al entrenador encontrado; NULL si no existe o el
 *         registro es inválido.
 */
Entrenador *entrenador_buscar(RegistroEntrenadores *reg, int id)
{
    int i;

    if (reg == NULL) {
        return NULL;
    }
    for (i = 0; i < reg->cantidad; i++) {
        if (reg->entrenadores[i].id == id) {
            return &reg->entrenadores[i];
        }
    }
    return NULL;
}

/**
 * @brief Registra un entrenador nuevo con id único (RF-ENT-02).
 *
 * Valida id positivo, nombre no vacío, id no repetido (búsqueda O(n)
 * previa) y capacidad del registro. El equipo queda vacío y los contadores
 * de torneo en cero (RF-ENT-01).
 *
 * @param reg    Puntero al registro (no debe ser NULL).
 * @param id     Identificador único propuesto (entero > 0).
 * @param nombre Nombre del entrenador (no debe ser NULL).
 * @return true si se registró; false si el id ya existe, los parámetros
 *         son inválidos o el registro está lleno.
 */
bool entrenador_registrar(RegistroEntrenadores *reg, int id, const char *nombre)
{
    Entrenador *ent;

    if (reg == NULL || nombre == NULL) {
        return false;
    }
    if (id <= 0) {
        return false;
    }
    if (nombre[0] == '\0') {
        return false;
    }
    if (reg->cantidad >= MAX_ENTRENADORES) {
        return false;
    }
    if (entrenador_buscar(reg, id) != NULL) {
        return false;   /* id duplicado: MUST NOT existir dos iguales */
    }

    ent = &reg->entrenadores[reg->cantidad];
    ent->id = id;
    snprintf(ent->nombre, sizeof(ent->nombre), "%s", nombre);
    ent->victorias = 0;
    ent->empates = 0;
    ent->derrotas = 0;
    ent->puntos = 0;
    ent->pokemon_derrotados = 0;
    ent->equipo = NULL;
    reg->cantidad++;
    return true;
}

/**
 * @brief Muestra por consola todos los entrenadores del registro.
 *
 * Incluye id, nombre, contadores de fase de grupos y si el entrenador ya
 * tiene equipo (RF-ENT-01).
 *
 * @param reg Puntero al registro (no debe ser NULL).
 */
void entrenador_mostrar_todos(const RegistroEntrenadores *reg)
{
    int i;

    if (reg == NULL) {
        return;
    }
    if (reg->cantidad == 0) {
        printf("No hay entrenadores registrados.\n");
        return;
    }

    printf("--- Entrenadores registrados (%d/%d) ---\n",
           reg->cantidad, MAX_ENTRENADORES);
    for (i = 0; i < reg->cantidad; i++) {
        const Entrenador *ent = &reg->entrenadores[i];
        printf("id %3d  %-20s  G:%d E:%d P:%d  Puntos:%d  Equipo:%s\n",
               ent->id, ent->nombre, ent->victorias, ent->empates,
               ent->derrotas, ent->puntos,
               (ent->equipo != NULL) ? "sí" : "no");
    }
}
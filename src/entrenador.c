/**
 * @file entrenador.c
 * @brief Implementación del módulo de entrenadores: registro con id único,
 *        búsqueda y listado (RF-ENT-01..02).
 * @author Nombre del estudiante
 * @date 2026-09-19
 */

#include <stdio.h>
#include <string.h>

#include "entrenador.h"

/* Implementación de entrenador_buscar: documentación canónica en entrenador.h. */
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

/* Implementación de entrenador_registrar: documentación canónica en entrenador.h. */
void entrenador_registrar(RegistroEntrenadores *reg, int id, const char *nombre,
                          bool *exito)
{
    Entrenador *ent;

    if (reg == NULL || nombre == NULL) {
        *exito = false;
        return;
    }
    if (id <= 0 || nombre[0] == '\0' ||
        reg->cantidad >= MAX_ENTRENADORES) {
        *exito = false;
        return;
    }
    if (entrenador_buscar(reg, id) != NULL) {
        *exito = false;   /* id duplicado: MUST NOT existir dos iguales */
        return;
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
    *exito = true;
}

/* Implementación de entrenador_mostrar_todos: documentación canónica en entrenador.h. */
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
        printf("id %3d  %-20s  G:%d E:%d P:%d  Puntos:%d  Equipo:%d/%d\n",
               ent->id, ent->nombre, ent->victorias, ent->empates,
               ent->derrotas, ent->puntos, equipo_contar(ent), MAX_EQUIPO);
    }
}
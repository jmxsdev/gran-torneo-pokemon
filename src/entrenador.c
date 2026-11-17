/* Jesus Mora - CI 28480557 */
/**
 * @file entrenador.c
 * @brief Implementación del módulo de entrenadores: registro con id único,
 * búsqueda y listado.
 * @author Jesus Mora
 * @date 2026-09-19
 */

#include <stdio.h>
#include <string.h>

#include "entrenador.h"
#include "pantalla.h"

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

/* Anchos de la tabla de entrenadores (columnas visibles UTF-8). */
#define ENT_ANCHO_ID     3
#define ENT_ANCHO_NOMBRE 18
#define ENT_ANCHO_EQUIPO 6
#define ENT_ANCHO_TOTAL  (1 + ENT_ANCHO_ID + 2 + ENT_ANCHO_NOMBRE + 2 + 4 + \
                          2 + 4 + 2 + 4 + 2 + 7 + 2 + ENT_ANCHO_EQUIPO)

/* Implementación de entrenador_mostrar_todos: documentación canónica en entrenador.h. */
void entrenador_mostrar_todos(const RegistroEntrenadores *reg)
{
    int i;
    char titulo[64];

    if (reg == NULL) {
        return;
    }
    if (reg->cantidad == 0) {
        printf("No hay entrenadores registrados.\n");
        return;
    }
    snprintf(titulo, sizeof(titulo), "ENTRENADORES REGISTRADOS (%d/%d)",
             reg->cantidad, MAX_ENTRENADORES);
    pantalla_linea(ENT_ANCHO_TOTAL, "═");
    pantalla_centro(titulo, ENT_ANCHO_TOTAL);
    pantalla_linea(ENT_ANCHO_TOTAL, "═");
    printf(" %3s  ", "ID");
    pantalla_pad("Nombre", ENT_ANCHO_NOMBRE);
    printf("  %4s  %4s  %4s  %7s  ", "V", "E", "D", "Puntos");
    pantalla_pad("Equipo", ENT_ANCHO_EQUIPO);
    printf("\n");
    pantalla_linea(ENT_ANCHO_TOTAL, "─");
    for (i = 0; i < reg->cantidad; i++) {
        const Entrenador *ent = &reg->entrenadores[i];
        char equipo_txt[8];

        snprintf(equipo_txt, sizeof(equipo_txt), "%d/%d",
                 equipo_contar(ent), MAX_EQUIPO);
        printf(" %3d  ", ent->id);
        pantalla_pad(ent->nombre, ENT_ANCHO_NOMBRE);
        printf("  %4d  %4d  %4d  %7d  ", ent->victorias, ent->empates,
               ent->derrotas, ent->puntos);
        pantalla_pad(equipo_txt, ENT_ANCHO_EQUIPO);
        printf("\n");
    }
    pantalla_linea(ENT_ANCHO_TOTAL, "─");
}
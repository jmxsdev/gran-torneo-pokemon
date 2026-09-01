/**
 * @file equipo.h
 * @brief Ejemplares y equipos: creación desde especie, validación y
 *        formación automática por backtracking (RF-EQP-01..05).
 * @author <Nombre del estudiante>
 * @date 2026-09-01
 */

#ifndef EQUIPO_H
#define EQUIPO_H

#include <stdbool.h>

#include "constantes.h"
#include "pokedex.h"

/* Declaración adelantada: equipo.h es autónomo y NO incluye entrenador.h
   (evita ciclo de cabeceras; entrenador.h incluye equipo.h). El struct
   completo de Entrenador vive en entrenador.h. */
typedef struct Entrenador Entrenador;

/**
 * @brief Ejemplar: Pokémon concreto de un entrenador (dato mutable).
 *
 * Deriva sus estadísticas de la especie en la creación (D2) y copia sus
 * tipos para que el combate no consulte la Pokédex. hp_actual es el ÚNICO
 * campo que muta durante un combate (RF-EQP-03).
 */
typedef struct Ejemplar {
    int   id;                    /* único global, monótono */
    int   numero_especie;        /* 1..150 — referencia, NO copia propietaria */
    char  nombre[TAM_MAX_APODO]; /* apodo del ejemplar */
    int   nivel;                 /* NIVEL_MIN..NIVEL_MAX */
    int   hp_max, ataque, defensa, velocidad;   /* derivados de la especie (D2) */
    int   hp_actual;             /* único campo que muta en combate */
    Tipo  tipo_primario, tipo_secundario;       /* copia: combate no consulta la Pokédex */
    struct Ejemplar *siguiente;  /* TDA lista enlazada del equipo */
} Ejemplar;

/**
 * @brief Restricciones de la formación automática de equipos (RF-EQP-04).
 */
typedef struct {
    int  cantidad;               /* n buscado, 1..MAX_EQUIPO */
    int  nivel_total_max;
    int  min_tipos;              /* tipos distintos mínimos */
    int  ataque_total_min;       /* opcional (stats objetivo); 0 = sin restricción */
    bool permitir_repetidas;     /* especies repetidas dentro del equipo */
} RestriccionesEquipo;

/**
 * @brief Crea un ejemplar nuevo a partir de una especie y un nivel.
 *
 * Deriva hp/ataque/defensa/velocidad con la fórmula D2
 * (stat = stat_base + nivel) y copia los tipos de la especie (RF-EQP-02).
 * El ejemplar se asigna con malloc: el llamador es dueño de la memoria.
 *
 * @param esp   Puntero a la especie base (no debe ser NULL).
 * @param nivel Nivel del ejemplar, en el rango NIVEL_MIN..NIVEL_MAX.
 * @param apodo Nombre/apodo del ejemplar (no debe ser NULL).
 * @param id    Identificador único global del ejemplar.
 * @return Puntero al ejemplar creado (hp_actual = hp_max); NULL si los
 *         parámetros son inválidos o falla la reserva de memoria.
 */
Ejemplar *equipo_crear_ejemplar(const Especie *esp, int nivel,
                                const char *apodo, int id);

/**
 * @brief Agrega un ejemplar al equipo del entrenador respetando MAX_EQUIPO.
 *
 * Inserta el ejemplar al inicio de la lista enlazada del entrenador.
 *
 * @param ent   Puntero al entrenador (no debe ser NULL).
 * @param nuevo Ejemplar ya creado que pasa a ser propiedad del equipo.
 * @return true si se agregó; false si el equipo ya tiene MAX_EQUIPO
 *         ejemplares o los parámetros son inválidos.
 */
bool equipo_agregar_ejemplar(Entrenador *ent, Ejemplar *nuevo);

/**
 * @brief Valida que el equipo del entrenador cumpla las reglas del torneo.
 *
 * Comprueba el tamaño requerido (1..MAX_EQUIPO; el torneo usa
 * TAM_EQUIPO_TORNEO), especies existentes, niveles y tipos válidos
 * (RF-EQP-05).
 *
 * @param ent             Puntero al entrenador (no debe ser NULL).
 * @param tamano_requerido Tamaño que debe cumplir el equipo (1..MAX_EQUIPO).
 * @return true si el equipo es válido; false en caso contrario.
 */
bool equipo_validar(const Entrenador *ent, int tamano_requerido);

/**
 * @brief Forma un equipo automáticamente con recursividad + backtracking.
 *
 * Explora combinaciones de especies hasta cumplir TODAS las restricciones
 * de *r (RF-EQP-04). La lista resultante se crea con malloc y es propiedad
 * del llamador; ante "no existe solución" devuelve false sin equipo parcial.
 *
 * @param pd       Puntero a la Pokédex cargada (no debe ser NULL).
 * @param r        Puntero a las restricciones (no debe ser NULL).
 * @param salida   Puntero donde se escribe la cabeza de la lista creada
 *                 (no debe ser NULL).
 * @param cantidad Puntero donde se escribe cuántos ejemplares se crearon.
 * @return true si existe un equipo que cumple todas las restricciones;
 *         false si no existe solución o los parámetros son inválidos.
 */
bool equipo_formar_backtracking(const Pokedex *pd, const RestriccionesEquipo *r,
                                Ejemplar **salida, int *cantidad);

#endif /* EQUIPO_H */
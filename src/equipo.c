/**
 * @file equipo.c
 * @brief Implementación del módulo de equipos: creación de ejemplares desde
 *        especie (D2), agregación, validación, liberación y consulta
 *        (RF-EQP-01..05).
 * @author <Nombre del estudiante>
 * @date 2026-09-19
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "equipo.h"
#include "entrenador.h"   /* struct Entrenador completo (equipo.h solo adelanta) */

/* Contador global de ids de ejemplar: único y monótono (diseño §2 y §5.2).
   Al recargar data/entrenadores.txt se fija al máximo id visto + 1. */
static int contador_id_ejemplar = 0;

/**
 * Devuelve el siguiente id de ejemplar (único global, monótono).
 */
int equipo_siguiente_id(void)
{
    return ++contador_id_ejemplar;
}

/**
 * Fija el contador global de ids al máximo visto; solo sube.
 */
void equipo_fijar_contador_id(int id_maximo)
{
    if (id_maximo > contador_id_ejemplar) {
        contador_id_ejemplar = id_maximo;
    }
}

/**
 * @brief Crea un ejemplar nuevo a partir de una especie de la Pokédex.
 *
 * Fórmula D2 exacta (variación determinista única por ejemplar, división
 * entera truncada de C): variacion = (id * 7) % 16; hp_max = (hp_base *
 * nivel / 50) + nivel + 10 + variacion; stat = (stat_base * nivel / 50) +
 * nivel + 5 + variacion; hp_actual = hp_max. La especie NO se muta
 * (RF-EQP-03). El ejemplar se asigna con malloc.
 *
 * @param pd             Puntero a la Pokédex cargada (no debe ser NULL).
 * @param numero_especie Número de la especie base, 1..POKEDEX_MAX.
 * @param nombre         Nombre/apodo del ejemplar (no debe ser NULL).
 * @param nivel          Nivel del ejemplar, en el rango NIVEL_MIN..NIVEL_MAX.
 * @param id_ejemplar    Identificador único global del ejemplar (> 0).
 * @return Puntero al ejemplar creado (hp_actual = hp_max); NULL si la
 *         especie no existe, los parámetros son inválidos o falla malloc.
 */
Ejemplar *equipo_crear_ejemplar(const Pokedex *pd, int numero_especie,
                                const char *nombre, int nivel, int id_ejemplar)
{
    const Especie *esp;
    Ejemplar *ej;
    int variacion;

    if (pd == NULL || nombre == NULL || id_ejemplar <= 0) {
        return NULL;
    }
    if (nivel < NIVEL_MIN || nivel > NIVEL_MAX) {
        return NULL;
    }
    esp = pokedex_buscar_numero(pd, numero_especie);
    if (esp == NULL) {
        return NULL;   /* RF-EQP-01: la especie no pertenece a la Pokédex */
    }
    ej = (Ejemplar *)malloc(sizeof(Ejemplar));
    if (ej == NULL) {
        return NULL;
    }
    variacion = (id_ejemplar * 7) % 16;
    ej->id = id_ejemplar;
    ej->numero_especie = numero_especie;
    snprintf(ej->nombre, sizeof(ej->nombre), "%s", nombre);
    ej->nivel = nivel;
    ej->hp_max = (esp->hp_base * nivel) / 50 + nivel + 10 + variacion;
    ej->ataque = (esp->ataque_base * nivel) / 50 + nivel + 5 + variacion;
    ej->defensa = (esp->defensa_base * nivel) / 50 + nivel + 5 + variacion;
    ej->velocidad = (esp->velocidad_base * nivel) / 50 + nivel + 5 + variacion;
    ej->hp_actual = ej->hp_max;
    ej->tipo_primario = esp->tipo_primario;
    ej->tipo_secundario = esp->tipo_secundario;
    ej->siguiente = NULL;
    return ej;
}

/**
 * Cuenta los ejemplares del equipo (recorrido de la lista enlazada).
 */
int equipo_contar(const Entrenador *ent)
{
    const Ejemplar *ej;
    int n = 0;

    if (ent == NULL) {
        return 0;
    }
    for (ej = ent->equipo; ej != NULL; ej = ej->siguiente) {
        n++;
    }
    return n;
}

/**
 * @brief Agrega un ejemplar al equipo respetando MAX_EQUIPO (RF-EQP-05).
 *
 * Inserta al inicio de la lista enlazada (TDA encapsulado: solo este módulo
 * toca el campo siguiente).
 *
 * @param ent   Puntero al entrenador (no debe ser NULL).
 * @param nuevo Ejemplar ya creado que pasa a ser propiedad del equipo.
 * @return true si se agregó; false si el equipo ya tiene MAX_EQUIPO
 *         ejemplares o los parámetros son inválidos.
 */
bool equipo_agregar_ejemplar(Entrenador *ent, Ejemplar *nuevo)
{
    if (ent == NULL || nuevo == NULL) {
        return false;
    }
    if (equipo_contar(ent) >= MAX_EQUIPO) {
        return false;   /* RF-EQP-05: tamaño máximo 6 */
    }
    nuevo->siguiente = ent->equipo;
    ent->equipo = nuevo;
    return true;
}

/**
 * @brief Valida el equipo del entrenador (RF-EQP-05).
 *
 * Comprueba tamaño (1..tamano_requerido), existencia de cada especie en la
 * Pokédex (RF-EQP-01), niveles (1..100) y tipos copiados en el ejemplar.
 *
 * @param pd               Puntero a la Pokédex cargada (no debe ser NULL).
 * @param ent              Puntero al entrenador (no debe ser NULL).
 * @param tamano_requerido Tamaño máximo admitido (1..MAX_EQUIPO).
 * @return true si el equipo es válido; false en caso contrario.
 */
bool equipo_validar(const Pokedex *pd, const Entrenador *ent,
                    int tamano_requerido)
{
    const Ejemplar *ej;
    int n;

    if (pd == NULL || ent == NULL) {
        return false;
    }
    if (tamano_requerido < 1 || tamano_requerido > MAX_EQUIPO) {
        return false;
    }
    n = equipo_contar(ent);
    if (n < 1 || n > tamano_requerido) {
        return false;   /* tamaño fuera del rango permitido 1..6 */
    }
    for (ej = ent->equipo; ej != NULL; ej = ej->siguiente) {
        if (pokedex_buscar_numero(pd, ej->numero_especie) == NULL ||
            ej->nivel < NIVEL_MIN || ej->nivel > NIVEL_MAX ||
            ej->tipo_primario >= TIPO_NINGUNO ||
            ej->tipo_secundario > TIPO_NINGUNO) {
            return false;   /* especie, nivel o tipos inválidos */
        }
    }
    return true;
}

/**
 * @brief Libera todos los ejemplares del equipo (free disciplinado).
 *
 * Recorre la lista liberando cada nodo con free; deja ent->equipo en NULL.
 *
 * @param ent Puntero al entrenador cuyo equipo se libera (no debe ser NULL).
 */
void equipo_liberar(Entrenador *ent)
{
    Ejemplar *ej;

    if (ent == NULL) {
        return;
    }
    ej = ent->equipo;
    while (ej != NULL) {
        Ejemplar *siguiente = ej->siguiente;
        free(ej);
        ej = siguiente;
    }
    ent->equipo = NULL;
}

/**
 * @brief Muestra por consola el equipo completo de un entrenador.
 *
 * @param pd  Puntero a la Pokédex cargada (no debe ser NULL).
 * @param ent Puntero al entrenador (no debe ser NULL).
 */
void equipo_mostrar(const Pokedex *pd, const Entrenador *ent)
{
    const Ejemplar *ej;
    int n;

    if (ent == NULL) {
        return;
    }
    n = equipo_contar(ent);
    if (n == 0) {
        printf("El entrenador %s (id %d) no tiene equipo.\n",
               ent->nombre, ent->id);
        return;
    }
    printf("--- Equipo de %s (id %d): %d ejemplar(es) ---\n",
           ent->nombre, ent->id, n);
    for (ej = ent->equipo; ej != NULL; ej = ej->siguiente) {
        const Especie *esp = pokedex_buscar_numero(pd, ej->numero_especie);
        printf("  #%d %s  (especie #%d %s, nivel %d)\n",
               ej->id, ej->nombre, ej->numero_especie,
               (esp != NULL) ? esp->nombre : "?",
               ej->nivel);
        printf("      HP %d/%d  Ataque %d  Defensa %d  Velocidad %d  %s/%s\n",
               ej->hp_actual, ej->hp_max, ej->ataque, ej->defensa,
               ej->velocidad, tipos_a_texto(ej->tipo_primario),
               tipos_a_texto(ej->tipo_secundario));
    }
}
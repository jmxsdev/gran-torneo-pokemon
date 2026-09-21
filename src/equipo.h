/**
 * @file equipo.h
 * @brief Ejemplares y equipos: creación desde especie, agregación,
 *        validación, liberación y consulta (RF-EQP-01..05).
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
   completo de Entrenador vive en entrenador.h con el tag "struct Entrenador"
   para completar esta declaración. */
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
 *
 * Extiende el diseño §2 con la lista de especies permitidas (restricción de
 * integrantes pedida en el contrato F4.1): si cantidad_permitidas es 0, todas
 * las especies de la Pokédex son candidatas.
 */
typedef struct {
    int  cantidad;               /* n buscado, 1..MAX_EQUIPO */
    int  nivel_total_max;        /* suma de niveles de los ejemplares */
    int  min_tipos;              /* tipos distintos mínimos, 1..CANT_TIPOS */
    int  ataque_total_min;       /* stats objetivo; 0 = sin restricción */
    bool permitir_repetidas;     /* especies repetidas dentro del equipo */
    int  especies_permitidas[POKEDEX_MAX]; /* números de especie admitidos */
    int  cantidad_permitidas;    /* 0 = todas las especies permitidas */
} RestriccionesEquipo;

/**
 * @brief Devuelve el siguiente id de ejemplar (único global y monótono).
 *
 * El contador vive en equipo.c; al recargar datos desde archivo se fija al
 * máximo id visto con equipo_fijar_contador_id (diseño §5.2).
 *
 * @return Un id de ejemplar no usado antes en la sesión.
 */
int equipo_siguiente_id(void);

/**
 * @brief Fija el contador global de ids de ejemplar al valor indicado.
 *
 * Solo sube el contador: si id_maximo es menor o igual al valor actual, no
 * hace nada. Se usa tras cargar data/entrenadores.txt para que los nuevos
 * ejemplares no colisionen con los del archivo.
 *
 * @param id_maximo Máximo id de ejemplar visto en la carga.
 */
void equipo_fijar_contador_id(int id_maximo);

/**
 * @brief Crea un ejemplar nuevo a partir de una especie de la Pokédex.
 *
 * Valida que la especie exista (RF-EQP-01) y el nivel (RF-EQP-05). Deriva
 * las stats con la fórmula D2 exacta (división entera truncada de C, en ese
 * orden): variacion = (id_ejemplar * 7) % 16; hp_max = (hp_base * nivel /
 * 50) + nivel + 10 + variacion; stat = (stat_base * nivel / 50) + nivel + 5
 * + variacion para ataque, defensa y velocidad; hp_actual = hp_max. La
 * especie NO se muta: se copian nombre, tipos y stats (RF-EQP-03). El
 * ejemplar se asigna con malloc: el llamador es dueño de la memoria.
 *
 * @param pd            Puntero a la Pokédex cargada (no debe ser NULL).
 * @param numero_especie Número de la especie base, 1..POKEDEX_MAX.
 * @param nombre        Nombre/apodo del ejemplar (no debe ser NULL).
 * @param nivel         Nivel del ejemplar, en el rango NIVEL_MIN..NIVEL_MAX.
 * @param id_ejemplar   Identificador único global del ejemplar (> 0).
 * @return Puntero al ejemplar creado (hp_actual = hp_max); NULL si la
 *         especie no existe, los parámetros son inválidos o falla malloc.
 */
Ejemplar *equipo_crear_ejemplar(const Pokedex *pd, int numero_especie,
                                const char *nombre, int nivel, int id_ejemplar);

/**
 * @brief Cuenta los ejemplares del equipo de un entrenador.
 *
 * @param ent Puntero al entrenador (no debe ser NULL).
 * @return Cantidad de ejemplares de la lista enlazada; 0 si es NULL.
 */
int equipo_contar(const Entrenador *ent);

/**
 * @brief Agrega un ejemplar al equipo del entrenador respetando MAX_EQUIPO.
 *
 * Inserta al inicio de la lista enlazada del entrenador.
 *
 * @param ent   Puntero al entrenador (no debe ser NULL).
 * @param nuevo Ejemplar ya creado que pasa a ser propiedad del equipo.
 * @return true si se agregó; false si el equipo ya tiene MAX_EQUIPO
 *         ejemplares o los parámetros son inválidos.
 */
bool equipo_agregar_ejemplar(Entrenador *ent, Ejemplar *nuevo);

/**
 * @brief Valida que el equipo del entrenador cumpla las reglas (RF-EQP-05).
 *
 * Comprueba el tamaño (1..tamano_requerido), la existencia de cada especie
 * en la Pokédex (RF-EQP-01), los niveles (1..100) y los tipos copiados.
 *
 * @param pd               Puntero a la Pokédex cargada (no debe ser NULL).
 * @param ent              Puntero al entrenador (no debe ser NULL).
 * @param tamano_requerido Tamaño máximo admitido (1..MAX_EQUIPO).
 * @return true si el equipo es válido; false en caso contrario.
 */
bool equipo_validar(const Pokedex *pd, const Entrenador *ent,
                    int tamano_requerido);

/**
 * @brief Libera todos los ejemplares del equipo de un entrenador.
 *
 * Recorre la lista enlazada liberando cada nodo con free y deja el equipo
 * en NULL. No libera al entrenador ni su nombre.
 *
 * @param ent Puntero al entrenador cuyo equipo se libera (no debe ser NULL).
 */
void equipo_liberar(Entrenador *ent);

/**
 * @brief Reemplaza el equipo de un entrenador por una lista nueva.
 *
 * Libera el equipo anterior (si existe) y asigna la lista indicada como
 * equipo del entrenador. La lista pasa a ser propiedad del entrenador y se
 * liberará con equipo_liberar. Encapsula el TDA: solo este módulo toca el
 * campo siguiente de los ejemplares.
 *
 * @param ent    Puntero al entrenador (no debe ser NULL).
 * @param equipo Cabeza de la lista a asignar (puede ser NULL para vaciar).
 */
void equipo_asignar(Entrenador *ent, Ejemplar *equipo);

/**
 * @brief Muestra por consola el equipo completo de un entrenador.
 *
 * Solo lectura: no modifica ningún dato (RF-EQP-03).
 *
 * @param pd  Puntero a la Pokédex cargada (no debe ser NULL).
 * @param ent Puntero al entrenador (no debe ser NULL).
 */
void equipo_mostrar(const Pokedex *pd, const Entrenador *ent);

/**
 * @brief Forma un equipo automáticamente con recursividad + backtracking.
 *
 * Explora combinaciones de especies hasta cumplir TODAS las restricciones
 * de *r (RF-EQP-04). Recursión real sobre el índice de especie (diseño
 * §4): en cada nodo decide cuántas copias de la especie actual incluir
 * (0..restantes, según permitir_repetidas) y retrocede ante parciales que
 * ya no pueden alcanzar la solución. Podas del diseño §4.3: cantidad
 * (restantes > especies disponibles), cota inferior de nivel (nivel_acum +
 * restantes*NIVEL_MIN > nivel_total_max), cota de tipos con sufijo
 * precomputado y cota por ejemplares restantes (un ejemplar aporta a lo
 * sumo 2 tipos). Salida temprana en el primer éxito. La lista resultante
 * se crea con malloc y es propiedad del llamador; ante "no existe
 * solución" devuelve false y deja *salida en NULL (spec RF-EQP-04: no se
 * devuelve un equipo inválido).
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
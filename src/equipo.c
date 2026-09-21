/**
 * @file equipo.c
 * @brief Implementación del módulo de equipos: creación de ejemplares desde
 *        especie (D2), agregación, validación, liberación y consulta
 *        (RF-EQP-01..05).
 * @author Nombre del estudiante
 * @date 2026-09-19
 */

#include <stdio.h>
#include <stdint.h>
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

/* Implementación de equipo_crear_ejemplar: documentación canónica en equipo.h. */
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

/* Implementación de equipo_agregar_ejemplar: documentación canónica en equipo.h. */
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

/* Implementación de equipo_validar: documentación canónica en equipo.h. */
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

/* Implementación de equipo_liberar: documentación canónica en equipo.h. */
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

/* Implementación de equipo_mostrar: documentación canónica en equipo.h. */
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

/**
 * Reemplaza el equipo de un entrenador por una lista nueva (libera la previa).
 */
void equipo_asignar(Entrenador *ent, Ejemplar *equipo)
{
    if (ent == NULL) {
        return;
    }
    if (ent->equipo != NULL) {
        equipo_liberar(ent);
    }
    ent->equipo = equipo;
}

/* ------------------------------------------------------------------ */
/* Formación automática por recursividad + backtracking (RF-EQP-04).  */
/* Diseño §4: podas de cantidad, cota inferior de nivel, cota de tipos */
/* con sufijo precomputado, restricciones de integrantes y salida      */
/* temprana; cada ejemplar descartado se libera al retroceder (§4.4).  */
/* ------------------------------------------------------------------ */

/**
 * Cuenta los bits en 1 de una máscara de tipos (popcount, máx 18 bits).
 */
static int bt_contar_bits(uint32_t v)
{
    int n = 0;
    while (v != 0) {
        v &= v - 1;
        n++;
    }
    return n;
}

/**
 * Verifica la restricción de integrantes: especie admitida por la lista.
 */
static bool bt_especie_admitida(const RestriccionesEquipo *r, int numero)
{
    int n = (r->cantidad_permitidas > POKEDEX_MAX) ? POKEDEX_MAX
                                                   : r->cantidad_permitidas;
    int i;

    if (n <= 0) {
        return true;   /* sin lista: todas las especies son candidatas */
    }
    for (i = 0; i < n; i++) {
        if (r->especies_permitidas[i] == numero) {
            return true;
        }
    }
    return false;
}

/**
 * Invierte una lista enlazada de ejemplares (solución en orden natural).
 */
static void bt_invertir_lista(Ejemplar **lista)
{
    Ejemplar *previo = NULL;
    Ejemplar *actual = *lista;

    while (actual != NULL) {
        Ejemplar *siguiente = actual->siguiente;
        actual->siguiente = previo;
        previo = actual;
        actual = siguiente;
    }
    *lista = previo;
}

/**
 * @brief Recursión pura del backtracking sobre el índice de especie.
 *
 * En cada nodo decide cuántas copias k de la especie i incluir (1..k_max,
 * donde k_max = restantes si se permiten repetidas y 1 si no) y luego
 * excluirla; el primer éxito (salida temprana §4.3.5) se propaga true.
 * Aplica las podas §4.3 antes de ramificar: 1) cantidad, 2) cota inferior
 * de nivel, 3) cota de tipos con el sufijo precomputado y la máscara
 * actual, y la cota extra 3b (un ejemplar aporta a lo sumo 2 tipos).
 * Los ejemplares creados que no conducen a solución se liberan al
 * retroceder (§4.4): la lista parcial queda intacta para el llamador.
 *
 * @param pd           Pokédex cargada (no debe ser NULL).
 * @param r            Restricciones a cumplir (no debe ser NULL).
 * @param tipos_sufijo Tabla sufijo: tipos alcanzables desde cada índice.
 * @param i            Índice de especie en curso (0..cantidad).
 * @param restantes    Ejemplares aún por incluir (0..r->cantidad).
 * @param nivel_acum   Suma de niveles del equipo parcial.
 * @param tipos_dist   Tipos distintos presentes en el parcial.
 * @param ataque_acum  Suma de ataque derivado del parcial.
 * @param mascara      Máscara de bits de los tipos vistos (reemplaza el
 *                     arreglo bool del diseño §4.1; §4.2 habla de máscara).
 * @param parcial      Puntero a la cabeza de la lista parcial.
 * @param creados      Contador de ejemplares vivos del parcial.
 * @return true si el parcial se completó cumpliendo todas las restricciones.
 */
static bool bt_rec(const Pokedex *pd, const RestriccionesEquipo *r,
                   const uint32_t *tipos_sufijo, int i,
                   int restantes, int nivel_acum, int tipos_dist,
                   int ataque_acum, uint32_t mascara,
                   Ejemplar **parcial, int *creados)
{
    const Especie *esp;
    int k_max;
    int k;

    /* Caso base de éxito: equipo completo que cumple TODAS las restricciones. */
    if (restantes == 0) {
        return tipos_dist >= r->min_tipos &&
               ataque_acum >= r->ataque_total_min;
    }
    /* Caso base de fracaso: se agotaron las especies candidatas. */
    if (i >= pd->cantidad) {
        return false;
    }

    /* Poda 1 (cantidad, §4.3.1): no quedan especies suficientes para el
       equipo (se usa pd->cantidad como límite real de candidatas). */
    if (restantes > pd->cantidad - i) {
        return false;
    }
    /* Poda 2 (cota inferior de nivel, §4.3.2): cada ejemplar cuesta al
       menos NIVEL_MIN; si el presupuesto ya no alcanza, retroceder. Hace
       que "6 Pokémon con nivel total máximo 5" falle en profundidad 1. */
    if (nivel_acum + restantes * NIVEL_MIN > r->nivel_total_max) {
        return false;
    }
    /* Poda 3 (cota de tipos, §4.3.3): los tipos aún alcanzables desde i
       (sufijo menos la máscara actual) no bastan para min_tipos. */
    if (tipos_dist + bt_contar_bits(tipos_sufijo[i] & ~mascara)
        < r->min_tipos) {
        return false;
    }
    /* Poda 3b (cota extra): un ejemplar aporta a lo sumo 2 tipos nuevos;
       evita que min_tipos inalcanzable explote el árbol (§4.5 peor caso). */
    if (tipos_dist + 2 * restantes < r->min_tipos) {
        return false;
    }

    esp = &pd->especies[i];

    /* Restricción de integrantes (§4.3.4, aplicada al generar el candidato):
       si la especie no está admitida por la lista, solo queda excluirla. */
    if (!bt_especie_admitida(r, esp->numero)) {
        return bt_rec(pd, r, tipos_sufijo, i + 1, restantes, nivel_acum,
                      tipos_dist, ataque_acum, mascara, parcial, creados);
    }

    /* Ramificar sobre el número k de copias de la especie i (integrante
       admitido por la lista de especies permitidas). */
    k_max = r->permitir_repetidas ? restantes : 1;
    for (k = 1; k <= k_max; k++) {
        int niv = nivel_acum;
        int atq = ataque_acum;
        int tips = tipos_dist;
        uint32_t masc = mascara;
        int j;

        for (j = 0; j < k; j++) {
            int nivel_max_ej;
            int nivel;
            uint32_t bits;
            Ejemplar *ej;

            /* Nivel del ejemplar (diseño §4.2): mínimo factible dentro del
               presupuesto restante; con objetivo de ataque se escala al
               máximo que el presupuesto permite (superset determinista del
               criterio "mejor base", sin estado adicional). */
            nivel_max_ej = r->nivel_total_max - niv
                           - (restantes - j - 1) * NIVEL_MIN;
            if (nivel_max_ej > NIVEL_MAX) {
                nivel_max_ej = NIVEL_MAX;
            }
            if (nivel_max_ej < NIVEL_MIN) {
                nivel_max_ej = NIVEL_MIN;   /* defensivo: poda 2 lo garantiza */
            }
            nivel = (r->ataque_total_min > 0) ? nivel_max_ej : NIVEL_MIN;

            ej = equipo_crear_ejemplar(pd, esp->numero, esp->nombre, nivel,
                                       equipo_siguiente_id());
            if (ej == NULL) {
                break;   /* sin memoria: no se forma esta rama */
            }
            bits = (uint32_t)1u << ej->tipo_primario;
            if (ej->tipo_secundario != TIPO_NINGUNO) {
                bits |= (uint32_t)1u << ej->tipo_secundario;
            }
            tips += bt_contar_bits(bits & ~masc);
            masc |= bits;
            niv += ej->nivel;
            atq += ej->ataque;
            ej->siguiente = *parcial;
            *parcial = ej;
            (*creados)++;
        }

        if (j == k &&
            bt_rec(pd, r, tipos_sufijo, i + 1, restantes - k,
                   niv, tips, atq, masc, parcial, creados)) {
            return true;   /* salida temprana: primera solución hallada */
        }

        /* Retroceder (§4.4): liberar las j copias descartadas de esta rama
           para que la búsqueda continúe sin fugas de memoria. */
        while (j > 0) {
            Ejemplar *descarte = *parcial;
            *parcial = descarte->siguiente;
            free(descarte);
            (*creados)--;
            j--;
        }
    }

    /* Excluir la especie i y probar con la siguiente. */
    return bt_rec(pd, r, tipos_sufijo, i + 1, restantes, nivel_acum,
                  tipos_dist, ataque_acum, mascara, parcial, creados);
}

/* Implementación de equipo_formar_backtracking: documentación canónica en equipo.h. */
bool equipo_formar_backtracking(const Pokedex *pd, const RestriccionesEquipo *r,
                                Ejemplar **salida, int *cantidad)
{
    uint32_t tipos_sufijo[POKEDEX_MAX + 1];
    Ejemplar *parcial = NULL;
    int creados = 0;
    int i;

    if (salida != NULL) {
        *salida = NULL;
    }
    if (cantidad != NULL) {
        *cantidad = 0;
    }
    if (pd == NULL || r == NULL || salida == NULL || cantidad == NULL) {
        return false;
    }
    if (r->cantidad < 1 || r->cantidad > MAX_EQUIPO ||
        r->nivel_total_max < 1 || r->min_tipos < 1 ||
        r->min_tipos > CANT_TIPOS || pd->cantidad == 0) {
        return false;
    }

    /* Tabla sufijo de tipos: en tipos_sufijo[i] están los tipos de todas
       las especies con índice >= i (base de la poda 3, §4.3.3). */
    tipos_sufijo[POKEDEX_MAX] = 0;
    for (i = POKEDEX_MAX - 1; i >= 0; i--) {
        uint32_t bits = 0;
        if (i < pd->cantidad) {
            bits = (uint32_t)1u << pd->especies[i].tipo_primario;
            if (pd->especies[i].tipo_secundario != TIPO_NINGUNO) {
                bits |= (uint32_t)1u << pd->especies[i].tipo_secundario;
            }
        }
        tipos_sufijo[i] = tipos_sufijo[i + 1] | bits;
    }

    if (!bt_rec(pd, r, tipos_sufijo, 0, r->cantidad, 0, 0, 0, 0u,
                &parcial, &creados)) {
        return false;   /* *salida ya quedó en NULL: sin equipo inválido */
    }

    /* La búsqueda inserta al frente: invertir para el orden de inclusión. */
    bt_invertir_lista(&parcial);
    *salida = parcial;
    *cantidad = creados;
    return true;
}
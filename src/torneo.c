/**
 * @file torneo.c
 * @brief Estado del torneo: grupos A-H, calendario 1-48, puntuación 3/1/0
 *        y clasificación con desempates (RF-TRN-01..06, RF-CLS-01, D9/D10).
 *
 * F5: fase de grupos completa. La transición a eliminatorias (49-64) y la
 * tabla de bracket se completan en F6.
 * @author <Nombre del estudiante>
 * @date 2026-09-21
 */

#include <stdio.h>
#include <string.h>

#include "torneo.h"

/* 8 grupos (A..H) de 4 entrenadores: 32 en total (D10). */
#define CANT_GRUPOS 8
#define POR_GRUPO 4
/* Combates de la fase de grupos: 8 grupos x 6 pares (RF-TRN-02). */
#define COMBATES_GRUPO 48

/* Pares fijos del round-robin de un grupo (diseño §6.1):
   (m0,m1) (m0,m2) (m0,m3) (m1,m2) (m1,m3) (m2,m3). */
static const int PARES[6][2] = {
    {0, 1}, {0, 2}, {0, 3}, {1, 2}, {1, 3}, {2, 3}
};

/**
 * Número de combate del par p (0..5) dentro del grupo g (0..7):
 * 6*g + p + 1 (grupo A: 1-6, B: 7-12, ..., H: 43-48).
 */
static int numero_combate(int grupo, int par)
{
    return 6 * grupo + par + 1;
}

/**
 * Letra del grupo (0..7 -> 'A'..'H').
 */
static char letra_grupo(int grupo)
{
    return (char)('A' + grupo);
}

/**
 * Acumula las estadísticas de fase de grupos de un entrenador recorriendo
 * los combates 1..48 (fuente de verdad: los resultados aplicados; los
 * combates pendientes no aportan).
 */
static void stats_de_combates(const Torneo *t, int id, int *puntos,
                              int *victorias, int *empates, int *derrotas,
                              int *derrotados)
{
    int k;
    *puntos = *victorias = *empates = *derrotas = *derrotados = 0;
    for (k = 0; k < COMBATES_GRUPO; k++) {
        const Combate *c = &t->combates[k];
        int es1 = (c->id_entrenador1 == id);
        int es2 = (c->id_entrenador2 == id);
        if (!es1 && !es2) {
            continue;
        }
        if (c->estado == RES_PENDIENTE) {
            continue;
        }
        if (c->estado == RES_EMPATE) {
            (*empates)++;
            (*puntos)++;
        } else if (c->id_ganador == id) {
            (*victorias)++;
            (*puntos) += 3;
        } else {
            (*derrotas)++;
        }
        *derrotados += es1 ? c->kos1 : c->kos2;
    }
}

/**
 * Resultado del enfrentamiento directo entre a y b en fase de grupos (D9):
 * id del ganador; 0 si fue empate, está pendiente o no existe el combate
 * entre ambos (no debería ocurrir dentro del mismo grupo).
 */
static int combate_directo(const Torneo *t, int a, int b)
{
    int k;
    for (k = 0; k < COMBATES_GRUPO; k++) {
        const Combate *c = &t->combates[k];
        int es_par = (c->id_entrenador1 == a && c->id_entrenador2 == b) ||
                     (c->id_entrenador1 == b && c->id_entrenador2 == a);
        if (!es_par) {
            continue;
        }
        if (c->estado == RES_PENDIENTE || c->estado == RES_EMPATE) {
            return 0;
        }
        return c->id_ganador;
    }
    return 0;
}

/**
 * Comparador de clasificación (RF-TRN-04): devuelve <0 si a va antes que b.
 * 1) puntos desc, 2) victorias desc, 3) pokemon_derrotados desc,
 * 4) enfrentamiento directo (D9), 5) id asc.
 */
static int comparar_clasificacion(const Torneo *t, int a, int b)
{
    int pa, va, ea, da, ka;
    int pb, vb, eb, db, kb;
    int directo;

    stats_de_combates(t, a, &pa, &va, &ea, &da, &ka);
    stats_de_combates(t, b, &pb, &vb, &eb, &db, &kb);

    if (pa != pb) {
        return (pa > pb) ? -1 : 1;
    }
    if (va != vb) {
        return (va > vb) ? -1 : 1;
    }
    if (ka != kb) {
        return (ka > kb) ? -1 : 1;
    }

    directo = combate_directo(t, a, b);
    if (directo == a) {
        return -1;
    }
    if (directo == b) {
        return 1;
    }

    return (a < b) ? -1 : 1;
}

/**
 * Ordena los 4 ids de un grupo con inserción según el comparador de
 * RF-TRN-04 (determinista: el orden inicial es el de aparición en el
 * calendario y el comparador es una cadena total con el id como último
 * criterio).
 */
static void ordenar_grupo(const Torneo *t, int ids[POR_GRUPO])
{
    int i, j;
    for (i = 1; i < POR_GRUPO; i++) {
        int clave = ids[i];
        j = i - 1;
        while (j >= 0 && comparar_clasificacion(t, clave, ids[j]) < 0) {
            ids[j + 1] = ids[j];
            j--;
        }
        ids[j + 1] = clave;
    }
}

/**
 * Ids de los 4 entrenadores del grupo g en el orden de aparición en el
 * calendario (m0..m3 = posiciones 4g..4g+3 del registro; el primer combate
 * del grupo entrega m0 y m1, el segundo m2 y el tercero m3).
 */
static void ids_del_grupo(const Torneo *t, int grupo, int ids[POR_GRUPO])
{
    int par, k, n;
    n = 0;
    for (par = 0; par < 6 && n < POR_GRUPO; par++) {
        const Combate *c = &t->combates[numero_combate(grupo, par) - 1];
        int candidatos[2];
        candidatos[0] = c->id_entrenador1;
        candidatos[1] = c->id_entrenador2;
        for (k = 0; k < 2; k++) {
            int ya = 0, m;
            if (candidatos[k] <= 0) {
                continue;
            }
            for (m = 0; m < n; m++) {
                if (ids[m] == candidatos[k]) {
                    ya = 1;
                }
            }
            if (!ya) {
                ids[n++] = candidatos[k];
            }
        }
    }
}

/**
 * Núcleo de la clasificación (RF-TRN-05/06): ordena cada grupo con la
 * cadena de RF-TRN-04 y guarda el 1.º y 2.º en id_clasificados[16] con el
 * orden 1A, 2A, 1B, 2B, ..., 1H, 2H.
 */
static void clasificar_grupos(Torneo *t)
{
    int g;
    for (g = 0; g < CANT_GRUPOS; g++) {
        int ids[POR_GRUPO];
        ids_del_grupo(t, g, ids);
        ordenar_grupo(t, ids);
        t->id_clasificados[2 * g] = ids[0];
        t->id_clasificados[2 * g + 1] = ids[1];
    }
}

/**
 * Busca un entrenador por id sin mutar el registro (solo lectura, para
 * mostrar nombres en las consultas).
 */
static const Entrenador *entrenador_por_id(const RegistroEntrenadores *reg,
                                           int id)
{
    int i;
    for (i = 0; i < reg->cantidad; i++) {
        if (reg->entrenadores[i].id == id) {
            return &reg->entrenadores[i];
        }
    }
    return NULL;
}

bool torneo_armar_grupos(Torneo *t, const RegistroEntrenadores *reg)
{
    int g, par, k;

    if (t == NULL || reg == NULL) {
        return false;
    }
    if (reg->cantidad != MAX_ENTRENADORES) {
        return false;   /* D10: exactamente 32 entrenadores */
    }

    /* Estado inicial de los 64 combates (49..64 se resuelven en F6). */
    for (k = 0; k < TOTAL_COMBATES; k++) {
        t->combates[k].numero = k + 1;
        t->combates[k].id_entrenador1 = 0;
        t->combates[k].id_entrenador2 = 0;
        t->combates[k].estado = RES_PENDIENTE;
        t->combates[k].id_ganador = 0;
        t->combates[k].kos1 = 0;
        t->combates[k].kos2 = 0;
    }

    /* Distribución determinista (diseño §6.1): la posición k del registro
       define grupo k/4 y puesto k%4; el calendario usa los pares fijos con
       número de combate 6*g + par + 1 (A: 1-6, ..., H: 43-48). */
    for (g = 0; g < CANT_GRUPOS; g++) {
        for (par = 0; par < 6; par++) {
            Combate *c = &t->combates[numero_combate(g, par) - 1];
            c->id_entrenador1 = reg->entrenadores[4 * g + PARES[par][0]].id;
            c->id_entrenador2 = reg->entrenadores[4 * g + PARES[par][1]].id;
        }
    }

    for (k = 0; k < 16; k++) {
        t->id_clasificados[k] = 0;
    }
    t->estado = TORNEO_GRUPOS;
    return true;
}

bool torneo_aplicar_resultado(Torneo *t, RegistroEntrenadores *reg,
                              const ResultadoCargado *r, char *msg, size_t n)
{
    Combate *c;
    Entrenador *e1;
    Entrenador *e2;
    int k;
    int completos;

    if (t == NULL || reg == NULL || r == NULL || msg == NULL || n == 0) {
        return false;
    }
    if (t->estado == TORNEO_SIN_INICIAR) {
        snprintf(msg, n, "El torneo no está armado (se requieren %d "
                         "entrenadores registrados).", MAX_ENTRENADORES);
        return false;
    }
    if (r->numero < 1 || r->numero > TOTAL_COMBATES) {
        snprintf(msg, n, "Número de combate inválido: %d.", r->numero);
        return false;
    }
    if (r->numero > COMBATES_GRUPO) {
        snprintf(msg, n, "El combate %d pertenece a la fase eliminatoria "
                         "(aún no disponible).", r->numero);
        return false;
    }
    c = &t->combates[r->numero - 1];
    if (c->estado != RES_PENDIENTE) {
        snprintf(msg, n, "El combate %d ya tiene resultado.", r->numero);
        return false;
    }
    if (r->id_entrenador1 != c->id_entrenador1 ||
        r->id_entrenador2 != c->id_entrenador2) {
        snprintf(msg, n, "Los participantes no coinciden con los resueltos "
                         "por el sistema (RF-RES-03).");
        return false;
    }
    if (r->resultado != RES_V1 && r->resultado != RES_V2 &&
        r->resultado != RES_EMPATE) {
        snprintf(msg, n, "Resultado inválido para el combate %d.", r->numero);
        return false;
    }
    if (r->kos1 < 0 || r->kos2 < 0) {
        snprintf(msg, n, "Los KOs del combate %d no pueden ser negativos.",
                 r->numero);
        return false;
    }
    if (r->resultado == RES_V1 && r->id_ganador != c->id_entrenador1) {
        snprintf(msg, n, "El ganador no coincide con el resultado (V1).");
        return false;
    }
    if (r->resultado == RES_V2 && r->id_ganador != c->id_entrenador2) {
        snprintf(msg, n, "El ganador no coincide con el resultado (V2).");
        return false;
    }
    if (r->resultado == RES_EMPATE && r->id_ganador != 0) {
        snprintf(msg, n, "Un empate no declara ganador.");
        return false;
    }

    e1 = entrenador_buscar(reg, c->id_entrenador1);
    e2 = entrenador_buscar(reg, c->id_entrenador2);
    if (e1 == NULL || e2 == NULL) {
        snprintf(msg, n, "Entrenador del combate %d no encontrado.",
                 r->numero);
        return false;
    }

    c->estado = r->resultado;
    c->id_ganador = r->id_ganador;
    c->kos1 = r->kos1;
    c->kos2 = r->kos2;

    /* Puntuación 3/1/0 (RF-TRN-03) y KOs (criterio 3 de RF-TRN-04). */
    if (r->resultado == RES_EMPATE) {
        e1->empates++;
        e2->empates++;
        e1->puntos++;
        e2->puntos++;
    } else if (r->id_ganador == c->id_entrenador1) {
        e1->victorias++;
        e1->puntos += 3;
        e2->derrotas++;
    } else {
        e2->victorias++;
        e2->puntos += 3;
        e1->derrotas++;
    }
    e1->pokemon_derrotados += r->kos1;
    e2->pokemon_derrotados += r->kos2;

    snprintf(msg, n, "Resultado del combate %d (grupo %c) aplicado: %s.",
             r->numero, letra_grupo((r->numero - 1) / 6),
             r->resultado == RES_EMPATE ? "empate" : "victoria");

    /* Al completar los 48 combates de grupos se ordena y se definen los
       clasificados (RF-TRN-06); la transición a eliminatorias es de F6. */
    completos = 0;
    for (k = 0; k < COMBATES_GRUPO; k++) {
        if (t->combates[k].estado != RES_PENDIENTE) {
            completos++;
        }
    }
    if (completos == COMBATES_GRUPO) {
        torneo_ordenar_grupos(t);
        snprintf(msg, n, "Fase de grupos completada: clasificados "
                         "1A/2A...1H/2H definidos.");
    }
    return true;
}

void torneo_ordenar_grupos(Torneo *t)
{
    if (t == NULL) {
        return;
    }
    clasificar_grupos(t);
}

void torneo_clasificados(Torneo *t)
{
    if (t == NULL) {
        return;
    }
    clasificar_grupos(t);
}

void torneo_mostrar_clasificacion(const Torneo *t,
                                  const RegistroEntrenadores *reg)
{
    int g, p;

    if (t == NULL || reg == NULL) {
        return;
    }
    if (t->estado == TORNEO_SIN_INICIAR) {
        printf("El torneo no está armado.\n");
        return;
    }

    for (g = 0; g < CANT_GRUPOS; g++) {
        int ids[POR_GRUPO];
        ids_del_grupo(t, g, ids);
        ordenar_grupo(t, ids);
        printf("[GRUPO %c]\n", letra_grupo(g));
        for (p = 0; p < POR_GRUPO; p++) {
            const Entrenador *e = entrenador_por_id(reg, ids[p]);
            int puntos, victorias, empates, derrotas, derrotados;
            stats_de_combates(t, ids[p], &puntos, &victorias, &empates,
                              &derrotas, &derrotados);
            printf("%d;%d;%s;%d;%d;%d;%d;%d\n",
                   p + 1, ids[p], e != NULL ? e->nombre : "?",
                   victorias, empates, derrotas, puntos, derrotados);
        }
    }
}

void torneo_mostrar_enfrentamientos(const Torneo *t,
                                    const RegistroEntrenadores *reg)
{
    int k;

    if (t == NULL || reg == NULL) {
        return;
    }
    if (t->estado == TORNEO_SIN_INICIAR) {
        printf("El torneo no está armado.\n");
        return;
    }

    printf("=== Enfrentamientos de la fase de grupos (combates 1-48) ===\n");
    for (k = 0; k < COMBATES_GRUPO; k++) {
        const Combate *c = &t->combates[k];
        const Entrenador *e1 = entrenador_por_id(reg, c->id_entrenador1);
        const Entrenador *e2 = entrenador_por_id(reg, c->id_entrenador2);
        printf("%d;%c;%d;%s;%d;%s\n", c->numero,
               letra_grupo((c->numero - 1) / 6),
               c->id_entrenador1, e1 != NULL ? e1->nombre : "?",
               c->id_entrenador2, e2 != NULL ? e2->nombre : "?");
    }
}
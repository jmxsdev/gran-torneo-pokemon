/* Jesus Mora - CI 28480557 */
/**
 * @file torneo.c
 * @brief Estado del torneo: grupos A-H, calendario 1-48, puntuación 3/1/0,
 * clasificación y bracket eliminatorio 49-64.
 *
 * Fase de grupos, clasificación con desempates, bracket 49-64 con
 * participantes resueltos por el sistema, transición de estados y
 * posiciones finales.
 * @author Jesus Mora
 * @date 2026-09-21
 */

#include <stdio.h>
#include <string.h>

#include "torneo.h"
#include "pantalla.h"

/* 8 grupos (A..H) de 4 entrenadores: 32 en total. */
#define CANT_GRUPOS 8
#define POR_GRUPO 4
/* Combates de la fase de grupos: 8 grupos x 6 pares.
   COMBATES_GRUPO (48) se define en constantes.h. */

/* Pares fijos del round-robin de un grupo:
   (m0,m1) (m0,m2) (m0,m3) (m1,m2) (m1,m3) (m2,m3). */
static const int PARES[6][2] = {
    {0, 1}, {0, 2}, {0, 3}, {1, 2}, {1, 3}, {2, 3}
};

/* ---- Fuentes de los participantes del bracket ---- */

/* Tipo de fuente de un participante en la tabla ORIGEN_PARTICIPANTE. */
typedef enum {
    ORIGEN_NINGUNO = 0,   /* 1..48: los fija el calendario */
    ORIGEN_CLASIFICADO,   /* 49..56: puesto del grupo en id_clasificados */
    ORIGEN_GANADOR,       /* 57..64: ganador del combate fuente (G#) */
    ORIGEN_PERDEDOR       /* 63: perdedor del combate fuente (P61/P62) */
} TipoOrigen;

/* Un participante del bracket según su origen. */
typedef struct {
    TipoOrigen tipo;
    int valor;   /* índice 0..15 de id_clasificados, o número de combate */
} OrigenParticipante;

/* Tabla estática de orígenes: de dónde sale cada participante
   de los combates 49..64. Las filas 1..48 quedan en ORIGEN_NINGUNO (el
   calendario las fija en torneo_armar_grupos). Orden de id_clasificados:
   1A,2A,1B,2B,...,1H,2H (índices 0..15). Los octavos enfrentan a los 16
   clasificados con los cruces fijos de y los cuartos/semifinales/
   tercer lugar/final encadenan ganadores y perdedores. */
static const OrigenParticipante ORIGEN_PARTICIPANTE[TOTAL_COMBATES][2] = {
    /* Octavos 49-52: cruces superiores del cuadro */
    [48] = { {ORIGEN_CLASIFICADO, 0},  {ORIGEN_CLASIFICADO, 3} },  /* 1A-2B */
    [49] = { {ORIGEN_CLASIFICADO, 4},  {ORIGEN_CLASIFICADO, 7} },  /* 1C-2D */
    [50] = { {ORIGEN_CLASIFICADO, 8},  {ORIGEN_CLASIFICADO, 11} }, /* 1E-2F */
    [51] = { {ORIGEN_CLASIFICADO, 12}, {ORIGEN_CLASIFICADO, 15} }, /* 1G-2H */
    /* Octavos 53-56: cruces inferiores del cuadro */
    [52] = { {ORIGEN_CLASIFICADO, 2},  {ORIGEN_CLASIFICADO, 1} },  /* 1B-2A */
    [53] = { {ORIGEN_CLASIFICADO, 6},  {ORIGEN_CLASIFICADO, 5} },  /* 1D-2C */
    [54] = { {ORIGEN_CLASIFICADO, 10}, {ORIGEN_CLASIFICADO, 9} },  /* 1F-2E */
    [55] = { {ORIGEN_CLASIFICADO, 14}, {ORIGEN_CLASIFICADO, 13} }, /* 1H-2G */
    /* Cuartos 57-60 */
    [56] = { {ORIGEN_GANADOR, 49}, {ORIGEN_GANADOR, 50} },  /* G49-G50 */
    [57] = { {ORIGEN_GANADOR, 53}, {ORIGEN_GANADOR, 54} },  /* G53-G54 */
    [58] = { {ORIGEN_GANADOR, 51}, {ORIGEN_GANADOR, 52} },  /* G51-G52 */
    [59] = { {ORIGEN_GANADOR, 55}, {ORIGEN_GANADOR, 56} },  /* G55-G56 */
    /* Semifinales 61-62 */
    [60] = { {ORIGEN_GANADOR, 57}, {ORIGEN_GANADOR, 58} },  /* G57-G58 */
    [61] = { {ORIGEN_GANADOR, 59}, {ORIGEN_GANADOR, 60} },  /* G59-G60 */
    /* Tercer lugar 63 y final 64 */
    [62] = { {ORIGEN_PERDEDOR, 61}, {ORIGEN_PERDEDOR, 62} }, /* P61-P62 */
    [63] = { {ORIGEN_GANADOR, 61},  {ORIGEN_GANADOR, 62} }   /* G61-G62 */
};

/**
 * Número de combate del par p (0..5) dentro del grupo g (0..7):
 * 6*g + p + 1 (grupo A: 1-6, B: 7-12,..., H: 43-48).
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
 * Resultado del enfrentamiento directo entre a y b en fase de grupos:
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
 * Comparador de clasificación: devuelve <0 si a va antes que b.
 * 1) puntos desc, 2) victorias desc, 3) pokemon_derrotados desc,
 * 4) enfrentamiento directo, 5) id asc.
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
 * (determinista: el orden inicial es el de aparición en el
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
 * Núcleo de la clasificación: ordena cada grupo con la
 * cadena de y guarda el 1.º y 2.º en id_clasificados[16] con el
 * orden 1A, 2A, 1B, 2B,..., 1H, 2H.
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

/**
 * Nombre de un entrenador, o "?" si el id no existe en el registro.
 */
static const char *nombre_entrenador(const RegistroEntrenadores *reg, int id)
{
    const Entrenador *e = entrenador_por_id(reg, id);
    return e != NULL ? e->nombre : "?";
}

/**
 * Resuelve el id del entrenador que produce una fuente del bracket.
 * CLASIFICADO: id_clasificados[valor] (0 si la fase de grupos no terminó).
 * GANADOR: ganador del combate fuente (0 si está pendiente). PERDEDOR:
 * perdedor del combate fuente (el que no ganó; 0 si está pendiente).
 * ORIGEN_NINGUNO: 0 (no aplica a los combates 1..48).
 */
static int resolver_origen(const Torneo *t, const OrigenParticipante *o)
{
    const Combate *c;

    if (o->tipo == ORIGEN_CLASIFICADO) {
        return t->id_clasificados[o->valor];
    }
    if (o->tipo == ORIGEN_GANADOR) {
        c = &t->combates[o->valor - 1];
        if (c->estado == RES_PENDIENTE) {
            return 0;
        }
        return c->id_ganador;
    }
    if (o->tipo == ORIGEN_PERDEDOR) {
        c = &t->combates[o->valor - 1];
        if (c->estado == RES_PENDIENTE) {
            return 0;
        }
        return (c->id_ganador == c->id_entrenador1)
                   ? c->id_entrenador2
                   : c->id_entrenador1;
    }
    return 0;
}

void torneo_participantes(const Torneo *t, int numero, int *id1, int *id2)
{
    if (id1 != NULL) {
        *id1 = 0;
    }
    if (id2 != NULL) {
        *id2 = 0;
    }
    if (t == NULL || id1 == NULL || id2 == NULL) {
        return;
    }
    if (numero < 1 || numero > TOTAL_COMBATES) {
        return;
    }
    if (numero <= COMBATES_GRUPO) {
        *id1 = t->combates[numero - 1].id_entrenador1;
        *id2 = t->combates[numero - 1].id_entrenador2;
        return;
    }
    *id1 = resolver_origen(t, &ORIGEN_PARTICIPANTE[numero - 1][0]);
    *id2 = resolver_origen(t, &ORIGEN_PARTICIPANTE[numero - 1][1]);
}

/**
 * Ronda del bracket a la que pertenece un combate de eliminatoria:
 * 1 octavos, 2 cuartos, 3 semifinal, 4 tercer lugar, 5 final.
 */
static int ronda_de(int numero)
{
    if (numero <= 56) {
        return 1;
    }
    if (numero <= 60) {
        return 2;
    }
    if (numero <= 62) {
        return 3;
    }
    return (numero == 63) ? 4 : 5;
}

/**
 * Nombre textual de la ronda de un combate de eliminatoria.
 */
static const char *ronda_a_texto(int numero)
{
    static const char *const RONDAS[5] = {
        "Octavos", "Cuartos", "Semifinal", "Tercer lugar", "Final"
    };
    return RONDAS[ronda_de(numero) - 1];
}

/**
 * Escribe la etiqueta de una fuente del bracket ("1A", "G49", "P61" o "-"
 * para el calendario) en el buffer buf.
 */
static void etiqueta_origen(const OrigenParticipante *o, char *buf, size_t n)
{
    if (o->tipo == ORIGEN_CLASIFICADO) {
        snprintf(buf, n, "%d%c", (o->valor % 2) + 1,
                 (char)('A' + o->valor / 2));
    } else if (o->tipo == ORIGEN_GANADOR) {
        snprintf(buf, n, "G%d", o->valor);
    } else if (o->tipo == ORIGEN_PERDEDOR) {
        snprintf(buf, n, "P%d", o->valor);
    } else {
        snprintf(buf, n, "-");
    }
}

/* Ancho del recuadro de posiciones finales y campeón. */
#define POS_ANCHO 55

/**
 * Imprime las posiciones finales del torneo: campeon = G64,
 * subcampeon = P64, tercero = G63, cuarto = P63, con estilo.
 */
static void mostrar_posiciones(const Torneo *t,
                               const RegistroEntrenadores *reg)
{
    const Combate *c64 = &t->combates[63];
    const Combate *c63 = &t->combates[62];
    int campeon = c64->id_ganador;
    int subcampeon = (campeon == c64->id_entrenador1)
                         ? c64->id_entrenador2
                         : c64->id_entrenador1;
    int tercero = c63->id_ganador;
    int cuarto = (tercero == c63->id_entrenador1)
                     ? c63->id_entrenador2
                     : c63->id_entrenador1;

    pantalla_linea(POS_ANCHO, "═");
    pantalla_centro("POSICIONES FINALES", POS_ANCHO);
    pantalla_linea(POS_ANCHO, "═");
    printf("   1.  ");
    pantalla_pad("Campeón", 14);
    printf("  %s (id %d)\n", nombre_entrenador(reg, campeon), campeon);
    printf("   2.  ");
    pantalla_pad("Subcampeón", 14);
    printf("  %s (id %d)\n", nombre_entrenador(reg, subcampeon),
           subcampeon);
    printf("   3.  ");
    pantalla_pad("Tercer lugar", 14);
    printf("  %s (id %d)\n", nombre_entrenador(reg, tercero), tercero);
    printf("   4.  ");
    pantalla_pad("Cuarto lugar", 14);
    printf("  %s (id %d)\n", nombre_entrenador(reg, cuarto), cuarto);
    pantalla_linea(POS_ANCHO, "─");
}

void torneo_armar_grupos(Torneo *t, const RegistroEntrenadores *reg,
                         bool *exito)
{
    int g, par, k;

    if (t == NULL || reg == NULL) {
        *exito = false;
        return;
    }
    if (reg->cantidad != MAX_ENTRENADORES) {
        *exito = false;   /* exactamente 32 entrenadores */
        return;
    }

    /* Estado inicial de los 64 combates (49..64 se resuelven). */
    for (k = 0; k < TOTAL_COMBATES; k++) {
        t->combates[k].numero = k + 1;
        t->combates[k].id_entrenador1 = 0;
        t->combates[k].id_entrenador2 = 0;
        t->combates[k].estado = RES_PENDIENTE;
        t->combates[k].id_ganador = 0;
        t->combates[k].kos1 = 0;
        t->combates[k].kos2 = 0;
    }

    /* Distribución determinista: la posición k del registro
       define grupo k/4 y puesto k%4; el calendario usa los pares fijos con
       número de combate 6*g + par + 1 (A: 1-6,..., H: 43-48). */
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
    *exito = true;
}

void torneo_aplicar_resultado(Torneo *t, RegistroEntrenadores *reg,
                              const ResultadoCargado *r, char *msg, size_t n,
                              bool *exito)
{
    Combate *c;
    Entrenador *e1;
    Entrenador *e2;
    const Entrenador *ganador;
    int k;
    int completos;
    int es_eliminatoria;

    if (t == NULL || reg == NULL || r == NULL || msg == NULL || n == 0) {
        *exito = false;
        return;
    }
    if (t->estado == TORNEO_SIN_INICIAR) {
        snprintf(msg, n, "El torneo no está armado (se requieren %d "
                         "entrenadores registrados).", MAX_ENTRENADORES);
        *exito = false;
        return;
    }
    if (r->numero < 1 || r->numero > TOTAL_COMBATES) {
        snprintf(msg, n, "Número de combate inválido: %d.", r->numero);
        *exito = false;
        return;
    }
    if (t->estado == TORNEO_FINALIZADO) {
        snprintf(msg, n, "El torneo ya finalizó; no se aceptan más "
                         "resultados.");
        *exito = false;
        return;
    }
    es_eliminatoria = (r->numero > COMBATES_GRUPO);
    if (es_eliminatoria && t->estado != TORNEO_ELIMINATORIAS) {
        snprintf(msg, n, "La fase de grupos no está completa: faltan "
                         "resultados de los combates 1-48.");
        *exito = false;
        return;
    }

    c = &t->combates[r->numero - 1];
    if (c->estado != RES_PENDIENTE) {
        snprintf(msg, n, "El combate %d ya tiene resultado.", r->numero);
        *exito = false;
        return;
    }

    /* Participantes resueltos por el sistema: el calendario
       fija 1..48 y el bracket (clasificados / G# / P#) resuelve 49..64. */
    if (es_eliminatoria) {
        int id1;
        int id2;
        torneo_participantes(t, r->numero, &id1, &id2);
        if (id1 == 0 || id2 == 0) {
            snprintf(msg, n, "El combate %d no está disponible aún: sus "
                             "fuentes (clasificados o G#/P#) no están "
                             "resueltas.", r->numero);
            *exito = false;
            return;
        }
        if (r->id_entrenador1 != id1 || r->id_entrenador2 != id2) {
            snprintf(msg, n, "Los participantes no coinciden con los "
                             "resueltos por el sistema.");
            *exito = false;
            return;
        }
        /* Persiste los participantes resueltos: el perdedor de una fuente
           P# y las consultas del bracket los leen del combate. */
        c->id_entrenador1 = id1;
        c->id_entrenador2 = id2;
    } else if (r->id_entrenador1 != c->id_entrenador1 ||
               r->id_entrenador2 != c->id_entrenador2) {
        snprintf(msg, n, "Los participantes no coinciden con los resueltos "
                         "por el sistema.");
        *exito = false;
        return;
    }

    if (r->resultado != RES_V1 && r->resultado != RES_V2 &&
        r->resultado != RES_EMPATE) {
        snprintf(msg, n, "Resultado inválido para el combate %d.", r->numero);
        *exito = false;
        return;
    }
    if (es_eliminatoria && r->resultado == RES_EMPATE) {
        snprintf(msg, n, "La eliminatoria no admite empates.");
        *exito = false;
        return;
    }
    if (r->kos1 < 0 || r->kos2 < 0) {
        snprintf(msg, n, "Los KOs del combate %d no pueden ser negativos.",
                 r->numero);
        *exito = false;
        return;
    }
    if (r->resultado == RES_V1 && r->id_ganador != c->id_entrenador1) {
        snprintf(msg, n, "El ganador no coincide con el resultado (V1).");
        *exito = false;
        return;
    }
    if (r->resultado == RES_V2 && r->id_ganador != c->id_entrenador2) {
        snprintf(msg, n, "El ganador no coincide con el resultado (V2).");
        *exito = false;
        return;
    }
    if (r->resultado == RES_EMPATE && r->id_ganador != 0) {
        snprintf(msg, n, "Un empate no declara ganador.");
        *exito = false;
        return;
    }

    e1 = entrenador_buscar(reg, c->id_entrenador1);
    e2 = entrenador_buscar(reg, c->id_entrenador2);
    if (e1 == NULL || e2 == NULL) {
        snprintf(msg, n, "Entrenador del combate %d no encontrado.",
                 r->numero);
        *exito = false;
        return;
    }
    ganador = (r->id_ganador == c->id_entrenador1) ? e1 : e2;

    c->estado = r->resultado;
    c->id_ganador = r->id_ganador;
    c->kos1 = r->kos1;
    c->kos2 = r->kos2;

    if (es_eliminatoria) {
        /* La eliminatoria no toca los contadores de grupos (puntúa 3/1/0
           solo en 1..48): solo avanza G#/P# en el estado. */
        snprintf(msg, n, "Resultado del combate %d (%s) aplicado: victoria "
                         "de %s (id %d).", r->numero,
                 ronda_a_texto(r->numero),
                 ganador != NULL ? ganador->nombre : "?", r->id_ganador);

        /* el torneo finaliza solo cuando la final (64) Y el tercer
           lugar (63) tienen resultado; aplicar 64 antes de 63 ya no bloquea
           el combate 63 (antes quedaba perdido para siempre). */
        if (t->combates[TOTAL_COMBATES - 1].estado != RES_PENDIENTE &&
            t->combates[TOTAL_COMBATES - 2].estado != RES_PENDIENTE) {
            t->estado = TORNEO_FINALIZADO;
            /*: al finalizar, el sistema muestra los 4 primeros. */
            mostrar_posiciones(t, reg);
            snprintf(msg, n, "Torneo finalizado: el campeón es %s (id %d).",
                     nombre_entrenador(reg,
                             t->combates[TOTAL_COMBATES - 1].id_ganador),
                     t->combates[TOTAL_COMBATES - 1].id_ganador);
        } else if (r->numero == TOTAL_COMBATES) {
            snprintf(msg, n, "Resultado del combate %d (%s) aplicado: "
                             "victoria de %s (id %d). Falta resolver el "
                             "combate 63 (tercer lugar) para finalizar.",
                     r->numero, ronda_a_texto(r->numero),
                     ganador != NULL ? ganador->nombre : "?", r->id_ganador);
        }
        *exito = true;
        return;
    }

    /* Puntuación 3/1/0 y KOs (criterio 3). */
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

    /* Al completar los 48 combates de grupos se ordena, se definen los
       clasificados, se resuelven los octavos 49..56 y el
       torneo pasa a la eliminatoria. */
    completos = 0;
    for (k = 0; k < COMBATES_GRUPO; k++) {
        if (t->combates[k].estado != RES_PENDIENTE) {
            completos++;
        }
    }
    if (completos == COMBATES_GRUPO) {
        torneo_ordenar_grupos(t);
        for (k = 49; k <= 56; k++) {
            int id1;
            int id2;
            torneo_participantes(t, k, &id1, &id2);
            t->combates[k - 1].id_entrenador1 = id1;
            t->combates[k - 1].id_entrenador2 = id2;
        }
        t->estado = TORNEO_ELIMINATORIAS;
        snprintf(msg, n, "Fase de grupos completada: clasificados "
                         "1A/2A...1H/2H definidos; comienza la eliminatoria "
                         "(combates 49-64).");
    }
    *exito = true;
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

void torneo_clasificacion_grupo(const Torneo *t, int grupo, int filas[4][7])
{
    int ids[POR_GRUPO];
    int p;

    if (t == NULL || filas == NULL || grupo < 0 || grupo >= CANT_GRUPOS) {
        return;
    }
    ids_del_grupo(t, grupo, ids);
    ordenar_grupo(t, ids);
    for (p = 0; p < POR_GRUPO; p++) {
        int puntos;
        int victorias;
        int empates;
        int derrotas;
        int derrotados;
        stats_de_combates(t, ids[p], &puntos, &victorias, &empates,
                          &derrotas, &derrotados);
        filas[p][0] = p + 1;
        filas[p][1] = ids[p];
        filas[p][2] = victorias;
        filas[p][3] = empates;
        filas[p][4] = derrotas;
        filas[p][5] = puntos;
        filas[p][6] = derrotados;
    }
}

/* Anchos de la tabla de clasificación (columnas visibles UTF-8). */
#define CLA_ANCHO_POS     3
#define CLA_ANCHO_NOMBRE  20
#define CLA_ANCHO_TOTAL   (1 + CLA_ANCHO_POS + 2 + CLA_ANCHO_NOMBRE + 2 + \
                           4 + 2 + 4 + 2 + 4 + 2 + 7 + 2 + 9)

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

    pantalla_linea(CLA_ANCHO_TOTAL, "═");
    pantalla_centro("CLASIFICACIÓN", CLA_ANCHO_TOTAL);
    pantalla_linea(CLA_ANCHO_TOTAL, "═");
    for (g = 0; g < CANT_GRUPOS; g++) {
        int ids[POR_GRUPO];
        ids_del_grupo(t, g, ids);
        ordenar_grupo(t, ids);
        printf("\n GRUPO %c\n", letra_grupo(g));
        printf(" %3s  ", "Pos");
        pantalla_pad("Entrenador", CLA_ANCHO_NOMBRE);
        printf("  %4s  %4s  %4s  %7s  %9s\n", "V", "E", "D", "Puntos",
               "Derrotados");
        pantalla_linea(CLA_ANCHO_TOTAL, "─");
        for (p = 0; p < POR_GRUPO; p++) {
            const Entrenador *e = entrenador_por_id(reg, ids[p]);
            int puntos, victorias, empates, derrotas, derrotados;
            char nombre_txt[64];

            stats_de_combates(t, ids[p], &puntos, &victorias, &empates,
                              &derrotas, &derrotados);
            snprintf(nombre_txt, sizeof(nombre_txt), "%s (id %d)",
                     e != NULL ? e->nombre : "?", ids[p]);
            printf(" %3d  ", p + 1);
            pantalla_pad(nombre_txt, CLA_ANCHO_NOMBRE);
            printf("  %4d  %4d  %4d  %7d  %9d\n", victorias, empates,
                   derrotas, puntos, derrotados);
        }
    }
    pantalla_linea(CLA_ANCHO_TOTAL, "─");
}

/* Anchos de la tabla del calendario. */
#define CAL_ANCHO_NUM     3
#define CAL_ANCHO_GRUPO   7
#define CAL_ANCHO_NOMBRE  22
#define CAL_ANCHO_TOTAL   (1 + CAL_ANCHO_NUM + 2 + CAL_ANCHO_GRUPO + 2 + \
                           CAL_ANCHO_NOMBRE + 2 + CAL_ANCHO_NOMBRE)

void torneo_mostrar_enfrentamientos(const Torneo *t,
                                    const RegistroEntrenadores *reg)
{
    int k;
    char grupo_txt[8];
    char nombre1[64];
    char nombre2[64];

    if (t == NULL || reg == NULL) {
        return;
    }
    if (t->estado == TORNEO_SIN_INICIAR) {
        printf("El torneo no está armado.\n");
        return;
    }

    pantalla_linea(CAL_ANCHO_TOTAL, "═");
    pantalla_centro("ENFRENTAMIENTOS DE LA FASE DE GRUPOS (1-48)",
                    CAL_ANCHO_TOTAL);
    pantalla_linea(CAL_ANCHO_TOTAL, "═");
    printf(" %3s  ", "#");
    pantalla_pad("Grupo", CAL_ANCHO_GRUPO);
    printf("  ");
    pantalla_pad("Entrenador 1", CAL_ANCHO_NOMBRE);
    printf("  ");
    pantalla_pad("Entrenador 2", CAL_ANCHO_NOMBRE);
    printf("\n");
    pantalla_linea(CAL_ANCHO_TOTAL, "─");
    for (k = 0; k < COMBATES_GRUPO; k++) {
        const Combate *c = &t->combates[k];
        snprintf(grupo_txt, sizeof(grupo_txt), "%c",
                 letra_grupo(c->numero / 6));
        snprintf(nombre1, sizeof(nombre1), "%s (id %d)",
                 nombre_entrenador(reg, c->id_entrenador1),
                 c->id_entrenador1);
        snprintf(nombre2, sizeof(nombre2), "%s (id %d)",
                 nombre_entrenador(reg, c->id_entrenador2),
                 c->id_entrenador2);
        printf(" %3d  ", c->numero);
        pantalla_pad(grupo_txt, CAL_ANCHO_GRUPO);
        printf("  ");
        pantalla_pad(nombre1, CAL_ANCHO_NOMBRE);
        printf("  ");
        pantalla_pad(nombre2, CAL_ANCHO_NOMBRE);
        printf("\n");
    }
    pantalla_linea(CAL_ANCHO_TOTAL, "─");
}

/* Anchos de la tabla del historial (columnas visibles UTF-8). */
#define HIST_ANCHO_NUM   3
#define HIST_ANCHO_FASE  13
#define HIST_ANCHO_NOM   20
#define HIST_ANCHO_RES   10
#define HIST_ANCHO_GAN   20
/* 1 (borde inicial) + columnas + 2 espacios entre campos + 3 de "KOs". */
#define HIST_ANCHO_TOTAL (1 + HIST_ANCHO_NUM + 2 + HIST_ANCHO_FASE + 2 + \
                          HIST_ANCHO_NOM + 2 + HIST_ANCHO_NOM + 2 + \
                          HIST_ANCHO_RES + 2 + HIST_ANCHO_GAN + 2 + 3)

void torneo_mostrar_historial(const Torneo *t, const RegistroEntrenadores *reg)
{
    int k;
    int resueltos = 0;
    int pendientes = 0;
    char subtitulo[64];
    char resumen[64];

    if (t == NULL || reg == NULL) {
        return;
    }
    if (t->estado == TORNEO_SIN_INICIAR) {
        printf("El torneo no está armado.\n");
        return;
    }

    /* Conteo previo para el subtítulo y el pie de resumen. */
    for (k = 0; k < TOTAL_COMBATES; k++) {
        if (t->combates[k].estado == RES_PENDIENTE) {
            pendientes++;
        } else {
            resueltos++;
        }
    }

    snprintf(subtitulo, sizeof(subtitulo),
             "Resueltos: %d  ·  Pendientes: %d", resueltos, pendientes);
    pantalla_linea(HIST_ANCHO_TOTAL, "═");
    pantalla_centro("HISTORIAL DE COMBATES", HIST_ANCHO_TOTAL);
    pantalla_centro(subtitulo, HIST_ANCHO_TOTAL);
    pantalla_linea(HIST_ANCHO_TOTAL, "═");

    /* Cabecera de columnas alineada con los anchos de la tabla. */
    printf(" %*s", HIST_ANCHO_NUM, "#");
    printf("  ");
    pantalla_pad("Fase", HIST_ANCHO_FASE);
    printf("  ");
    pantalla_pad("Entrenador 1", HIST_ANCHO_NOM);
    printf("  ");
    pantalla_pad("Entrenador 2", HIST_ANCHO_NOM);
    printf("  ");
    pantalla_pad("Resultado", HIST_ANCHO_RES);
    printf("  ");
    pantalla_pad("Ganador", HIST_ANCHO_GAN);
    printf("  KOs\n");
    pantalla_linea(HIST_ANCHO_TOTAL, "─");

    for (k = 1; k <= TOTAL_COMBATES; k++) {
        const Combate *c = &t->combates[k - 1];
        int id1;
        int id2;
        char fase[16];
        char nombre1[64];
        char nombre2[64];
        char ganador[64];
        const char *resultado;
        int kos1;
        int kos2;

        torneo_participantes(t, k, &id1, &id2);

        if (k <= COMBATES_GRUPO) {
            snprintf(fase, sizeof(fase), "Grupo %c",
                     letra_grupo((k - 1) / 6));
        } else {
            snprintf(fase, sizeof(fase), "%s", ronda_a_texto(k));
        }

        if (id1 > 0) {
            snprintf(nombre1, sizeof(nombre1), "%s (id %d)",
                     nombre_entrenador(reg, id1), id1);
        } else {
            snprintf(nombre1, sizeof(nombre1), "(por definir)");
        }
        if (id2 > 0) {
            snprintf(nombre2, sizeof(nombre2), "%s (id %d)",
                     nombre_entrenador(reg, id2), id2);
        } else {
            snprintf(nombre2, sizeof(nombre2), "(por definir)");
        }

        switch (c->estado) {
        case RES_V1:
        case RES_V2:
            resultado = "Victoria";
            break;
        case RES_EMPATE:
            resultado = "Empate";
            break;
        default:
            resultado = "Pendiente";
            break;
        }

        if (c->estado == RES_V1 || c->estado == RES_V2) {
            snprintf(ganador, sizeof(ganador), "%s (id %d)",
                     nombre_entrenador(reg, c->id_ganador), c->id_ganador);
        } else {
            snprintf(ganador, sizeof(ganador), "-");
        }

        kos1 = (c->estado == RES_PENDIENTE) ? 0 : c->kos1;
        kos2 = (c->estado == RES_PENDIENTE) ? 0 : c->kos2;

        printf(" %*d", HIST_ANCHO_NUM, k);
        printf("  ");
        pantalla_pad(fase, HIST_ANCHO_FASE);
        printf("  ");
        pantalla_pad(nombre1, HIST_ANCHO_NOM);
        printf("  ");
        pantalla_pad(nombre2, HIST_ANCHO_NOM);
        printf("  ");
        pantalla_pad(resultado, HIST_ANCHO_RES);
        printf("  ");
        pantalla_pad(ganador, HIST_ANCHO_GAN);
        printf("  %d-%d\n", kos1, kos2);
    }

    pantalla_linea(HIST_ANCHO_TOTAL, "─");
    snprintf(resumen, sizeof(resumen),
             "Resumen: %d resueltos · %d pendientes", resueltos, pendientes);
    pantalla_centro(resumen, HIST_ANCHO_TOTAL);
}

/* Anchos de la tabla del bracket (columnas visibles UTF-8). */
#define BRI_ANCHO_NUM     3
#define BRI_ANCHO_ORIGEN  8
#define BRI_ANCHO_NOMBRE  20
#define BRI_ANCHO_TOTAL   (1 + BRI_ANCHO_NUM + 2 + BRI_ANCHO_ORIGEN + 2 + \
                           BRI_ANCHO_NOMBRE + 2 + BRI_ANCHO_ORIGEN + 2 + \
                           BRI_ANCHO_NOMBRE + 2 + BRI_ANCHO_NOMBRE)

void torneo_mostrar_resultados(const Torneo *t,
                               const RegistroEntrenadores *reg)
{
    int numero;
    int ronda_prev;

    if (t == NULL || reg == NULL) {
        return;
    }
    if (t->estado == TORNEO_SIN_INICIAR) {
        printf("El torneo no está armado.\n");
        return;
    }

    pantalla_linea(BRI_ANCHO_TOTAL, "═");
    pantalla_centro("RESULTADOS DEL TORNEO (COMBATES 49-64)",
                    BRI_ANCHO_TOTAL);
    pantalla_linea(BRI_ANCHO_TOTAL, "═");
    ronda_prev = 0;
    for (numero = 49; numero <= TOTAL_COMBATES; numero++) {
        const Combate *c = &t->combates[numero - 1];
        const OrigenParticipante *o1 = &ORIGEN_PARTICIPANTE[numero - 1][0];
        const OrigenParticipante *o2 = &ORIGEN_PARTICIPANTE[numero - 1][1];
        char et1[8];
        char et2[8];
        char nombre1[64];
        char nombre2[64];
        char ganador[64];
        int id1;
        int id2;
        int ronda = ronda_de(numero);

        if (ronda != ronda_prev) {
            printf("\n [%s]\n", ronda_a_texto(numero));
            printf(" %3s  ", "#");
            pantalla_pad("Origen 1", BRI_ANCHO_ORIGEN);
            printf("  ");
            pantalla_pad("Entrenador 1", BRI_ANCHO_NOMBRE);
            printf("  ");
            pantalla_pad("Origen 2", BRI_ANCHO_ORIGEN);
            printf("  ");
            pantalla_pad("Entrenador 2", BRI_ANCHO_NOMBRE);
            printf("  ");
            pantalla_pad("Ganador", BRI_ANCHO_NOMBRE);
            printf("\n");
            pantalla_linea(BRI_ANCHO_TOTAL, "─");
            ronda_prev = ronda;
        }
        torneo_participantes(t, numero, &id1, &id2);
        etiqueta_origen(o1, et1, sizeof(et1));
        etiqueta_origen(o2, et2, sizeof(et2));
        if (id1 > 0) {
            snprintf(nombre1, sizeof(nombre1), "%s (id %d)",
                     nombre_entrenador(reg, id1), id1);
        } else {
            snprintf(nombre1, sizeof(nombre1), "(por definir)");
        }
        if (id2 > 0) {
            snprintf(nombre2, sizeof(nombre2), "%s (id %d)",
                     nombre_entrenador(reg, id2), id2);
        } else {
            snprintf(nombre2, sizeof(nombre2), "(por definir)");
        }
        if (c->estado != RES_PENDIENTE) {
            snprintf(ganador, sizeof(ganador), "%s (id %d)",
                     nombre_entrenador(reg, c->id_ganador),
                     c->id_ganador);
        } else {
            snprintf(ganador, sizeof(ganador), "-");
        }
        printf(" %3d  ", numero);
        pantalla_pad(et1, BRI_ANCHO_ORIGEN);
        printf("  ");
        pantalla_pad(nombre1, BRI_ANCHO_NOMBRE);
        printf("  ");
        pantalla_pad(et2, BRI_ANCHO_ORIGEN);
        printf("  ");
        pantalla_pad(nombre2, BRI_ANCHO_NOMBRE);
        printf("  ");
        pantalla_pad(ganador, BRI_ANCHO_NOMBRE);
        printf("\n");
    }
    pantalla_linea(BRI_ANCHO_TOTAL, "─");

    if (t->estado == TORNEO_FINALIZADO) {
        mostrar_posiciones(t, reg);
    }
}

void torneo_mostrar_campeon(const Torneo *t,
                            const RegistroEntrenadores *reg)
{
    const Entrenador *campeon;
    char nombre_txt[64];

    if (t == NULL || reg == NULL) {
        return;
    }
    if (t->estado != TORNEO_FINALIZADO) {
        printf("El torneo aún no ha finalizado; aún no hay campeón.\n");
        return;
    }
    campeon = entrenador_por_id(reg, t->combates[63].id_ganador);
    snprintf(nombre_txt, sizeof(nombre_txt), "%s (id %d)",
             campeon != NULL ? campeon->nombre : "?",
             t->combates[63].id_ganador);
    pantalla_linea(POS_ANCHO, "═");
    pantalla_centro("CAMPEÓN DEL TORNEO", POS_ANCHO);
    pantalla_linea(POS_ANCHO, "═");
    pantalla_centro(nombre_txt, POS_ANCHO);
    pantalla_linea(POS_ANCHO, "═");
}

void torneo_mostrar_posiciones_finales(const Torneo *t,
                                       const RegistroEntrenadores *reg)
{
    if (t == NULL || reg == NULL) {
        return;
    }
    if (t->estado != TORNEO_FINALIZADO) {
        printf("El torneo aún no ha finalizado.\n");
        return;
    }
    mostrar_posiciones(t, reg);
}
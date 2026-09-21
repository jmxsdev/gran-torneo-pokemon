/**
 * @file resultados.c
 * @brief Carga de resultados por teclado o archivo con validación completa
 *        (RF-RES-01, RF-RES-02, RF-RES-03, RF-RES-04).
 *
 * F7: catálogo de validación del diseño §8.1. Los participantes los
 * resuelve el sistema (RF-RES-03); la aplicación se delega en
 * torneo_aplicar_resultado.
 * @author <Nombre del estudiante>
 * @date 2026-09-21
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "resultados.h"

/* Combates de la fase de grupos (48); el resto (49..64) es eliminatoria. */
#define COMBATES_GRUPO 48

/* Busca un entrenador por id sin mutar el registro (solo lectura). */
static const Entrenador *entrenador_por_id(const RegistroEntrenadores *reg,
                                           int id)
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

/* Descarta el resto de la línea actual de stdin (tras un scanf de entero). */
static void descartar_linea(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        /* descartar */
    }
}

/**
 * Lee un entero de stdin validando la conversión (RF-TEC-03): 1 si se leyó
 * un entero; 0 si la entrada no es numérica; -1 ante EOF (cierre ordenado).
 * Ninguna entrada inválida termina el programa. Patrón idéntico al lector
 * de main.c; la fase F8 lo unificará en el módulo validacion.
 */
static int leer_entero(int *salida)
{
    int leido = scanf("%d", salida);

    if (leido == EOF) {
        return -1;
    }
    if (leido != 1) {
        printf("Entrada inválida.\n");
        descartar_linea();
        return 0;
    }
    descartar_linea();
    return 1;
}

bool resultados_validar(const Torneo *t, const RegistroEntrenadores *reg,
                        const ResultadoCargado *r, char *msg, size_t n)
{
    const Combate *c;
    int id1;
    int id2;
    int es_eliminatoria;

    if (t == NULL || reg == NULL || r == NULL || msg == NULL || n == 0) {
        return false;
    }
    if (t->estado == TORNEO_SIN_INICIAR) {
        snprintf(msg, n, "El torneo no está armado (se requieren %d "
                         "entrenadores registrados).", MAX_ENTRENADORES);
        return false;
    }
    if (t->estado == TORNEO_FINALIZADO) {
        snprintf(msg, n, "El torneo ya finalizó; no se aceptan más "
                         "resultados.");
        return false;
    }
    if (r->numero < 1 || r->numero > TOTAL_COMBATES) {
        snprintf(msg, n, "Número de combate inválido: %d.", r->numero);
        return false;
    }

    /* Ronda correcta: la eliminatoria solo admite registros en ese estado. */
    es_eliminatoria = (r->numero > COMBATES_GRUPO);
    if (es_eliminatoria && t->estado != TORNEO_ELIMINATORIAS) {
        snprintf(msg, n, "La fase de grupos no está completa: faltan "
                         "resultados de los combates 1-48.");
        return false;
    }

    c = &t->combates[r->numero - 1];
    if (c->estado != RES_PENDIENTE) {
        snprintf(msg, n, "El combate %d ya tiene resultado.", r->numero);
        return false;
    }

    /* Entrenadores existentes (catálogo §8.1: RF-RES-02). */
    if (entrenador_por_id(reg, r->id_entrenador1) == NULL) {
        snprintf(msg, n, "El entrenador %d del combate %d no está "
                         "registrado.", r->id_entrenador1, r->numero);
        return false;
    }
    if (entrenador_por_id(reg, r->id_entrenador2) == NULL) {
        snprintf(msg, n, "El entrenador %d del combate %d no está "
                         "registrado.", r->id_entrenador2, r->numero);
        return false;
    }

    /* Participantes resueltos por el sistema (RF-RES-03): el usuario nunca
       los inventa. Esto cubre también "eliminado no reaparece": los ids
       resueltos son los únicos válidos para cada combate (§8.1). */
    torneo_participantes(t, r->numero, &id1, &id2);
    if (id1 == 0 || id2 == 0) {
        snprintf(msg, n, "El combate %d no está disponible aún: sus "
                         "fuentes (clasificados o G#/P#) no están "
                         "resueltas.", r->numero);
        return false;
    }
    if (r->id_entrenador1 != id1 || r->id_entrenador2 != id2) {
        const Entrenador *e1 = entrenador_por_id(reg, id1);
        const Entrenador *e2 = entrenador_por_id(reg, id2);
        snprintf(msg, n, "Los participantes no coinciden con los resueltos "
                         "por el sistema (RF-RES-03): el combate %d "
                         "enfrenta a %s (id %d) contra %s (id %d).",
                 r->numero, e1 != NULL ? e1->nombre : "?", id1,
                 e2 != NULL ? e2->nombre : "?", id2);
        return false;
    }

    if (r->resultado != RES_V1 && r->resultado != RES_V2 &&
        r->resultado != RES_EMPATE) {
        snprintf(msg, n, "Resultado inválido para el combate %d.",
                 r->numero);
        return false;
    }
    if (es_eliminatoria && r->resultado == RES_EMPATE) {
        snprintf(msg, n, "La eliminatoria no admite empates (RF-ELM-01).");
        return false;
    }
    if (r->kos1 < 0 || r->kos2 < 0) {
        snprintf(msg, n, "Los KOs del combate %d no pueden ser negativos.",
                 r->numero);
        return false;
    }

    /* Ganador perteneciente al combate y coherente con el resultado. */
    if (r->resultado == RES_V1 && r->id_ganador != id1) {
        snprintf(msg, n, "El ganador no coincide con el resultado (V1): "
                         "debe ser el entrenador %d.", id1);
        return false;
    }
    if (r->resultado == RES_V2 && r->id_ganador != id2) {
        snprintf(msg, n, "El ganador no coincide con el resultado (V2): "
                         "debe ser el entrenador %d.", id2);
        return false;
    }
    if (r->resultado == RES_EMPATE && r->id_ganador != 0) {
        snprintf(msg, n, "Un empate no declara ganador.");
        return false;
    }
    return true;
}

/**
 * Valida y aplica un resultado: si falla imprime el motivo específico y no
 * aplica nada; si pasa, delega en torneo_aplicar_resultado e imprime su
 * mensaje.
 *
 * @param t   Puntero al estado del torneo (no debe ser NULL).
 * @param reg Puntero al registro de entrenadores (no debe ser NULL).
 * @param r   Puntero al resultado ya parseado (no debe ser NULL).
 * @return true si el resultado se aplicó; false si fue rechazado.
 */
static bool aplicar_resultado(Torneo *t, RegistroEntrenadores *reg,
                              const ResultadoCargado *r)
{
    char msg[256];

    if (!resultados_validar(t, reg, r, msg, sizeof(msg))) {
        printf("%s\n", msg);
        return false;
    }
    if (!torneo_aplicar_resultado(t, reg, r, msg, sizeof(msg))) {
        printf("Error al aplicar: %s\n", msg);
        return false;
    }
    printf("%s\n", msg);
    return true;
}

bool resultados_cargar_teclado(Torneo *t, RegistroEntrenadores *reg)
{
    int numero;
    int aplicados = 0;

    if (t == NULL || reg == NULL) {
        return false;
    }
    printf("--- Cargar resultados por teclado ---\n");
    for (;;) {
        ResultadoCargado r;
        int id1;
        int id2;
        int res_opcion;
        int leido;
        const Entrenador *e1;
        const Entrenador *e2;

        printf("Número de combate (1-%d, 0 para terminar): ", TOTAL_COMBATES);
        leido = leer_entero(&numero);
        if (leido == -1 || numero == 0) {
            break;   /* EOF o "terminar": cierre ordenado */
        }
        if (leido == 0) {
            continue;   /* entrada no numérica: reintentar */
        }
        if (numero < 1 || numero > TOTAL_COMBATES) {
            printf("Número de combate inválido (1-%d).\n", TOTAL_COMBATES);
            continue;
        }

        /* El sistema resuelve los participantes (RF-RES-03). */
        torneo_participantes(t, numero, &id1, &id2);
        if (id1 == 0 || id2 == 0) {
            printf("El combate %d no está disponible aún: sus fuentes "
                   "(clasificados o G#/P#) no están resueltas.\n", numero);
            continue;
        }
        e1 = entrenador_por_id(reg, id1);
        e2 = entrenador_por_id(reg, id2);
        printf("Combate %d: %s (id %d) vs %s (id %d)\n", numero,
               e1 != NULL ? e1->nombre : "?", id1,
               e2 != NULL ? e2->nombre : "?", id2);

        if (numero <= COMBATES_GRUPO) {
            printf("Resultado (1=V1, 2=V2, 3=Empate, 0=terminar): ");
        } else {
            printf("Resultado (1=V1, 2=V2, 0=terminar): ");
        }
        leido = leer_entero(&res_opcion);
        if (leido == -1 || res_opcion == 0) {
            break;   /* EOF o "terminar" */
        }
        if (leido == 0) {
            continue;
        }
        if (res_opcion < 1 || res_opcion > 3 ||
            (numero > COMBATES_GRUPO && res_opcion == 3)) {
            printf("Resultado inválido para este combate.\n");
            continue;
        }

        printf("KOs del entrenador %d (>=0): ", id1);
        leido = leer_entero(&r.kos1);
        if (leido == -1) {
            break;
        }
        if (leido == 0) {
            continue;
        }
        if (r.kos1 < 0) {
            printf("Los KOs no pueden ser negativos.\n");
            continue;
        }
        printf("KOs del entrenador %d (>=0): ", id2);
        leido = leer_entero(&r.kos2);
        if (leido == -1) {
            break;
        }
        if (leido == 0) {
            continue;
        }
        if (r.kos2 < 0) {
            printf("Los KOs no pueden ser negativos.\n");
            continue;
        }

        r.numero = numero;
        r.id_entrenador1 = id1;
        r.id_entrenador2 = id2;
        r.resultado = (res_opcion == 1) ? RES_V1
                      : (res_opcion == 2) ? RES_V2 : RES_EMPATE;
        r.id_ganador = (r.resultado == RES_EMPATE) ? 0
                       : (r.resultado == RES_V1) ? id1 : id2;
        if (aplicar_resultado(t, reg, &r)) {
            aplicados++;
        }
    }
    printf("Resultados cargados por teclado: %d aplicados.\n", aplicados);
    return aplicados > 0;
}

bool resultados_cargar_archivo(Torneo *t, RegistroEntrenadores *reg,
                               const char *ruta)
{
    FILE *archivo;
    char linea[TAM_MAX_LINEA];
    int numero_linea = 0;
    int aplicados = 0;
    int rechazadas = 0;

    if (t == NULL || reg == NULL || ruta == NULL) {
        return false;
    }
    archivo = fopen(ruta, "r");
    if (archivo == NULL) {
        printf("Error: no se pudo abrir el archivo de resultados: %s\n",
               ruta);
        return false;
    }

    while (fgets(linea, (int)sizeof(linea), archivo) != NULL) {
        ResultadoCargado r;
        char *tok;
        char *tok_ganador;
        size_t largo;

        numero_linea++;
        largo = strlen(linea);
        while (largo > 0 && (linea[largo - 1] == '\n' ||
                             linea[largo - 1] == '\r')) {
            linea[--largo] = '\0';
        }
        if (largo == 0) {
            continue;   /* linea vacia: no cuenta */
        }

        /* Esquema §5.3: NUM;ID1;ID2;RESULTADO;GANADOR[;KOS1;KOS2].
           Los 5 primeros campos son el mínimo de RF-RES-04: si la línea
           trae solo 5, los KOs valen 0. */
        tok = strtok(linea, ";");
        if (tok == NULL) {
            goto linea_invalida;
        }
        r.numero = atoi(tok);
        tok = strtok(NULL, ";");
        if (tok == NULL) {
            goto linea_invalida;
        }
        r.id_entrenador1 = atoi(tok);
        tok = strtok(NULL, ";");
        if (tok == NULL) {
            goto linea_invalida;
        }
        r.id_entrenador2 = atoi(tok);
        tok = strtok(NULL, ";");
        if (tok == NULL) {
            goto linea_invalida;
        }
        if (strcmp(tok, "V1") == 0) {
            r.resultado = RES_V1;
        } else if (strcmp(tok, "V2") == 0) {
            r.resultado = RES_V2;
        } else if (strcmp(tok, "E") == 0) {
            r.resultado = RES_EMPATE;
        } else {
            goto linea_invalida;
        }
        tok = strtok(NULL, ";");
        if (tok == NULL) {
            goto linea_invalida;
        }
        tok_ganador = tok;
        r.id_ganador = (strcmp(tok_ganador, "-") == 0) ? 0 : atoi(tok_ganador);

        tok = strtok(NULL, ";");
        if (tok == NULL) {
            r.kos1 = 0;   /* solo 5 campos: KOs por defecto en 0 */
            r.kos2 = 0;
        } else {
            r.kos1 = atoi(tok);
            tok = strtok(NULL, ";");
            if (tok == NULL) {
                goto linea_invalida;   /* 6 campos: malformado */
            }
            r.kos2 = atoi(tok);
            if (strtok(NULL, ";") != NULL) {
                goto linea_invalida;   /* mas de 7 campos */
            }
        }

        if (aplicar_resultado(t, reg, &r)) {
            aplicados++;
        } else {
            rechazadas++;
        }
        continue;

linea_invalida:
        printf("Línea %d rechazada: campos inválidos o resultado "
               "desconocido.\n", numero_linea);
        rechazadas++;
    }
    fclose(archivo);
    printf("Resultados cargados: %d aplicados, %d rechazadas.\n",
           aplicados, rechazadas);
    return aplicados > 0;
}

void resultados_mostrar_pendientes(const Torneo *t)
{
    int k;
    int pend_grupos = 0;
    int pend_elim = 0;

    if (t == NULL || t->estado == TORNEO_SIN_INICIAR) {
        return;
    }
    for (k = 0; k < TOTAL_COMBATES; k++) {
        if (t->combates[k].estado == RES_PENDIENTE) {
            if (k < COMBATES_GRUPO) {
                pend_grupos++;
            } else {
                pend_elim++;
            }
        }
    }
    printf("Combates pendientes: %d de 48 (fase de grupos), %d de 16 "
           "(eliminatoria).\n", pend_grupos, pend_elim);
}
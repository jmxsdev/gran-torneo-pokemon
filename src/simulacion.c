/**
 * @file simulacion.c
 * @brief Simulación automática del torneo: relleno de equipos por
 *        backtracking (D18) y ejecución de los 64 combates (RF-SIM-01..04).
 * @author Nombre del estudiante
 * @date 2026-11-16
 */

#include <stdbool.h>
#include <stdio.h>

#include "simulacion.h"
#include "combate.h"
#include "constantes.h"
#include "entrenador.h"
#include "equipo.h"

/* Nivel medio del relleno automático (D18): equipos no básicos. */
#define NIVEL_MEDIO 30
/* Ventana rotativa determinista de especies (D18): 24 especies, paso 4. */
#define ANCHO_VENTANA 24
#define PASO_VENTANA 4

/**
 * Selecciona el primer Pokémon disponible (D11): devuelve la posición 1 de
 * los ejemplares con hp_actual > 0. Determinista y sin consola.
 */
static int seleccionar_primero_disponible(const Entrenador *ent, const char *rol)
{
    const Ejemplar *ej;
    int i = 0;

    (void)rol;
    if (ent == NULL) {
        return -1;
    }
    for (ej = ent->equipo; ej != NULL; ej = ej->siguiente) {
        if (ej->hp_actual > 0) {
            i++;
            if (i == 1) {
                return 1;
            }
        }
    }
    return -1;
}

/**
 * Rellena por backtracking los equipos vacíos o inválidos (D18).
 *
 * Para cada entrenador sin equipo válido arma una ventana de especies
 * rotativa (ANCHO_VENTANA especies desde pos * PASO_VENTANA, con envoltura)
 * y llama equipo_formar_backtracking con nivel medio NIVEL_MEDIO. Los equipos
 * válidos existentes no se modifican.
 *
 * @param pd    Pokédex cargada (no NULL).
 * @param reg   Registro de entrenadores (no NULL).
 * @param exito true si todos los equipos quedaron válidos; false si falló
 *              una generación (no debería ocurrir con restricciones laxas).
 */
static void simulacion_preparar_equipos(const Pokedex *pd,
                                        RegistroEntrenadores *reg,
                                        bool *exito)
{
    int i;

    for (i = 0; i < reg->cantidad; i++) {
        Entrenador *ent = &reg->entrenadores[i];
        RestriccionesEquipo r;
        Ejemplar *equipo = NULL;
        int cantidad = 0;
        bool ok;
        int j;

        if (equipo_validar(pd, ent, MAX_EQUIPO)) {
            continue;   /* equipo válido existente: no se toca */
        }

        r.cantidad = MAX_EQUIPO;
        r.nivel_total_max = MAX_EQUIPO * NIVEL_MEDIO;
        r.min_tipos = 2;
        r.ataque_total_min = 0;
        r.nivel_objetivo = NIVEL_MEDIO;
        r.permitir_repetidas = false;
        r.cantidad_permitidas = ANCHO_VENTANA;
        for (j = 0; j < ANCHO_VENTANA; j++) {
            r.especies_permitidas[j] =
                ((i * PASO_VENTANA + j) % POKEDEX_MAX) + 1;
        }

        equipo_formar_backtracking(pd, &r, &equipo, &cantidad, &ok);
        if (!ok) {
            *exito = false;
            return;
        }
        equipo_asignar(ent, equipo);
    }
    *exito = true;
}

/* Implementación de simulacion_ejecutar: documentación canónica en simulacion.h. */
void simulacion_ejecutar(const Pokedex *pd, Torneo *t,
                         RegistroEntrenadores *reg, bool *exito)
{
    int k;
    bool ok;

    if (pd == NULL || t == NULL || reg == NULL) {
        *exito = false;
        return;
    }
    if (t->estado == TORNEO_SIN_INICIAR || t->estado == TORNEO_FINALIZADO) {
        *exito = false;
        return;
    }

    /* Rellena equipos faltantes antes de combatir (D18). */
    simulacion_preparar_equipos(pd, reg, &ok);
    if (!ok) {
        *exito = false;
        return;
    }

    for (k = 1; k <= TOTAL_COMBATES && t->estado != TORNEO_FINALIZADO; k++) {
        const Combate *c = &t->combates[k - 1];
        int id1;
        int id2;
        Entrenador *e1;
        Entrenador *e2;
        ResultadoCombate res;
        ResultadoCargado r;
        char msg[256];

        if (c->estado != RES_PENDIENTE) {
            continue;   /* respeta resultados parciales ya cargados */
        }

        torneo_participantes(t, k, &id1, &id2);
        if (id1 == 0 || id2 == 0) {
            continue;   /* fuente del bracket aún no resuelta */
        }

        e1 = entrenador_buscar(reg, id1);
        e2 = entrenador_buscar(reg, id2);
        if (e1 == NULL || e2 == NULL) {
            *exito = false;
            return;
        }

        combate_ejecutar(e1, e2, k > COMBATES_GRUPO, true,
                         seleccionar_primero_disponible, &res, &ok);
        if (!ok) {
            *exito = false;
            return;
        }

        r.numero = k;
        r.id_entrenador1 = id1;
        r.id_entrenador2 = id2;
        r.resultado = res.empate ? RES_EMPATE
                      : (res.id_ganador == id1 ? RES_V1 : RES_V2);
        r.id_ganador = res.empate ? 0 : res.id_ganador;
        r.kos1 = res.kos_local;
        r.kos2 = res.kos_visita;

        torneo_aplicar_resultado(t, reg, &r, msg, sizeof(msg), &ok);
        if (!ok) {
            *exito = false;
            return;
        }
    }
    *exito = true;
}

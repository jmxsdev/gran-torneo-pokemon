/**
 * @file combate.c
 * @brief Reglamento del combate 1 vs 1: daño (D1), orden (D3), KO y
 *        reemplazo (RF-CMB-04), empate en grupos (D4) y anti-empate en
 *        eliminatoria (D5) — RF-CMB-01..05, diseño §7.
 * @author <Nombre del estudiante>
 * @date 2026-09-21
 */

#include <stdio.h>

#include "combate.h"
#include "tipos.h"

/**
 * Restaura hp_actual = hp_max de todos los ejemplares del equipo (D2: un
 * combate siempre arranca con equipos íntegros).
 */
static void combate_restaurar_hp(Entrenador *ent)
{
    Ejemplar *ej;
    for (ej = ent->equipo; ej != NULL; ej = ej->siguiente) {
        ej->hp_actual = ej->hp_max;
    }
}

/**
 * Cuenta los ejemplares disponibles (hp_actual > 0) del equipo.
 */
static int combate_disponibles(const Entrenador *ent)
{
    const Ejemplar *ej;
    int n = 0;
    for (ej = ent->equipo; ej != NULL; ej = ej->siguiente) {
        if (ej->hp_actual > 0) {
            n++;
        }
    }
    return n;
}

/**
 * Devuelve el ejemplar en la posicion indicada (1..N) entre los disponibles;
 * NULL si la posicion no existe o el equipo es invalido.
 */
static Ejemplar *combate_ejemplar_posicion(Entrenador *ent, int posicion)
{
    Ejemplar *ej;
    int i = 0;
    if (ent == NULL || posicion < 1) {
        return NULL;
    }
    for (ej = ent->equipo; ej != NULL; ej = ej->siguiente) {
        if (ej->hp_actual > 0) {
            i++;
            if (i == posicion) {
                return ej;
            }
        }
    }
    return NULL;
}

/**
 * Pide al llamador la posicion del Pokemon activo y la convierte en el
 * ejemplar correspondiente. Devuelve NULL ante cancelacion (EOF).
 */
static Ejemplar *combate_seleccionar_activo(Entrenador *ent, const char *rol,
                                            CombateSeleccionar seleccionar)
{
    int posicion = seleccionar(ent, rol);
    if (posicion < 1) {
        return NULL;
    }
    return combate_ejemplar_posicion(ent, posicion);
}

/**
 * @brief Calcula el daño de un ataque del ejemplar atacante al defensor.
 *
 * Fórmula cerrada D1 con enteros en ese orden: base = (2*nivel/5 + 2) *
 * ataque / defensa + 2; mult = tipos_multiplicador(tipo primario del
 * atacante, tipo(s) del defensor); daño = (int)(base * mult). Mínimo 1 si
 * mult > 0 y daño < 1; 0 si mult == 0 (RF-CMB-03/RF-CMB-05). Solo lee
 * campos de los ejemplares: nunca muta la Pokédex (RF-PDX-04).
 *
 * @param atacante Ejemplar que ataca (no debe ser NULL).
 * @param defensor Ejemplar que recibe (no debe ser NULL).
 * @return Daño calculado, entero >= 0; 0 si los parámetros son inválidos.
 */
int combate_calcular_danio(const Ejemplar *atacante, const Ejemplar *defensor)
{
    int base;
    float mult;

    if (atacante == NULL || defensor == NULL || defensor->defensa <= 0) {
        return 0;
    }
    base = (2 * atacante->nivel / 5 + 2) * atacante->ataque /
           defensor->defensa + 2;
    mult = tipos_multiplicador(atacante->tipo_primario,
                               defensor->tipo_primario,
                               defensor->tipo_secundario);
    if (mult > 0.0f && (int)(base * mult) < 1) {
        return 1;
    }
    if (mult == 0.0f) {
        return 0;
    }
    return (int)(base * mult);
}

/**
 * @brief Decide qué ejemplar ataca primero según su velocidad (D3).
 *
 * Mayor velocidad ataca primero; empate => ataca el ejemplar del entrenador
 * local (entrenador 1 del enfrentamiento, RF-CMB-02).
 *
 * @param local  Ejemplar activo del entrenador local (no debe ser NULL).
 * @param visita Ejemplar activo del entrenador visitante (no debe ser NULL).
 * @return true si ataca primero *local; false si ataca primero *visita.
 */
bool combate_ataca_primero(const Ejemplar *local, const Ejemplar *visita)
{
    if (local == NULL || visita == NULL) {
        return true;
    }
    return local->velocidad >= visita->velocidad;
}

/**
 * Aplica un ataque dentro de un turno: calcula el daño, lo imprime en la
 * traza, lo aplica a hp_actual (piso 0) y, ante KO, suma el derrotado al
 * atacante y pide un reemplazo al defensor (si no tiene, el atacante gana).
 * Devuelve el id del ganador (0 si el combate continúa; -1 si se cancela).
 */
static int combate_atacar(int turno, Entrenador *atacante_ent,
                          Entrenador *defensor_ent, Ejemplar *atacante,
                          Ejemplar **defensor, bool atacante_es_local,
                          CombateSeleccionar seleccionar,
                          ResultadoCombate *res)
{
    int danio = combate_calcular_danio(atacante, *defensor);

    (*defensor)->hp_actual -= danio;
    if ((*defensor)->hp_actual < 0) {
        (*defensor)->hp_actual = 0;
    }
    if (danio > 0) {
        printf("Turno %d: %s ataca a %s y causa %d de daño (HP %d/%d).\n",
               turno, atacante->nombre, (*defensor)->nombre, danio,
               (*defensor)->hp_actual, (*defensor)->hp_max);
    } else {
        printf("Turno %d: %s ataca a %s: sin efecto (HP %d/%d).\n",
               turno, atacante->nombre, (*defensor)->nombre,
               (*defensor)->hp_actual, (*defensor)->hp_max);
    }
    if ((*defensor)->hp_actual > 0) {
        return 0;
    }

    /* KO (RF-CMB-04): el atacante suma un derrotado y el defensor elige
       reemplazo entre sus disponibles; sin disponibles, el atacante gana. */
    printf("¡%s ha sido debilitado!\n", (*defensor)->nombre);
    if (atacante_es_local) {
        res->kos_local++;
    } else {
        res->kos_visita++;
    }
    if (combate_disponibles(defensor_ent) == 0) {
        return atacante_ent->id;
    }
    *defensor = combate_seleccionar_activo(defensor_ent, "reemplazo",
                                           seleccionar);
    if (*defensor == NULL) {
        return -1;   /* entrada terminada: combate cancelado */
    }
    return 0;
}

/**
 * Suma el HP actual de todos los ejemplares del equipo (criterio 1 de D5).
 */
static int combate_total_hp(const Entrenador *ent)
{
    const Ejemplar *ej;
    int total = 0;
    for (ej = ent->equipo; ej != NULL; ej = ej->siguiente) {
        total += ej->hp_actual;
    }
    return total;
}

/**
 * Suma el nivel de todos los ejemplares del equipo (criterio 2 de D5).
 */
static int combate_total_nivel(const Entrenador *ent)
{
    const Ejemplar *ej;
    int total = 0;
    for (ej = ent->equipo; ej != NULL; ej = ej->siguiente) {
        total += ej->nivel;
    }
    return total;
}

/**
 * @brief Ejecuta un combate completo entre dos equipos (RF-CMB-01..05).
 *
 * Restaura hp_actual = hp_max (D2), pide el Pokémon inicial de cada
 * entrenador y aplica hasta MAX_TURNOS_COMBATE intercambios completos:
 * cada turno atacan ambos activos según el orden de velocidad (D3) y los
 * KOs se reemplazan con otro disponible (RF-CMB-04). El combate termina
 * cuando un entrenador se queda sin Pokémon disponibles (gana el otro), o
 * al agotar los turnos: empate en fase de grupos (D4) o anti-empate en
 * eliminatoria por la cadena HP total -> nivel total -> entrenador 1 (D5).
 *
 * @param local         Entrenador local (entrenador 1; no debe ser NULL).
 * @param visita        Entrenador visitante (no debe ser NULL).
 * @param es_eliminatoria true si el combate es de eliminatoria (D5); false
 *                      si es de fase de grupos (D4, admite empate).
 * @param seleccionar   Función de selección del Pokémon activo (no NULL).
 * @param res           Puntero donde se escribe el resultado (no NULL).
 * @return true si el combate se ejecutó y *res quedó definido; false si los
 *         parámetros son inválidos, un equipo está vacío o se canceló (EOF).
 */
bool combate_ejecutar(Entrenador *local, Entrenador *visita,
                      bool es_eliminatoria, CombateSeleccionar seleccionar,
                      ResultadoCombate *res)
{
    Ejemplar *activo_local;
    Ejemplar *activo_visita;
    int turno;
    int ganador = 0;

    if (local == NULL || visita == NULL || seleccionar == NULL ||
        res == NULL) {
        return false;
    }
    if (equipo_contar(local) < 1 || equipo_contar(visita) < 1) {
        return false;
    }

    res->empate = false;
    res->id_ganador = 0;
    res->kos_local = 0;
    res->kos_visita = 0;

    combate_restaurar_hp(local);
    combate_restaurar_hp(visita);

    printf("=== Combate: %s vs %s (%s) ===\n", local->nombre, visita->nombre,
           es_eliminatoria ? "eliminatoria" : "fase de grupos");

    activo_local = combate_seleccionar_activo(local, "local", seleccionar);
    if (activo_local == NULL) {
        printf("Combate cancelado.\n");
        return false;
    }
    activo_visita = combate_seleccionar_activo(visita, "visita", seleccionar);
    if (activo_visita == NULL) {
        printf("Combate cancelado.\n");
        return false;
    }

    for (turno = 1; turno <= MAX_TURNOS_COMBATE && ganador == 0; turno++) {
        Ejemplar *primero;
        Ejemplar *segundo;
        bool local_primero;
        int g;

        /* Fin por agotamiento: un entrenador sin disponibles pierde; si
           ambos quedan sin Pokémon en el mismo turno (D4) se resuelve fuera
           del bucle (empate en grupos o cadena D5 en eliminatoria). */
        if (combate_disponibles(local) == 0 &&
            combate_disponibles(visita) == 0) {
            break;
        }
        if (combate_disponibles(local) == 0) {
            ganador = visita->id;
            break;
        }
        if (combate_disponibles(visita) == 0) {
            ganador = local->id;
            break;
        }

        /* Orden de ataque por velocidad (D3). */
        local_primero = combate_ataca_primero(activo_local, activo_visita);
        if (local_primero) {
            primero = activo_local;
            segundo = activo_visita;
        } else {
            primero = activo_visita;
            segundo = activo_local;
        }

        /* Intercambio completo: ataca primero y luego el segundo activo. */
        g = combate_atacar(turno, local_primero ? local : visita,
                           local_primero ? visita : local, primero, &segundo,
                           local_primero, seleccionar, res);
        if (g == -1) {
            printf("Combate cancelado.\n");
            return false;
        }
        if (g != 0) {
            ganador = g;
            break;
        }
        g = combate_atacar(turno, local_primero ? visita : local,
                           local_primero ? local : visita, segundo, &primero,
                           !local_primero, seleccionar, res);
        if (g == -1) {
            printf("Combate cancelado.\n");
            return false;
        }
        if (g != 0) {
            ganador = g;
            break;
        }

        activo_local = local_primero ? primero : segundo;
        activo_visita = local_primero ? segundo : primero;
    }

    if (ganador != 0) {
        /* Victoria por agotamiento: gana quien conserva Pokémon (RF-CMB-04). */
        res->id_ganador = ganador;
        printf("Victoria de %s: %s se queda sin Pokémon disponibles "
               "(KOs: %d a %d).\n",
               ganador == local->id ? local->nombre : visita->nombre,
               ganador == local->id ? visita->nombre : local->nombre,
               res->kos_local, res->kos_visita);
        return true;
    }

    /* 20 turnos completados con ambos equipos vivos (D4/D5). */
    if (es_eliminatoria) {
        int hp_local = combate_total_hp(local);
        int hp_visita = combate_total_hp(visita);
        int nv_local = combate_total_nivel(local);
        int nv_visita = combate_total_nivel(visita);

        if (hp_local > hp_visita) {
            ganador = local->id;
        } else if (hp_visita > hp_local) {
            ganador = visita->id;
        } else if (nv_local > nv_visita) {
            ganador = local->id;
        } else if (nv_visita > nv_local) {
            ganador = visita->id;
        } else {
            ganador = local->id;   /* criterio 3: entrenador 1 */
        }
        res->id_ganador = ganador;
        printf("Desempate en eliminatoria tras %d turnos: HP total %d-%d, "
               "nivel total %d-%d; gana %s.\n",
               MAX_TURNOS_COMBATE, hp_local, hp_visita, nv_local, nv_visita,
               ganador == local->id ? local->nombre : visita->nombre);
    } else {
        res->empate = true;
        printf("Empate tras %d turnos (fase de grupos).\n",
               MAX_TURNOS_COMBATE);
    }
    return true;
}
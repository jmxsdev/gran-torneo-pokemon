/**
 * @file probe_torneo_completo.c
 * @brief Probe de integración completa (F9.3, RF-PRB-01): torneo 32
 *        entrenadores → 48 resultados de grupos → clasificación →
 *        bracket 49–64 → campeón y cuatro primeros lugares.
 *
 * Compila contra los módulos reales (todos los .c de src/, sin main.c):
 *   gcc -std=c99 -Wall -Wextra -I src -o probe_torneo_completo \
 *       src/archivos.c src/combate.c src/entrenador.c src/equipo.c \
 *       src/pokedex.c src/resultados.c src/tipos.c src/torneo.c \
 *       src/validacion.c tests/probes/probe_torneo_completo.c
 *
 * El guion es determinista (D2/D3/D5 sin azar, bracket fijo): siempre gana
 * el participante 1 (V1) con 1 KO, por lo que las posiciones finales son
 * reproducibles. Además de la línea feliz verifica las rutas inválidas de
 * la eliminatoria: empate en 49–64 (RF-ELM-01), eliminado que no reaparece
 * (RF-RES-02), participantes manuales (RF-RES-03) y resultados tras
 * finalizar (RF-RES-02).
 *
 * @author <Nombre del estudiante>
 * @date 2026-09-21
 */

#include <stdio.h>
#include <string.h>

#include "constantes.h"
#include "pokedex.h"
#include "tipos.h"
#include "entrenador.h"
#include "equipo.h"
#include "torneo.h"
#include "archivos.h"

static int casos = 0;
static int fallos = 0;

static void verificar(int condicion, const char *nombre)
{
    casos++;
    printf("%s: %s\n", condicion ? "PASS" : "FAIL", nombre);
    if (!condicion) {
        fallos++;
    }
}

/* Aplica V1 (gana el participante 1) al combate 'numero' con los
   participantes resueltos por el sistema (RF-RES-03). */
static int aplicar_v1(Torneo *t, RegistroEntrenadores *reg, int numero,
                      char *msg, size_t n)
{
    ResultadoCargado r;
    torneo_participantes(t, numero, &r.id_entrenador1, &r.id_entrenador2);
    r.numero = numero;
    r.resultado = RES_V1;
    r.id_ganador = r.id_entrenador1;
    r.kos1 = 1;
    r.kos2 = 0;
    return torneo_aplicar_resultado(t, reg, &r, msg, n);
}

/* Aplica los 48 resultados de grupos (V1). */
static void completar_grupos(Torneo *t, RegistroEntrenadores *reg,
                             char *msg, size_t n)
{
    int k;
    for (k = 1; k <= 48; k++) {
        if (!aplicar_v1(t, reg, k, msg, n)) {
            printf("ERROR: no se aplicó el combate de grupos %d\n", k);
        }
    }
}

/* Aplica el bracket completo 49..64 (V1). */
static void completar_eliminatoria(Torneo *t, RegistroEntrenadores *reg,
                                   char *msg, size_t n)
{
    int k;
    for (k = 49; k <= TOTAL_COMBATES; k++) {
        if (!aplicar_v1(t, reg, k, msg, n)) {
            printf("ERROR: no se aplicó el combate de eliminatoria %d\n", k);
        }
    }
}

int main(void)
{
    Pokedex pd;
    RegistroEntrenadores reg;
    Torneo t;
    char msg[256];
    int id1;
    int id2;

    /* ---------- 1. Carga de datos (RF-PRB-01: Pokédex y registro) ------- */
    tipos_inicializar();
    verificar(pokedex_cargar(&pd, RUTA_POKEDEX),
              "carga: Pokédex con 150 especies");
    reg.cantidad = 0;
    verificar(archivos_cargar_entrenadores(&reg, &pd, RUTA_ENTRENADORES),
              "carga: archivo de entrenadores");
    verificar(reg.cantidad == MAX_ENTRENADORES,
              "carga: exactamente 32 entrenadores (D10)");

    /* ---------- 2. Armado de grupos y calendario (RF-TRN-02) ------------ */
    t.estado = TORNEO_SIN_INICIAR;
    verificar(torneo_armar_grupos(&t, &reg), "grupos: torneo armado (8x4)");
    verificar(t.estado == TORNEO_GRUPOS, "grupos: estado GRUPOS");
    torneo_participantes(&t, 1, &id1, &id2);
    verificar(id1 == 1 && id2 == 2, "calendario: combate 1 = Ash(1) vs Misty(2)");
    torneo_participantes(&t, 6, &id1, &id2);
    verificar(id1 == 3 && id2 == 4, "calendario: combate 6 = Brock(3) vs Gary(4)");
    torneo_participantes(&t, 48, &id1, &id2);
    verificar(id1 == 31 && id2 == 32,
              "calendario: combate 48 = Sophocles(31) vs Gladion(32)");

    /* ---------- 3. Resultados de grupos con validaciones (RF-RES) ------- */
    verificar(!aplicar_v1(&t, &reg, 49, msg, sizeof(msg)),
              "resultados: combate 49 rechazado con grupos incompletos");
    verificar(strstr(msg, "fase de grupos no está completa") != NULL,
              "resultados: mensaje de ronda incorrecta");

    completar_grupos(&t, &reg, msg, sizeof(msg));
    verificar(t.estado == TORNEO_ELIMINATORIAS,
              "avance: GRUPOS->ELIMINATORIAS al completar 48/48");
    verificar(strstr(msg, "comienza la eliminatoria") != NULL,
              "avance: mensaje de transición a eliminatoria");

    /* Clasificados automáticos (RF-TRN-06): con V1 en todos los combates
       cada grupo queda 3V/2V/1V/0V, luego 1X y 2X por orden de registro. */
    verificar(t.id_clasificados[0] == 1 && t.id_clasificados[1] == 2,
              "clasificados: 1A=1 y 2A=2");
    verificar(t.id_clasificados[14] == 29 && t.id_clasificados[15] == 30,
              "clasificados: 1H=29 y 2H=30");

    /* Octavos fijos del bracket (RF-ELM-02). */
    torneo_participantes(&t, 49, &id1, &id2);
    verificar(id1 == 1 && id2 == 6, "octavos: 49 = 1A(1) vs 2B(6)");
    torneo_participantes(&t, 56, &id1, &id2);
    verificar(id1 == 29 && id2 == 26, "octavos: 56 = 1H(29) vs 2G(26)");

    /* ---------- 4. Eliminatoria: rutas inválidas (RF-ELM/RF-RES) -------- */
    /* Empate en eliminatoria (RF-ELM-01). */
    {
        ResultadoCargado r;
        torneo_participantes(&t, 57, &r.id_entrenador1, &r.id_entrenador2);
        r.numero = 57;
        r.resultado = RES_EMPATE;
        r.id_ganador = 0;
        r.kos1 = 1;
        r.kos2 = 1;
        verificar(!torneo_aplicar_resultado(&t, &reg, &r, msg, sizeof(msg)),
                  "eliminatoria: empate en 57 rechazado (RF-ELM-01)");
    }
    /* Participantes manuales (RF-RES-03). */
    {
        ResultadoCargado r;
        r.numero = 57;
        r.id_entrenador1 = 1;
        r.id_entrenador2 = 10;
        r.resultado = RES_V1;
        r.id_ganador = 1;
        r.kos1 = 1;
        r.kos2 = 0;
        verificar(!torneo_aplicar_resultado(&t, &reg, &r, msg, sizeof(msg)),
                  "eliminatoria: participantes inventados rechazados (RF-RES-03)");
    }

    completar_eliminatoria(&t, &reg, msg, sizeof(msg));
    verificar(t.estado == TORNEO_FINALIZADO,
              "final: ELIMINATORIAS->FINALIZADO al aplicar el 64");

    /* Campeón y posiciones (RF-ELM-05): con V1 en todo el bracket,
       campeón = G64 = 1 (Ash), subcampeón = P64 = 17, tercero = G63 = 5,
       cuarto = P63 = 21. */
    verificar(t.combates[63].id_ganador == 1, "campeón: G64 = entrenador 1");
    {
        const Combate *c64 = &t.combates[63];
        int sub = (c64->id_ganador == c64->id_entrenador1)
                      ? c64->id_entrenador2
                      : c64->id_entrenador1;
        verificar(sub == 17, "posiciones: subcampeón = P64 = 17");
    }
    verificar(t.combates[62].id_ganador == 5, "posiciones: tercero = G63 = 5");
    {
        const Combate *c63 = &t.combates[62];
        int cuarto = (c63->id_ganador == c63->id_entrenador1)
                         ? c63->id_entrenador2
                         : c63->id_entrenador1;
        verificar(cuarto == 21, "posiciones: cuarto = P63 = 21");
    }

    /* Eliminado que no reaparece (RF-RES-02): el perdedor de 49 (id 6) no
       puede participar en 58, cuyos participantes resueltos son 1 y 9. */
    {
        ResultadoCargado r;
        r.numero = 58;
        r.id_entrenador1 = 6;
        r.id_entrenador2 = 13;
        r.resultado = RES_V1;
        r.id_ganador = 6;
        r.kos1 = 1;
        r.kos2 = 0;
        verificar(!torneo_aplicar_resultado(&t, &reg, &r, msg, sizeof(msg)),
                  "eliminación: perdedor de 49 no reaparece en 58 (RF-RES-02)");
    }

    /* Tras finalizar no se aceptan más resultados (RF-RES-02). */
    {
        ResultadoCargado r;
        r.numero = 63;
        r.id_entrenador1 = 5;
        r.id_entrenador2 = 21;
        r.resultado = RES_V1;
        r.id_ganador = 5;
        r.kos1 = 1;
        r.kos2 = 0;
        verificar(!torneo_aplicar_resultado(&t, &reg, &r, msg, sizeof(msg)),
                  "final: aplicar 63 tras finalizar se rechaza");
    }

    /* La eliminatoria no altera los contadores de grupos (RF-TRN-03). */
    {
        const Entrenador *e1 = entrenador_buscar(&reg, 1);
        verificar(e1 != NULL && e1->victorias == 3 && e1->puntos == 9,
                  "grupos: contadores 3V/9pt de Ash intactos tras la eliminatoria");
    }

    /* ---------- 5. Consultas de estado (opciones 10 y 11) --------------- */
    printf("[RESULTADOS DEL TORNEO]\n");
    torneo_mostrar_resultados(&t, &reg);
    printf("[CAMPEON]\n");
    torneo_mostrar_campeon(&t, &reg);
    printf("[POSICIONES FINALES]\n");
    torneo_mostrar_posiciones_finales(&t, &reg);

    printf("\nPROBE TORNEO COMPLETO: %d/%d comprobaciones PASS%s\n",
           casos - fallos, casos, fallos == 0 ? " (todas)" : "");
    return fallos == 0 ? 0 : 1;
}
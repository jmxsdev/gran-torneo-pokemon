/**
 * @file main.c
 * @brief Bucle del menú principal y flujo general del torneo (RF-MEN-01).
 * @author Nombre del estudiante
 * @date 2026-09-01
 */

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constantes.h"
#include "validacion.h"
#include "pokedex.h"
#include "tipos.h"
#include "entrenador.h"
#include "equipo.h"
#include "combate.h"
#include "torneo.h"
#include "resultados.h"
#include "archivos.h"

/* Marca de modificación de entrenadores en sesión: solo se guarda el
   archivo si el registro cambió (RF-ENT-03). Los resultados NO se
   autoguardan: se persisten vía archivos_guardar_resultados (F7). */
static bool entrenadores_sucios = false;

/**
 * Muestra el menú de las 12 opciones del torneo (RF-MEN-01).
 */
static void mostrar_menu(void)
{
    printf("\n=== EL GRAN TORNEO POKÉMON ===\n");
    printf("1. Consultar Pokédex\n");
    printf("2. Registrar entrenador\n");
    printf("3. Crear equipo\n");
    printf("4. Consultar entrenadores\n");
    printf("5. Consultar equipos\n");
    printf("6. Cargar resultados\n");
    printf("7. Consultar clasificación\n");
    printf("8. Consultar enfrentamientos\n");
    printf("9. Consultar historial de combates\n");
    printf("10. Mostrar resultados del torneo\n");
    printf("11. Mostrar campeón\n");
    printf("12. Salir\n");
}

/* Lee la opción del menú con la API común (RF-TEC-03); 0 ante EOF. */
static int leer_opcion(void)
{
    return validar_leer_entero_msg("Seleccione una opción (1-12): ", 1, 12,
                                   "Opción inválida. Intente de nuevo.");
}

/* Lee un id de entrenador (el dominio lo valida el llamador); 0 ante EOF. */
static int leer_id_entrenador(const char *prompt)
{
    return validar_leer_entero(prompt, INT_MIN, INT_MAX);
}

/**
 * Muestra el resultado de una busqueda en la Pokédex: si se encontro la
 * especie muestra su ficha completa; si no, informa que no existe (RF-PDX-05).
 */
static void mostrar_resultado_busqueda(const Especie *esp)
{
    if (esp == NULL) {
        printf("No se encontró la especie solicitada.\n");
        return;
    }
    pokedex_mostrar_especie(esp);
}

/**
 * @brief Submenu de la opcion 1: consultar la Pokédex (RF-PDX-05).
 *
 * Ofrece mostrar todas, buscar por numero, buscar por nombre y consultar la
 * ficha completa de una especie. Las consultas no modifican ningun dato.
 */
static void consultar_pokedex(const Pokedex *pd)
{
    /* Cierre del hallazgo F1 (WARNING): si la Pokédex no está cargada, el
       submenú completo avisa y vuelve al menú en lugar de responder
       "No se encontró la especie solicitada." en las ramas 2/3/4
       (RF-PDX-06, RF-TEC-03). */
    if (pd->cantidad == 0) {
        printf("La Pokédex no está cargada.\n");
        return;
    }

    for (;;) {
        printf("\n--- Consultar Pokédex ---\n");
        printf("1. Mostrar todas las especies\n");
        printf("2. Buscar por número\n");
        printf("3. Buscar por nombre\n");
        printf("4. Consultar ficha completa\n");
        printf("0. Volver al menú principal\n");

        int opcion = validar_leer_entero_msg(
            "Seleccione una opción: ", 0, 4,
            "Opción inválida. Intente de nuevo.");

        if (opcion == 0) {
            printf("\n");
            return;   /* "volver" o EOF: cierre ordenado */
        }

        switch (opcion) {
        case 1:
            pokedex_mostrar_todas(pd);
            break;
        case 2:
        case 4: {
            int numero;
            numero = validar_leer_entero("Ingrese el número de especie: ",
                                         1, POKEDEX_MAX);
            if (numero == 0) {
                break;   /* EOF: vuelve al submenú */
            }
            mostrar_resultado_busqueda(pokedex_buscar_numero(pd, numero));
            break;
        }
        case 3: {
            char nombre[TAM_MAX_APODO * 2];
            bool exito;
            validar_leer_cadena("Ingrese el nombre de la especie: ",
                                nombre, sizeof(nombre), &exito);
            if (!exito) {
                printf("\n");
                return;   /* EOF: cierre ordenado */
            }
            mostrar_resultado_busqueda(pokedex_buscar_nombre(pd, nombre));
            break;
        }
        default:
            printf("Opción inválida. Intente de nuevo.\n");
            break;
        }
    }
}

/**
 * @brief Opcion 2: registra un entrenador por teclado (RF-ENT-01/02).
 *
 * Valida id entero positivo y único y nombre no vacío; ninguna entrada
 * inválida termina el programa (RF-TEC-03).
 */
static void registrar_entrenador(RegistroEntrenadores *reg)
{
    int id;
    char nombre[TAM_MAX_NOMBRE];
    bool exito;

    id = leer_id_entrenador("Ingrese el id del entrenador: ");
    if (id == 0 && feof(stdin)) {
        return;   /* EOF: cierre ordenado */
    }
    if (id <= 0) {
        printf("Id inválido: debe ser un entero positivo.\n");
        return;
    }
    validar_leer_cadena("Ingrese el nombre del entrenador: ",
                        nombre, sizeof(nombre), &exito);
    if (!exito) {
        return;   /* EOF: cierre ordenado */
    }
    if (nombre[0] == '\0') {
        printf("El nombre no puede estar vacío.\n");
        return;
    }
    if (entrenador_buscar(reg, id) != NULL) {
        printf("Ya existe un entrenador con id %d; el registro se rechaza.\n",
               id);
        return;
    }
    entrenador_registrar(reg, id, nombre, &exito);
    if (!exito) {
        printf("No se pudo registrar: registro lleno (máximo %d).\n",
               MAX_ENTRENADORES);
        return;
    }
    entrenadores_sucios = true;
    printf("Entrenador registrado: id %d, %s.\n", id, nombre);
}

/**
 * @brief Formación automática de equipo por backtracking (RF-EQP-04).
 *
 * Pide las restricciones (cantidad 1..MAX_EQUIPO, nivel total máximo
 * positivo y mínimo de tipos 1..CANT_TIPOS) con reintentos (RF-TEC-03) y
 * delega en equipo_formar_backtracking. Si existe solución la asigna al
 * entrenador y muestra el equipo; si no, informa que no existe y conserva
 * el equipo anterior (nunca se devuelve un equipo inválido).
 */
static void formar_equipo_automatico(const Pokedex *pd, Entrenador *ent)
{
    RestriccionesEquipo restricciones;
    Ejemplar *equipo = NULL;
    int cantidad = 0;
    char msg_cantidad[64];
    char msg_tipos[64];
    bool exito;

    if (pd->cantidad == 0) {
        printf("La Pokédex no está cargada; no se puede formar un equipo.\n");
        return;
    }

    /* Mensajes específicos de rechazo (F8): se conservan los textos que la
       batería F4 verifica, con la API común validar_leer_entero_msg. */
    snprintf(msg_cantidad, sizeof(msg_cantidad),
             "Cantidad inválida: debe estar entre 1 y %d.", MAX_EQUIPO);
    snprintf(msg_tipos, sizeof(msg_tipos),
             "Cantidad de tipos inválida: debe estar entre 1 y %d.",
             CANT_TIPOS);

    restricciones.cantidad = validar_leer_entero_msg(
        "Cantidad de Pokémon (1-6): ", 1, MAX_EQUIPO, msg_cantidad);
    if (restricciones.cantidad == 0) {
        return;   /* EOF: cierre ordenado */
    }
    restricciones.nivel_total_max = validar_leer_entero_msg(
        "Nivel total máximo (entero positivo): ", 1, INT_MAX,
        "Nivel total inválido: debe ser positivo.");
    if (restricciones.nivel_total_max == 0) {
        return;   /* EOF: cierre ordenado */
    }
    restricciones.min_tipos = validar_leer_entero_msg(
        "Mínimo de tipos diferentes (1-18): ", 1, CANT_TIPOS, msg_tipos);
    if (restricciones.min_tipos == 0) {
        return;   /* EOF: cierre ordenado */
    }
    restricciones.ataque_total_min = 0;
    restricciones.permitir_repetidas = false;
    restricciones.cantidad_permitidas = 0;

    printf("Formando equipo con backtracking...\n");
    equipo_formar_backtracking(pd, &restricciones, &equipo, &cantidad, &exito);
    if (!exito) {
        printf("No existe un equipo que cumpla las restricciones indicadas.\n");
        return;
    }
    equipo_asignar(ent, equipo);
    entrenadores_sucios = true;
    printf("Equipo generado: %d ejemplar(es) que cumplen todas las "
           "restricciones.\n", cantidad);
    equipo_mostrar(pd, ent);
}

/**
 * @brief Opcion 3: crea el equipo de un entrenador (manual o automático).
 *
 * Ofrece un submenu: formacion manual (seleccionando especies y niveles)
 * o formacion automatica por backtracking (RF-EQP-04) con restricciones.
 * En el modo manual libera el equipo anterior si existe y agrega ejemplares
 * por numero de especie y nivel (1..100); el id del ejemplar lo asigna el
 * modulo de equipos (unico global). Termina con especie 0 o al llegar a
 * MAX_EQUIPO. Ninguna entrada invalida termina el programa (RF-TEC-03).
 */
static void crear_equipo(const Pokedex *pd, RegistroEntrenadores *reg)
{
    int modo;
    int id_ent;
    Entrenador *ent;

    printf("\n--- Crear equipo ---\n");
    printf("1. Formación manual\n");
    printf("2. Formación automática (backtracking)\n");
    printf("0. Volver al menú principal\n");
    modo = validar_leer_entero_msg(
        "Seleccione una opción: ", 0, 2,
        "Opción inválida. Intente de nuevo.");
    if (modo == 0) {
        printf("\n");
        return;   /* "volver" o EOF: cierre ordenado */
    }

    id_ent = leer_id_entrenador("Ingrese el id del entrenador: ");
    if (id_ent == 0 && feof(stdin)) {
        return;   /* EOF: cierre ordenado */
    }
    ent = entrenador_buscar(reg, id_ent);
    if (ent == NULL) {
        printf("No existe un entrenador con id %d.\n", id_ent);
        return;
    }

    if (modo == 2) {
        formar_equipo_automatico(pd, ent);
        return;
    }

    if (ent->equipo != NULL) {
        equipo_liberar(ent);
        printf("Equipo anterior liberado.\n");
    }
    entrenadores_sucios = true;

    for (;;) {
        int numero;
        int nivel;
        int id_ejemplar;
        char apodo[TAM_MAX_APODO];
        char msg_nivel[64];
        char prompt_nivel[64];
        const Especie *esp;
        Ejemplar *ej;
        bool exito;

        numero = validar_leer_entero(
            "Ingrese el número de especie (0 para terminar): ",
            INT_MIN, INT_MAX);
        if (numero == 0) {
            break;   /* "terminar" o EOF: cierre ordenado de la creación */
        }
        esp = pokedex_buscar_numero(pd, numero);
        if (esp == NULL) {
            printf("La especie %d no existe en la Pokédex.\n", numero);
            continue;
        }
        if (equipo_contar(ent) >= MAX_EQUIPO) {
            printf("El equipo ya tiene %d ejemplares (máximo); el nuevo "
                   "no se agregó.\n", MAX_EQUIPO);
            break;
        }

        snprintf(msg_nivel, sizeof(msg_nivel),
                 "Nivel inválido (rango %d-%d).", NIVEL_MIN, NIVEL_MAX);
        snprintf(prompt_nivel, sizeof(prompt_nivel),
                 "Ingrese el nivel del ejemplar (%d-%d): ",
                 NIVEL_MIN, NIVEL_MAX);
        nivel = validar_leer_entero_msg(prompt_nivel,
                                        NIVEL_MIN, NIVEL_MAX, msg_nivel);
        if (nivel == 0) {
            break;   /* EOF: se omite este ejemplar */
        }

        validar_leer_cadena(
                "Nombre/apodo (Enter = nombre de la especie): ",
                apodo, sizeof(apodo), &exito);
        if (!exito) {
            return;   /* EOF: cierre ordenado */
        }
        if (apodo[0] == '\0') {
            snprintf(apodo, sizeof(apodo), "%s", esp->nombre);
        }

        equipo_siguiente_id(&id_ejemplar);
        ej = equipo_crear_ejemplar(pd, numero, apodo, nivel, id_ejemplar);
        if (ej == NULL) {
            printf("No se pudo crear el ejemplar (especie, nivel o memoria).\n");
            continue;
        }
        equipo_agregar_ejemplar(ent, ej, &exito);
        if (!exito) {
            printf("El equipo está lleno; el ejemplar no se agregó.\n");
            free(ej);
            break;
        }
        printf("Ejemplar #%d %s (especie %s, nivel %d) agregado; equipo %d/%d.\n",
               ej->id, ej->nombre, esp->nombre, ej->nivel,
               equipo_contar(ent), MAX_EQUIPO);
    }

    if (equipo_validar(pd, ent, MAX_EQUIPO)) {
        printf("El equipo es válido (tamaño %d, especies y niveles correctos).\n",
               equipo_contar(ent));
    } else {
        printf("Aviso: el equipo no cumple las reglas (RF-EQP-05).\n");
    }
}

/**
 * @brief Opcion 5: consulta el equipo de un entrenador (RF-EQP-02/03).
 */
static void consultar_equipo(const Pokedex *pd, RegistroEntrenadores *reg)
{
    int id_ent;
    Entrenador *ent;

    id_ent = leer_id_entrenador("Ingrese el id del entrenador: ");
    if (id_ent == 0 && feof(stdin)) {
        return;   /* EOF: cierre ordenado */
    }
    ent = entrenador_buscar(reg, id_ent);
    if (ent == NULL) {
        printf("No existe un entrenador con id %d.\n", id_ent);
        return;
    }
    equipo_mostrar(pd, ent);
}

/**
 * @brief Selecciona el Pokémon activo de un entrenador (posicion 1..N).
 *
 * Implementa CombateSeleccionar: lista los ejemplares disponibles
 * (hp_actual > 0) y lee una posicion validada. Ante EOF devuelve -1 (el
 * combate se cancela); una entrada no numerica o fuera de rango se rechaza
 * y se vuelve a preguntar sin terminar el programa (RF-TEC-03).
 *
 * @param ent Entrenador que elige (no debe ser NULL).
 * @param rol Rol textual del turno ("local", "visita" o "reemplazo").
 * @return Posicion 1-based elegida entre los disponibles; -1 ante EOF o
 *         si el entrenador no tiene Pokemon disponibles.
 */
static int seleccionar_pokemon_activo(const Entrenador *ent, const char *rol)
{
    const Ejemplar *ej;
    int disponibles = 0;
    int posicion;
    int i;

    if (ent == NULL) {
        return -1;
    }
    for (ej = ent->equipo; ej != NULL; ej = ej->siguiente) {
        if (ej->hp_actual > 0) {
            disponibles++;
        }
    }
    if (disponibles == 0) {
        return -1;
    }

    printf("--- Selección de Pokémon activo (%s: %s) ---\n", rol,
           ent->nombre);
    i = 0;
    for (ej = ent->equipo; ej != NULL; ej = ej->siguiente) {
        if (ej->hp_actual > 0) {
            i++;
            printf("%d. %s (nivel %d, HP %d/%d)\n", i, ej->nombre,
                   ej->nivel, ej->hp_actual, ej->hp_max);
        }
    }

    {
        char prompt[64];
        char msg_pos[64];
        snprintf(prompt, sizeof(prompt), "Seleccione posición (1-%d): ",
                 disponibles);
        snprintf(msg_pos, sizeof(msg_pos), "Posición inválida (1-%d).",
                 disponibles);
        posicion = validar_leer_entero_msg(prompt, 1, disponibles, msg_pos);
        if (posicion == 0) {
            return -1;   /* EOF: se cancela el combate */
        }
    }
    return posicion;
}

/**
 * @brief Opcion 8: combate amistoso entre dos entrenadores existentes.
 *
 * Submenu que elige la fase (grupos con empate D4 o eliminatoria con
 * anti-empate D5) y los ids de ambos entrenadores; valida que existan,
 * que sean distintos y que tengan equipo valido, y delega la ejecucion en
 * combate_ejecutar (RF-CMB-01..05). Ninguna entrada invalida termina el
 * programa (RF-TEC-03).
 */
static void jugar_combate(const Pokedex *pd, RegistroEntrenadores *reg)
{
    int modo;
    int id_local;
    int id_visita;
    Entrenador *local;
    Entrenador *visita;
    ResultadoCombate res;
    bool exito;

    printf("\n--- Combate amistoso ---\n");
    printf("1. Fase de grupos (admite empate)\n");
    printf("2. Eliminatoria (desempate forzoso)\n");
    printf("0. Volver al menú principal\n");
    modo = validar_leer_entero_msg(
        "Seleccione una opción: ", 0, 2,
        "Opción inválida. Intente de nuevo.");
    if (modo == 0) {
        printf("\n");
        return;   /* "volver" o EOF: cierre ordenado */
    }

    id_local = leer_id_entrenador("Id del entrenador local: ");
    if (id_local == 0 && feof(stdin)) {
        return;   /* EOF: cierre ordenado */
    }
    id_visita = leer_id_entrenador("Id del entrenador visitante: ");
    if (id_visita == 0 && feof(stdin)) {
        return;   /* EOF: cierre ordenado */
    }
    local = entrenador_buscar(reg, id_local);
    if (local == NULL) {
        printf("No existe un entrenador con id %d.\n", id_local);
        return;
    }
    visita = entrenador_buscar(reg, id_visita);
    if (visita == NULL) {
        printf("No existe un entrenador con id %d.\n", id_visita);
        return;
    }
    if (local == visita) {
        printf("Deben ser dos entrenadores distintos.\n");
        return;
    }
    if (equipo_contar(local) < 1 || !equipo_validar(pd, local, MAX_EQUIPO) ||
        equipo_contar(visita) < 1 || !equipo_validar(pd, visita, MAX_EQUIPO)) {
        printf("Ambos entrenadores deben tener un equipo válido "
               "(1-%d ejemplares).\n", MAX_EQUIPO);
        return;
    }

    combate_ejecutar(local, visita, modo == 2,
                     seleccionar_pokemon_activo, &res, &exito);
    if (!exito) {
        printf("El combate no se pudo ejecutar.\n");
        return;
    }
    if (res.empate) {
        printf("Resultado: empate (1 punto para cada entrenador).\n");
    } else {
        Entrenador *ganador = entrenador_buscar(reg, res.id_ganador);
        printf("Resultado: gana %s (id %d).\n",
               ganador != NULL ? ganador->nombre : "?",
               res.id_ganador);
    }
}

/**
 * @brief Arma los grupos del torneo si aún no están armados.
 *
 * La distribución es determinista por orden de registro (diseño §6.1) y
 * exige exactamente 32 entrenadores (D10). Se invoca desde las consultas
 * de clasificación y enfrentamientos; si el conteo no es 32 informa el
 * rechazo sin terminar el programa (RF-TEC-03).
 *
 * @param t   Puntero al estado del torneo (no debe ser NULL).
 * @param reg Puntero al registro de entrenadores (no debe ser NULL).
 * @param exito true si el torneo quedó armado (o ya lo estaba); false si
 *              los parámetros son inválidos o el conteo no es 32.
 */
static void armar_torneo_si_falta(Torneo *t, const RegistroEntrenadores *reg,
                                  bool *exito)
{
    if (t->estado != TORNEO_SIN_INICIAR) {
        *exito = true;
        return;
    }
    torneo_armar_grupos(t, reg, exito);
    if (!*exito) {
        printf("El torneo requiere exactamente %d entrenadores para armar "
               "los grupos (actualmente %d).\n", MAX_ENTRENADORES,
               reg->cantidad);
        return;
    }
    printf("Torneo armado: 8 grupos de 4 entrenadores (combates 1-48).\n");
    *exito = true;
}

/**
 * @brief Opcion 7: consultar la clasificación de la fase de grupos.
 *
 * Arma los grupos si hace falta (32, D10), muestra la tabla (RF-CLS-01) y
 * guarda la clasificación en data/clasificacion.txt (formato §5.4).
 */
static void consultar_clasificacion(Torneo *t, const RegistroEntrenadores *reg)
{
    bool exito;

    printf("\n--- Consultar clasificación ---\n");
    armar_torneo_si_falta(t, reg, &exito);
    if (!exito) {
        return;
    }
    torneo_mostrar_clasificacion(t, reg);
    archivos_guardar_clasificacion(t, reg, RUTA_CLASIFICACION, &exito);
    if (exito) {
        printf("Clasificación guardada en %s.\n", RUTA_CLASIFICACION);
    } else {
        printf("Aviso: no se pudo guardar la clasificación en %s.\n",
               RUTA_CLASIFICACION);
    }
}

/**
 * @brief Opcion 8: consultar los enfrentamientos del torneo.
 *
 * Submenu que ofrece el combate amistoso (migrado de F3, antes opcion
 * directa) y el calendario de la fase de grupos (combates 1-48 con los
 * participantes resueltos por el sistema, RF-RES-03). Ninguna entrada
 * invalida termina el programa (RF-TEC-03).
 */
static void consultar_enfrentamientos(const Pokedex *pd,
                                      RegistroEntrenadores *reg,
                                      Torneo *t)
{
    int sub;
    bool exito;

    printf("\n--- Consultar enfrentamientos ---\n");
    printf("1. Combate amistoso\n");
    printf("2. Calendario del torneo (combates 1-48)\n");
    printf("0. Volver al menú principal\n");
    sub = validar_leer_entero_msg(
        "Seleccione una opción: ", 0, 2,
        "Opción inválida. Intente de nuevo.");
    if (sub == 0) {
        printf("\n");
        return;   /* "volver" o EOF: cierre ordenado */
    }
    if (sub == 1) {
        jugar_combate(pd, reg);
        return;
    }
    if (sub == 2) {
        armar_torneo_si_falta(t, reg, &exito);
        if (!exito) {
            return;
        }
        torneo_mostrar_enfrentamientos(t, reg);
        return;
    }
    printf("Opción inválida. Intente de nuevo.\n");
}

/**
 * @brief Opcion 6: cargar resultados del torneo (RF-RES-01/RF-RES-04).
 *
 * Submenu teclado o data/resultados.txt (admite parciales); arma el torneo
 * si hace falta (32, D10) y muestra los pendientes. Los participantes los
 * resuelve el sistema (RF-RES-03); cada registro pasa por
 * resultados_validar (RF-RES-02).
 */
static void cargar_resultados(Torneo *t, RegistroEntrenadores *reg)
{
    int sub;
    bool exito;

    printf("\n--- Cargar resultados ---\n");
    printf("1. Por teclado\n");
    printf("2. Desde archivo (%s)\n", RUTA_RESULTADOS);
    printf("0. Volver al menú principal\n");
    sub = validar_leer_entero_msg(
        "Seleccione una opción: ", 0, 2,
        "Opción inválida. Intente de nuevo.");
    if (sub == 0) {
        printf("\n");
        return;   /* "volver" o EOF: cierre ordenado */
    }
    armar_torneo_si_falta(t, reg, &exito);
    if (!exito) {
        return;
    }
    if (sub == 1) {
        resultados_cargar_teclado(t, reg, &exito);
    } else {
        resultados_cargar_archivo(t, reg, RUTA_RESULTADOS, &exito);
    }
    (void)exito;   /* el resultado de la carga se ignora (RF-RES-01) */
    resultados_mostrar_pendientes(t);
}

/**
 * @brief Persiste los entrenadores modificados en la sesión al salir.
 *
 * Solo escribe si el registro cambió (RF-ENT-03), para no reescribir datos
 * intactos. La clasificación se guarda en la opción 7 y los resultados se
 * persisten vía archivos_guardar_resultados (F7, no autoguardado).
 */
static void guardar_al_salir(const RegistroEntrenadores *reg)
{
    bool exito;

    if (entrenadores_sucios) {
        archivos_guardar_entrenadores(reg, RUTA_ENTRENADORES, &exito);
        if (exito) {
            printf("Entrenadores guardados en %s.\n", RUTA_ENTRENADORES);
        } else {
            printf("Aviso: no se pudieron guardar los entrenadores.\n");
        }
    }
}

/**
 * @brief Punto de entrada: carga los datos iniciales, muestra el menú y
 *        despacha las 12 opciones.
 *
 * F2: al inicio se cargan tipos, Pokédex y (si existe) el archivo de
 * entrenadores; las opciones 1..5 quedan cableadas y la opción 12 sale
 * liberando todos los equipos (sin fugas).
 *
 * F5: las opciones 7 (clasificación) y 8 (enfrentamientos) delegan en el
 * módulo torneo; el torneo se arma al primer uso con 32 entrenadores.
 *
 * F6: las opciones 10 (resultados del torneo: bracket 49-64) y 11 (campeón)
 * delegan en el módulo torneo.
 *
 * F7: la opción 6 (cargar resultados) delega en resultados (teclado o
 * archivo, con pendientes), la opción 7 guarda la clasificación en
 * data/clasificacion.txt y al salir se persisten los entrenadores
 * modificados en la sesión.
 *
 * @return 0 al salir de forma ordenada.
 */
int main(void)
{
    Pokedex pokedex;
    RegistroEntrenadores registro;
    Torneo torneo;
    int salir = 0;
    int i;
    bool exito;

    registro.cantidad = 0;
    torneo.estado = TORNEO_SIN_INICIAR;

    /* Carga inicial de datos: tipos, Pokédex y entrenadores (RF-PDX-06). */
    tipos_inicializar();
    pokedex_cargar(&pokedex, RUTA_POKEDEX, &exito);
    if (!exito) {
        printf("Aviso: la Pokédex no está disponible; las consultas de "
               "especies quedarán deshabilitadas.\n");
    }

    /* Carga opcional de entrenadores: si el archivo no existe o queda
       vacío, el programa continúa y se registran por teclado (RF-ENT-03). */
    archivos_cargar_entrenadores(&registro, &pokedex, RUTA_ENTRENADORES,
                                 &exito);
    if (!exito) {
        printf("Aviso: no se cargaron entrenadores desde el archivo; use la "
               "opción 2 para registrarlos.\n");
    }

    while (!salir) {
        mostrar_menu();

        int opcion = leer_opcion();

        if (opcion == 0) {
            /* EOF: cierre ordenado (el guardado al salir corre al final) */
            printf("\n");
            salir = 1;
            continue;
        }
        if (opcion == 12) {
            printf("Saliendo del programa.\n");
            salir = 1;
            continue;
        }

        switch (opcion) {
        case 1:
            consultar_pokedex(&pokedex);
            break;
        case 2:
            registrar_entrenador(&registro);
            break;
        case 3:
            crear_equipo(&pokedex, &registro);
            break;
        case 4:
            entrenador_mostrar_todos(&registro);
            break;
        case 5:
            consultar_equipo(&pokedex, &registro);
            break;
        case 6:
            cargar_resultados(&torneo, &registro);
            break;
        case 7:
            consultar_clasificacion(&torneo, &registro);
            break;
        case 8:
            consultar_enfrentamientos(&pokedex, &registro, &torneo);
            break;
        case 10:
            torneo_mostrar_resultados(&torneo, &registro);
            break;
        case 11:
            torneo_mostrar_campeon(&torneo, &registro);
            break;
        default:
            printf("Opción %d en construcción\n", opcion);
            break;
        }
    }

    /* Persistencia de los entrenadores modificados en la sesión (RF-ENT-03)
       antes de liberar los equipos (sin fugas). */
    guardar_al_salir(&registro);
    for (i = 0; i < registro.cantidad; i++) {
        equipo_liberar(&registro.entrenadores[i]);
    }
    return 0;
}
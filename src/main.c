/**
 * @file main.c
 * @brief Bucle del menú principal y flujo general del torneo (RF-MEN-01).
 * @author <Nombre del estudiante>
 * @date 2026-09-01
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constantes.h"
#include "pokedex.h"
#include "tipos.h"
#include "entrenador.h"
#include "equipo.h"
#include "combate.h"
#include "archivos.h"

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
    printf("Seleccione una opción (1-12): ");
}

/**
 * Lee la opcion del menu con validacion de rango (RF-MEN-01, RF-TEC-03).
 * Devuelve 0 ante EOF para un cierre ordenado, -1 si la entrada no es un
 * entero valido y la opcion leida en caso contrario.
 */
static int leer_opcion(void)
{
    int opcion = 0;
    int leido = scanf("%d", &opcion);

    if (leido == EOF) {
        return 0;   /* cierre ordenado */
    }
    if (leido == 0) {
        /* entrada no numerica: descartar el resto de la linea */
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {
            /* descartar */
        }
        return -1;
    }
    return opcion;
}

/**
 * Lee una linea de texto desde stdin (sin el '\n' final). Devuelve false
 * ante EOF. Se usa para capturar el nombre de una especie a buscar.
 */
static bool leer_linea(char *buf, size_t n)
{
    if (fgets(buf, (int)n, stdin) == NULL) {
        return false;
    }
    size_t largo = strlen(buf);
    if (largo > 0 && buf[largo - 1] == '\n') {
        buf[largo - 1] = '\0';
    }
    return true;
}

/**
 * Descarta el resto de la linea actual de stdin (tras un scanf de entero).
 */
static void descartar_linea(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        /* descartar */
    }
}

/**
 * Lee un entero de stdin validando la conversión. Ante entrada no numérica
 * imprime aviso, descarta la línea y devuelve false; el llamador decide si
 * reintenta o abandona (RF-TEC-03: ninguna entrada termina el programa).
 */
static bool leer_entero(int *salida)
{
    if (scanf("%d", salida) != 1) {
        printf("Entrada inválida.\n");
        descartar_linea();
        return false;
    }
    descartar_linea();
    return true;
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
    int salir_submenu = 0;

    while (!salir_submenu) {
        printf("\n--- Consultar Pokédex ---\n");
        printf("1. Mostrar todas las especies\n");
        printf("2. Buscar por número\n");
        printf("3. Buscar por nombre\n");
        printf("4. Consultar ficha completa\n");
        printf("0. Volver al menú principal\n");
        printf("Seleccione una opción: ");

        int opcion = leer_opcion();

        if (opcion == 0) {
            printf("\n");
            return;   /* EOF: cierre ordenado */
        }

        switch (opcion) {
        case 0:
            salir_submenu = 1;
            break;
        case 1:
            if (pd->cantidad == 0) {
                printf("La Pokédex no está cargada.\n");
            } else {
                pokedex_mostrar_todas(pd);
            }
            break;
        case 2:
        case 4: {
            int numero;
            printf("Ingrese el número de especie: ");
            if (!leer_entero(&numero)) {
                break;
            }
            mostrar_resultado_busqueda(pokedex_buscar_numero(pd, numero));
            break;
        }
        case 3: {
            char nombre[TAM_MAX_APODO * 2];
            descartar_linea();
            printf("Ingrese el nombre de la especie: ");
            if (leer_linea(nombre, sizeof(nombre))) {
                mostrar_resultado_busqueda(
                    pokedex_buscar_nombre(pd, nombre));
            }
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

    printf("Ingrese el id del entrenador: ");
    if (!leer_entero(&id)) {
        return;
    }
    if (id <= 0) {
        printf("Id inválido: debe ser un entero positivo.\n");
        return;
    }
    printf("Ingrese el nombre del entrenador: ");
    if (!leer_linea(nombre, sizeof(nombre))) {
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
    if (!entrenador_registrar(reg, id, nombre)) {
        printf("No se pudo registrar: registro lleno (máximo %d).\n",
               MAX_ENTRENADORES);
        return;
    }
    printf("Entrenador registrado: id %d, %s.\n", id, nombre);
}

/**
 * @brief Opcion 3: crea el equipo de un entrenador seleccionando especies.
 *
 * Libera el equipo anterior si existe y agrega ejemplares por número de
 * especie y nivel (1..100); el id del ejemplar lo asigna el módulo de
 * equipos (único global). Termina con especie 0 o al llegar a MAX_EQUIPO.
 */
static void crear_equipo(const Pokedex *pd, RegistroEntrenadores *reg)
{
    int id_ent;
    Entrenador *ent;

    printf("Ingrese el id del entrenador: ");
    if (!leer_entero(&id_ent)) {
        return;
    }
    ent = entrenador_buscar(reg, id_ent);
    if (ent == NULL) {
        printf("No existe un entrenador con id %d.\n", id_ent);
        return;
    }
    if (ent->equipo != NULL) {
        equipo_liberar(ent);
        printf("Equipo anterior liberado.\n");
    }

    for (;;) {
        int numero;
        int nivel;
        int id_ejemplar;
        char apodo[TAM_MAX_APODO];
        const Especie *esp;
        Ejemplar *ej;

        printf("Ingrese el número de especie (0 para terminar): ");
        if (!leer_entero(&numero)) {
            continue;
        }
        if (numero == 0) {
            break;
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

        for (;;) {
            printf("Ingrese el nivel del ejemplar (%d-%d): ",
                   NIVEL_MIN, NIVEL_MAX);
            if (!leer_entero(&nivel)) {
                continue;
            }
            if (nivel < NIVEL_MIN || nivel > NIVEL_MAX) {
                printf("Nivel inválido (rango %d-%d).\n", NIVEL_MIN, NIVEL_MAX);
                continue;
            }
            break;
        }

        printf("Nombre/apodo (Enter = nombre de la especie): ");
        if (!leer_linea(apodo, sizeof(apodo))) {
            return;   /* EOF: cierre ordenado */
        }
        if (apodo[0] == '\0') {
            snprintf(apodo, sizeof(apodo), "%s", esp->nombre);
        }

        id_ejemplar = equipo_siguiente_id();
        ej = equipo_crear_ejemplar(pd, numero, apodo, nivel, id_ejemplar);
        if (ej == NULL) {
            printf("No se pudo crear el ejemplar (especie, nivel o memoria).\n");
            continue;
        }
        if (!equipo_agregar_ejemplar(ent, ej)) {
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

    printf("Ingrese el id del entrenador: ");
    if (!leer_entero(&id_ent)) {
        return;
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

    for (;;) {
        int leido = scanf("%d", &posicion);
        if (leido == EOF) {
            return -1;
        }
        if (leido != 1) {
            printf("Entrada inválida.\n");
            descartar_linea();
            continue;
        }
        if (posicion < 1 || posicion > disponibles) {
            printf("Posición inválida (1-%d).\n", disponibles);
            descartar_linea();
            continue;
        }
        descartar_linea();
        return posicion;
    }
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

    printf("\n--- Combate amistoso ---\n");
    printf("1. Fase de grupos (admite empate)\n");
    printf("2. Eliminatoria (desempate forzoso)\n");
    printf("0. Volver al menú principal\n");
    printf("Seleccione una opción: ");
    modo = leer_opcion();
    if (modo == 0) {
        printf("\n");
        return;   /* EOF o "volver": cierre ordenado */
    }
    if (modo != 1 && modo != 2) {
        printf("Opción inválida. Intente de nuevo.\n");
        return;
    }

    printf("Id del entrenador local: ");
    if (!leer_entero(&id_local)) {
        return;
    }
    printf("Id del entrenador visitante: ");
    if (!leer_entero(&id_visita)) {
        return;
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

    if (!combate_ejecutar(local, visita, modo == 2,
                          seleccionar_pokemon_activo, &res)) {
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
 * @brief Punto de entrada: carga los datos iniciales, muestra el menú y
 *        despacha las 12 opciones.
 *
 * F2: al inicio se cargan tipos, Pokédex y (si existe) el archivo de
 * entrenadores; las opciones 1..5 quedan cableadas y la opción 12 sale
 * liberando todos los equipos (sin fugas).
 *
 * @return 0 al salir de forma ordenada.
 */
int main(void)
{
    Pokedex pokedex;
    RegistroEntrenadores registro;
    int salir = 0;
    int i;

    registro.cantidad = 0;

    /* Carga inicial de datos: tipos, Pokédex y entrenadores (RF-PDX-06). */
    tipos_inicializar();
    if (!pokedex_cargar(&pokedex, RUTA_POKEDEX)) {
        printf("Aviso: la Pokédex no está disponible; las consultas de "
               "especies quedarán deshabilitadas.\n");
    }

    /* Carga opcional de entrenadores: si el archivo no existe o queda
       vacío, el programa continúa y se registran por teclado (RF-ENT-03). */
    if (!archivos_cargar_entrenadores(&registro, &pokedex, RUTA_ENTRENADORES)) {
        printf("Aviso: no se cargaron entrenadores desde el archivo; use la "
               "opción 2 para registrarlos.\n");
    }

    while (!salir) {
        mostrar_menu();

        int opcion = leer_opcion();

        if (opcion == 0) {
            /* EOF: cierre ordenado */
            printf("\n");
            break;
        }
        if (opcion < 1 || opcion > 12) {
            printf("Opción inválida. Intente de nuevo.\n");
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
        case 8:
            jugar_combate(&pokedex, &registro);
            break;
        default:
            printf("Opción %d en construcción\n", opcion);
            break;
        }
    }

    /* Liberación disciplinada de todos los equipos al salir (sin fugas). */
    for (i = 0; i < registro.cantidad; i++) {
        equipo_liberar(&registro.entrenadores[i]);
    }
    return 0;
}
/**
 * @file main.c
 * @brief Bucle del menú principal y flujo general del torneo (RF-MEN-01).
 * @author <Nombre del estudiante>
 * @date 2026-09-01
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "constantes.h"
#include "pokedex.h"
#include "tipos.h"

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
        case 2: {
            int numero;
            printf("Ingrese el número de especie: ");
            if (scanf("%d", &numero) != 1) {
                printf("Entrada inválida.\n");
                int c;
                while ((c = getchar()) != '\n' && c != EOF) { /* descartar */ }
                break;
            }
            mostrar_resultado_busqueda(pokedex_buscar_numero(pd, numero));
            break;
        }
        case 3: {
            char nombre[TAM_MAX_APODO * 2];
            int c;
            /* Descarta el '\n' dejado por scanf de la opcion. */
            while ((c = getchar()) != '\n' && c != EOF) { /* descartar */ }
            printf("Ingrese el nombre de la especie: ");
            if (leer_linea(nombre, sizeof(nombre))) {
                mostrar_resultado_busqueda(
                    pokedex_buscar_nombre(pd, nombre));
            }
            break;
        }
        case 4: {
            int numero;
            printf("Ingrese el número de especie: ");
            if (scanf("%d", &numero) != 1) {
                printf("Entrada inválida.\n");
                int c;
                while ((c = getchar()) != '\n' && c != EOF) { /* descartar */ }
                break;
            }
            mostrar_resultado_busqueda(pokedex_buscar_numero(pd, numero));
            break;
        }
        default:
            printf("Opción inválida. Intente de nuevo.\n");
            break;
        }
    }
}

/**
 * @brief Punto de entrada: carga los datos iniciales, muestra el menú y
 *        despacha las 12 opciones.
 *
 * F1: al inicio se cargan la tabla de tipos y la Pokédex; la opción 1
 * (consultar Pokédex) ya está cableada con un submenú. El resto de opciones
 * (2..11) informa "en construcción". La opción 12 sale del programa.
 *
 * @return 0 al salir de forma ordenada.
 */
int main(void)
{
    Pokedex pokedex;
    int salir = 0;

    /* Carga inicial de datos: la tabla de tipos y la Pokédex (RF-PDX-06). */
    tipos_inicializar();
    if (!pokedex_cargar(&pokedex, RUTA_POKEDEX)) {
        printf("Aviso: la Pokédex no está disponible; las consultas de "
               "especies quedarán deshabilitadas.\n");
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
        if (opcion == 1) {
            consultar_pokedex(&pokedex);
            continue;
        }
        printf("Opción %d en construcción\n", opcion);
    }

    return 0;
}
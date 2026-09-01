/**
 * @file main.c
 * @brief Bucle del menú principal y flujo general del torneo (RF-MEN-01).
 * @author <Nombre del estudiante>
 * @date 2026-09-01
 */

#include <stdio.h>

#include "constantes.h"

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
 * @brief Punto de entrada: muestra el menu y despacha las 12 opciones.
 *
 * F0: cada opcion (1..11) informa "en construccion"; la opcion 12 sale del
 * programa. La logica de negocio se cablea en las fases F1..F9.
 *
 * @return 0 al salir de forma ordenada.
 */
int main(void)
{
    int salir = 0;

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
        printf("Opción %d en construcción\n", opcion);
    }

    return 0;
}
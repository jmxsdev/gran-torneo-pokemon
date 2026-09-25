/**
 * @file validacion.c
 * @brief Implementación de la lectura segura de entradas (RF-TEC-03).
 *
 * F8: API común de lectura con reintentos y detección de EOF. Garantía
 * estructural del diseño §8.1: ninguna función de validación llama a exit;
 * todos los bucles reintentan hasta entrada válida o EOF (EOF ⇒ cierre
 * ordenado decidido por el llamador).
 * @author Nombre del estudiante
 * @date 2026-09-21
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "validacion.h"

/* Descarta el resto de la línea actual de stdin (tras un scanf de entero). */
static void descartar_linea(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        /* descartar */
    }
}

/**
 * Núcleo de lectura de enteros: prompt, conversión estricta, rango y EOF.
 * Devuelve el entero validado; 0 ante EOF. Nunca termina el programa.
 */
static int leer_entero_validado(const char *mensaje, int min, int max,
                                const char *msg_invalido)
{
    int valor;
    int leido;

    for (;;) {
        printf("%s", mensaje);
        leido = scanf("%d", &valor);
        if (leido == EOF) {
            return 0;   /* EOF: cierre ordenado decidido por el llamador */
        }
        if (leido != 1) {
            printf("Entrada inválida.\n");
            descartar_linea();
            continue;
        }
        descartar_linea();
        if (valor >= min && valor <= max) {
            return valor;
        }
        if (msg_invalido != NULL) {
            printf("%s\n", msg_invalido);
        } else {
            printf("Entrada inválida (debe estar entre %d y %d).\n",
                   min, max);
        }
    }
}

int validar_leer_entero(const char *mensaje, int min, int max)
{
    return leer_entero_validado(mensaje, min, max, NULL);
}

int validar_leer_entero_msg(const char *mensaje, int min, int max,
                            const char *msg_invalido)
{
    return leer_entero_validado(mensaje, min, max, msg_invalido);
}

void validar_leer_cadena(const char *mensaje, char *buf, size_t n,
                         bool *exito)
{
    size_t largo;

    if (buf == NULL || n == 0) {
        *exito = false;
        return;
    }
    printf("%s", mensaje);
    if (fgets(buf, (int)n, stdin) == NULL) {
        *exito = false;   /* EOF: cierre ordenado decidido por el llamador */
        return;
    }
    largo = strlen(buf);
    if (largo > 0 && buf[largo - 1] == '\n') {
        buf[largo - 1] = '\0';
        largo--;
    }
    if (largo > 0 && buf[largo - 1] == '\r') {
        buf[largo - 1] = '\0';
        largo--;
    }
    /* Si la línea excede el buffer, se descarta el resto para no
       desincronizar las lecturas siguientes (fgets dejó parte sin leer). */
    if (largo == (size_t)n - 1) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {
            /* descartar */
        }
    }
    *exito = true;
}

void validar_separar_campos(char *linea, char *campos[], int max_campos,
                            int *n)
{
    int n_campos = 0;
    char *p;

    if (linea == NULL || campos == NULL || max_campos <= 0) {
        *n = 0;
        return;
    }
    p = linea;
    for (;;) {
        char *inicio = p;
        while (*p != '\0' && *p != ';') {
            p++;
        }
        if (n_campos < max_campos) {
            campos[n_campos] = inicio;
        }
        n_campos++;
        if (*p == ';') {
            *p = '\0';
            p++;
            continue;
        }
        break;   /* fin de la línea */
    }
    *n = n_campos;
}
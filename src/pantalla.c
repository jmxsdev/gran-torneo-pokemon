/* Jesus Mora - CI 28480557 */
/**
 * @file pantalla.c
 * @brief Helpers de presentación por consola con padding por caracteres
 * visibles UTF-8 (guía de estilo de pantalla).
 * @author Jesus Mora
 * @date 2026-11-16
 */

#include <stdio.h>

#include "pantalla.h"

/* Implementación de pantalla_ancho: documentación canónica en pantalla.h. */
int pantalla_ancho(const char *texto)
{
    int columnas = 0;

    if (texto == NULL) {
        return 0;
    }
    while (*texto != '\0') {
        unsigned char c = (unsigned char)*texto;
        /* Los bytes de continuación UTF-8 (10xxxxxx) no abren columna. */
        if ((c & 0xC0) != 0x80) {
            columnas++;
        }
        texto++;
    }
    return columnas;
}

/* Implementación de pantalla_pad: documentación canónica en pantalla.h. */
void pantalla_pad(const char *texto, int ancho)
{
    int columnas = pantalla_ancho(texto);

    printf("%s", texto != NULL ? texto : "");
    while (columnas < ancho) {
        putchar(' ');
        columnas++;
    }
}

/* Implementación de pantalla_centro: documentación canónica en pantalla.h. */
void pantalla_centro(const char *texto, int ancho)
{
    int columnas = pantalla_ancho(texto);
    int izquierda = (ancho - columnas) / 2;
    int i;

    for (i = 0; i < izquierda; i++) {
        putchar(' ');
    }
    printf("%s\n", texto != NULL ? texto : "");
}

/* Implementación de pantalla_linea: documentación canónica en pantalla.h. */
void pantalla_linea(int ancho, const char *simbolo)
{
    int i;

    for (i = 0; i < ancho; i++) {
        printf("%s", simbolo != NULL ? simbolo : "-");
    }
    printf("\n");
}

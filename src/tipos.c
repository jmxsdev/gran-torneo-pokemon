/**
 * @file tipos.c
 * @brief Implementación del módulo de tipos: catálogo de 18 tipos y su
 *        matriz de efectividad (RF-CMB-05, RF-PDX-03).
 * @author Nombre del estudiante
 * @date 2026-09-01
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tipos.h"
#include "validacion.h"

/**
 * Normaliza una cadena UTF-8 a ASCII plegando acentos y minusculas, para
 * comparar sin distinguir mayusculas ni acentos (p. ej. "Eléctrico" y
 * "ELECTRICO" producen "electrico"). Se pleguan las vocales acentuadas y la
 * dieresis espanolas. Escribe en 'salida' (debe tener espacio suficiente).
 */
static void tipos_normalizar(const char *entrada, char *salida)
{
    int j = 0;
    while (*entrada != '\0') {
        unsigned char c = (unsigned char)*entrada;

        /* Vocales acentuadas en UTF-8 (2 bytes: 0xC3 0xA1..0xBC). */
        if (c == 0xC3 && entrada[1] != '\0') {
            unsigned char c2 = (unsigned char)entrada[1];
            char base = '\0';
            switch (c2) {
            case 0xA1: base = 'a'; break;   /* a */
            case 0xA9: base = 'e'; break;   /* e */
            case 0xAD: base = 'i'; break;   /* i */
            case 0xB3: base = 'o'; break;   /* o */
            case 0xBA: base = 'u'; break;   /* u */
            case 0xBC: base = 'u'; break;   /* u con dieresis */
            case 0x89: base = 'E'; break;   /* E */
            case 0x8D: base = 'I'; break;   /* I */
            case 0x93: base = 'O'; break;   /* O */
            case 0x9A: base = 'U'; break;   /* U */
            case 0x9C: base = 'U'; break;   /* U con dieresis */
            default: break;
            }
            if (base != '\0') {
                salida[j++] = (char)tolower((unsigned char)base);
                entrada += 2;
                continue;
            }
        }

        salida[j++] = (char)tolower(c);
        entrada++;
    }
    salida[j] = '\0';
}

/**
 * @brief Matriz de efectividad de 18x18 (MINOR-1).
 *
 * efectividad[atacante][defensor] es el multiplicador del tipo atacante
 * contra el tipo defensor. Valores posibles: 0.0f, 0.5f, 1.0f y 2.0f.
 * Vive únicamente en este archivo (no se declara en tipos.h) y se carga
 * desde data/efectividad.txt con tipos_inicializar. Si la carga falla se
 * usa una matriz por defecto embebida (ver tipos_matriz_por_defecto).
 *
 * Fuente de la tabla: catálogo de efectividad de tipos de la 1.ª generación
 * (juegos Rojo/Azul/Amarillo) para los 15 tipos nativos, completado con las
 * interacciones modernas de Siniestro, Acero y Hada introducidas en las
 * generaciones 2 y 6 (p. ej. Siniestro>Psíquico 2, Lucha>Hada 2,
 * Veneno>Hada 2, Acero>Hielo 2, Psíquico>Acero 0). Documentado en el
 * informe técnico (DOC-03) para la defensa oral.
 */
static float efectividad[CANT_TIPOS][CANT_TIPOS];

/**
 * @brief Matriz de respaldo embebida usada si no se puede leer el archivo.
 *
 * Orden de filas/columnas: TIPO_NORMAL, TIPO_FUEGO, TIPO_AGUA, TIPO_PLANTA,
 * TIPO_ELECTRICO, TIPO_HIELO, TIPO_LUCHA, TIPO_VENENO, TIPO_TIERRA,
 * TIPO_VOLADOR, TIPO_PSIQUICO, TIPO_BICHO, TIPO_ROCA, TIPO_FANTASMA,
 * TIPO_DRAGON, TIPO_SINIESTRO, TIPO_ACERO, TIPO_HADA.
 */
static const float matriz_por_defecto[CANT_TIPOS][CANT_TIPOS] = {
    /* NORMAL */ {1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,
                  1.0f,1.0f,1.0f,0.5f,0.0f,1.0f,1.0f,0.5f,1.0f},
    /* FUEGO  */ {1.0f,0.5f,0.5f,2.0f,1.0f,2.0f,1.0f,1.0f,1.0f,
                  1.0f,1.0f,2.0f,0.5f,1.0f,0.5f,1.0f,2.0f,1.0f},
    /* AGUA   */ {1.0f,2.0f,0.5f,0.5f,1.0f,1.0f,1.0f,1.0f,2.0f,
                  1.0f,1.0f,1.0f,2.0f,1.0f,0.5f,1.0f,1.0f,1.0f},
    /* PLANTA */ {1.0f,0.5f,2.0f,0.5f,1.0f,1.0f,1.0f,0.5f,2.0f,
                  0.5f,1.0f,0.5f,2.0f,1.0f,0.5f,1.0f,0.5f,1.0f},
    /* ELECTR */ {1.0f,1.0f,2.0f,0.5f,0.5f,1.0f,1.0f,1.0f,0.0f,
                  2.0f,1.0f,1.0f,1.0f,1.0f,0.5f,1.0f,1.0f,1.0f},
    /* HIELO  */ {1.0f,0.5f,0.5f,2.0f,1.0f,0.5f,1.0f,1.0f,2.0f,
                  2.0f,1.0f,1.0f,1.0f,1.0f,2.0f,1.0f,0.5f,1.0f},
    /* LUCHA  */ {2.0f,1.0f,1.0f,1.0f,1.0f,2.0f,1.0f,0.5f,1.0f,
                  0.5f,0.5f,0.5f,2.0f,0.0f,1.0f,2.0f,2.0f,0.5f},
    /* VENENO */ {1.0f,1.0f,1.0f,2.0f,1.0f,1.0f,1.0f,0.5f,0.5f,
                  1.0f,1.0f,1.0f,0.5f,0.5f,1.0f,1.0f,0.0f,2.0f},
    /* TIERRA */ {1.0f,2.0f,1.0f,0.5f,2.0f,1.0f,1.0f,2.0f,1.0f,
                  0.0f,1.0f,0.5f,2.0f,1.0f,1.0f,1.0f,2.0f,1.0f},
    /* VOLADOR*/ {1.0f,1.0f,1.0f,2.0f,0.5f,1.0f,2.0f,1.0f,1.0f,
                  1.0f,1.0f,2.0f,0.5f,1.0f,1.0f,1.0f,0.5f,1.0f},
    /* PSIQUIC*/ {1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,2.0f,2.0f,1.0f,
                  1.0f,0.5f,1.0f,1.0f,1.0f,1.0f,0.0f,0.5f,1.0f},
    /* BICHO  */ {1.0f,0.5f,1.0f,2.0f,1.0f,1.0f,0.5f,0.5f,1.0f,
                  0.5f,2.0f,1.0f,1.0f,0.5f,1.0f,2.0f,0.5f,0.5f},
    /* ROCA   */ {1.0f,2.0f,1.0f,1.0f,1.0f,2.0f,0.5f,1.0f,0.5f,
                  2.0f,1.0f,2.0f,1.0f,1.0f,1.0f,1.0f,0.5f,1.0f},
    /* FANTASMA*/{0.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,0.5f,1.0f,
                  1.0f,2.0f,1.0f,1.0f,2.0f,1.0f,0.5f,1.0f,1.0f},
    /* DRAGON */ {1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,
                  1.0f,1.0f,1.0f,1.0f,1.0f,2.0f,1.0f,0.5f,0.0f},
    /* SINIESTRO*/{1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,0.5f,1.0f,1.0f,
                  1.0f,2.0f,1.0f,1.0f,2.0f,1.0f,0.5f,1.0f,0.5f},
    /* ACERO  */ {1.0f,0.5f,0.5f,1.0f,0.5f,2.0f,1.0f,1.0f,1.0f,
                  1.0f,1.0f,1.0f,2.0f,1.0f,1.0f,1.0f,0.5f,2.0f},
    /* HADA   */ {1.0f,0.5f,1.0f,1.0f,1.0f,1.0f,2.0f,0.5f,1.0f,
                  1.0f,1.0f,1.0f,1.0f,1.0f,2.0f,2.0f,0.5f,1.0f}
};

/**
 * Copia la matriz por defecto embebida a la matriz activa del módulo.
 * Solo se usa como respaldo cuando falla la carga del archivo.
 */
static void tipos_usar_matriz_por_defecto(void)
{
    int i;
    for (i = 0; i < CANT_TIPOS; ++i) {
        memcpy(efectividad[i], matriz_por_defecto[i],
               CANT_TIPOS * sizeof(float));
    }
}

/**
 * @brief Carga la matriz de efectividad 18x18 desde data/efectividad.txt.
 *
 * Cada una de las 18 lineas debe contener exactamente 18 valores
 * numericos separados por ';', en el orden de TIPO_NORMAL..TIPO_HADA.
 * Si el archivo no se puede abrir o tiene un formato invalido, imprime un
 * mensaje claro y usa una matriz por defecto embebida (el programa continua).
 * El archivo no se modifica nunca (solo lectura).
 */
void tipos_inicializar(void)
{
    FILE *archivo = fopen(RUTA_EFECTIVIDAD, "r");

    if (archivo == NULL) {
        printf("Aviso: no se pudo abrir %s; se usara la tabla de "
               "efectividad por defecto.\n", RUTA_EFECTIVIDAD);
        tipos_usar_matriz_por_defecto();
        return;
    }

    int fila = 0;
    int cargadas = 0;
    char linea[TAM_MAX_LINEA];
    int errores = 0;

    while (fgets(linea, (int)sizeof(linea), archivo) != NULL) {
        /* Se aceptan solo lineas no vacias; se ignora el '\n' final. */
        size_t largo = strlen(linea);
        while (largo > 0 && (linea[largo - 1] == '\n' ||
                             linea[largo - 1] == '\r')) {
            linea[--largo] = '\0';
        }
        if (largo == 0) {
            continue;
        }

        int columna = 0;
        char *campos[CANT_TIPOS];
        int ncampos;
        validar_separar_campos(linea, campos, CANT_TIPOS, &ncampos);

        /* F8: se separa sin omitir campos vacios (';;'); un conteo distinto
           de CANT_TIPOS o un campo vacio invalida la matriz y se usa la
           tabla por defecto (cierre del hallazgo SUGGESTION F2). */
        if (ncampos != CANT_TIPOS) {
            printf("Error: linea %d de %s tiene %d valores (se esperaban "
                   "%d); se usara la tabla por defecto.\n",
                   fila + 1, RUTA_EFECTIVIDAD, ncampos, CANT_TIPOS);
            errores++;
            fclose(archivo);
            tipos_usar_matriz_por_defecto();
            return;
        }
        for (columna = 0; columna < CANT_TIPOS; columna++) {
            if (campos[columna][0] == '\0') {
                printf("Error: linea %d de %s tiene un campo vacio (';;'); "
                       "se usara la tabla por defecto.\n",
                       fila + 1, RUTA_EFECTIVIDAD);
                errores++;
                fclose(archivo);
                tipos_usar_matriz_por_defecto();
                return;
            }
            efectividad[fila][columna] = (float)atof(campos[columna]);
        }

        fila++;
        cargadas++;
    }

    fclose(archivo);

    if (errores > 0 || cargadas != CANT_TIPOS) {
        printf("Error: %s debe contener %d filas de %d valores; se "
               "usara la tabla por defecto.\n",
               RUTA_EFECTIVIDAD, CANT_TIPOS, CANT_TIPOS);
        tipos_usar_matriz_por_defecto();
    }
}

/* Implementación de tipos_es_valido: documentación canónica en tipos.h. */
void tipos_es_valido(const char *nombre, Tipo *salida, bool *es_valido)
{
    static const char *const nombres[TIPO_NINGUNO] = {
        "Normal", "Fuego", "Agua", "Planta", "Electrico", "Hielo",
        "Lucha", "Veneno", "Tierra", "Volador", "Psiquico", "Bicho",
        "Roca", "Fantasma", "Dragon", "Siniestro", "Acero", "Hada"
    };
    int i;
    char norm_nombre[64];
    char norm_catalogo[64];

    if (nombre == NULL || salida == NULL) {
        *es_valido = false;
        return;
    }

    if (strcmp(nombre, "-") == 0) {
        *salida = TIPO_NINGUNO;
        *es_valido = true;
        return;
    }

    tipos_normalizar(nombre, norm_nombre);
    for (i = 0; i < TIPO_NINGUNO; ++i) {
        tipos_normalizar(nombres[i], norm_catalogo);
        if (strcmp(norm_nombre, norm_catalogo) == 0) {
            *salida = (Tipo)i;
            *es_valido = true;
            return;
        }
    }

    *es_valido = false;
}

/* Implementación de tipos_a_texto: documentación canónica en tipos.h. */
const char *tipos_a_texto(Tipo t)
{
    static const char *const nombres[TIPO_NINGUNO] = {
        "Normal", "Fuego", "Agua", "Planta", "Electrico", "Hielo",
        "Lucha", "Veneno", "Tierra", "Volador", "Psiquico", "Bicho",
        "Roca", "Fantasma", "Dragon", "Siniestro", "Acero", "Hada"
    };

    if (t == TIPO_NINGUNO) {
        return "-";
    }
    if (t < 0 || t >= TIPO_NINGUNO) {
        return "?";
    }
    return nombres[t];
}

/* Implementación de tipos_multiplicador: documentación canónica en tipos.h. */
float tipos_multiplicador(Tipo ataque, Tipo def1, Tipo def2)
{
    float mult1, mult2;

    if (ataque < 0 || ataque >= CANT_TIPOS || ataque == TIPO_NINGUNO) {
        return 0.0f;
    }

    mult1 = (def1 >= 0 && def1 < CANT_TIPOS) ? efectividad[ataque][def1] : 1.0f;
    mult2 = (def2 >= 0 && def2 < CANT_TIPOS) ? efectividad[ataque][def2] : 1.0f;

    return mult1 * mult2;
}

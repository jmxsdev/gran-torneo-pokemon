/**
 * @file pokedex.c
 * @brief Implementación del módulo Pokédex: carga y consulta de las 150
 *        especies de la 1.ª generación (RF-PDX-01..06).
 * @author <Nombre del estudiante>
 * @date 2026-09-01
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pokedex.h"
#include "validacion.h"

/**
 * Normaliza una cadena UTF-8 a ASCII plegando acentos y minusculas, para
 * comparar nombres sin distinguir mayusculas ni acentos (RF-PDX-05).
 * Escribe en 'salida' (debe tener espacio suficiente, p. ej. TAM_MAX_APODO*2).
 */
static void pokedex_normalizar(const char *entrada, char *salida)
{
    int j = 0;
    while (*entrada != '\0') {
        unsigned char c = (unsigned char)*entrada;

        /* Vocales acentuadas en UTF-8 (2 bytes: 0xC3 0xA1..0xBC). */
        if (c == 0xC3 && entrada[1] != '\0') {
            unsigned char c2 = (unsigned char)entrada[1];
            char base = '\0';
            switch (c2) {
            case 0x81: base = 'A'; break;   /* A */
            case 0x89: base = 'E'; break;   /* E */
            case 0x8D: base = 'I'; break;   /* I */
            case 0x93: base = 'O'; break;   /* O */
            case 0x9A: base = 'U'; break;   /* U */
            case 0x9C: base = 'U'; break;   /* U dieresis */
            case 0xA1: base = 'a'; break;   /* a */
            case 0xA9: base = 'e'; break;   /* e */
            case 0xAD: base = 'i'; break;   /* i */
            case 0xB3: base = 'o'; break;   /* o */
            case 0xBA: base = 'u'; break;   /* u */
            case 0xBC: base = 'u'; break;   /* u dieresis */
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
 * @brief Carga las 150 especies desde un archivo de texto.
 *
 * Parseo estricto (formato D8, separador ';'): exactamente 8 campos por
 * linea y 150 lineas no vacias. Cada linea malformada se reporta con su
 * numero de linea; ante cualquier desvio la Pokédex queda con cantidad = 0
 * y el programa continua (RF-PDX-06). El archivo no se modifica (RF-PDX-04).
 *
 * @param pd   Puntero a la Pokédex que se completa (no debe ser NULL).
 * @param ruta Ruta del archivo de datos (p. ej. RUTA_POKEDEX).
 * @return true si se cargaron las 150 especies; false si el archivo no se
 *         pudo abrir, tiene formato invalido o no contiene 150 lineas.
 */
bool pokedex_cargar(Pokedex *pd, const char *ruta)
{
    FILE *archivo;
    char linea[TAM_MAX_LINEA];
    int numero_linea = 0;
    int cargadas = 0;
    int errores = 0;
    int vistos[POKEDEX_MAX + 1];

    if (pd == NULL || ruta == NULL) {
        return false;
    }

    pd->cantidad = 0;
    memset(vistos, 0, sizeof(vistos));

    archivo = fopen(ruta, "r");
    if (archivo == NULL) {
        printf("Error: no se pudo abrir el archivo de la Pokédex: %s\n", ruta);
        return false;
    }

    while (fgets(linea, (int)sizeof(linea), archivo) != NULL) {
        char *campos[8];
        int ncampos;
        int i;
        int numero;
        int hp, ataque, defensa, velocidad;
        Especie *esp;
        size_t largo;

        numero_linea++;

        /* Recorte del salto de linea final. */
        largo = strlen(linea);
        while (largo > 0 && (linea[largo - 1] == '\n' ||
                             linea[largo - 1] == '\r')) {
            linea[--largo] = '\0';
        }
        if (largo == 0) {
            continue;   /* linea vacia: no cuenta como especie */
        }

        /* Division estricta por ';' (formato D8). F8: se separa sin omitir
           campos vacios (';;') para no desplazar los campos siguientes y se
           rechaza cualquier exceso de campos (cierre del hallazgo
           SUGGESTION F2). */
        ncampos = validar_separar_campos(linea, campos, 8);
        if (ncampos != 8) {
            printf("Error en %s linea %d: se esperaban 8 campos y se "
                   "encontraron %d.\n", ruta, numero_linea, ncampos);
            errores++;
            continue;
        }
        for (i = 0; i < ncampos; i++) {
            if (campos[i][0] == '\0') {
                printf("Error en %s linea %d: campo vacio (';;').\n",
                       ruta, numero_linea);
                errores++;
                continue;
            }
        }

        numero = atoi(campos[0]);
        hp = atoi(campos[4]);
        ataque = atoi(campos[5]);
        defensa = atoi(campos[6]);
        velocidad = atoi(campos[7]);

        /* Validacion de rango de numero (RF-PDX-01). */
        if (numero < 1 || numero > POKEDEX_MAX) {
            printf("Error en %s linea %d: numero %d fuera de rango 1..%d.\n",
                   ruta, numero_linea, numero, POKEDEX_MAX);
            errores++;
            continue;
        }
        if (vistos[numero]) {
            printf("Error en %s linea %d: numero %d duplicado.\n",
                   ruta, numero_linea, numero);
            errores++;
            continue;
        }
        vistos[numero] = 1;

        /* Validacion de rangos numericos de stats (RF-PDX-02). */
        if (hp <= 0 || ataque <= 0 || defensa <= 0 || velocidad <= 0) {
            printf("Error en %s linea %d: las stats base deben ser > 0.\n",
                   ruta, numero_linea);
            errores++;
            continue;
        }

        /* Validacion de tipos (RF-PDX-03). */
        esp = &pd->especies[cargadas];
        if (!tipos_es_valido(campos[2], &esp->tipo_primario) ||
            !tipos_es_valido(campos[3], &esp->tipo_secundario)) {
            printf("Error en %s linea %d: tipo invalido.\n", ruta,
                   numero_linea);
            errores++;
            continue;
        }
        if (esp->tipo_primario == TIPO_NINGUNO) {
            printf("Error en %s linea %d: el tipo primario no puede ser "
                   "'-'.\n", ruta, numero_linea);
            errores++;
            continue;
        }

        esp->numero = numero;
        strncpy(esp->nombre, campos[1], TAM_MAX_APODO - 1);
        esp->nombre[TAM_MAX_APODO - 1] = '\0';
        esp->hp_base = hp;
        esp->ataque_base = ataque;
        esp->defensa_base = defensa;
        esp->velocidad_base = velocidad;

        cargadas++;
        if (cargadas >= POKEDEX_MAX) {
            break;   /* ya se lleno el arreglo */
        }
    }

    fclose(archivo);

    if (errores > 0) {
        printf("Error: la Pokédex no se cargó (se descartaron %d línea(s) "
               "con errores); quedó vacía.\n", errores);
        pd->cantidad = 0;
        return false;
    }

    if (cargadas != POKEDEX_MAX) {
        printf("Error: %s debe contener exactamente %d especies y tiene "
               "%d; la Pokédex quedó vacía.\n", ruta, POKEDEX_MAX, cargadas);
        pd->cantidad = 0;
        return false;
    }

    pd->cantidad = cargadas;
    return true;
}

/**
 * @brief Busca una especie por su número en la Pokédex.
 *
 * No modifica ningún dato de la Pokédex (RF-PDX-05).
 *
 * @param pd     Puntero a la Pokédex cargada (no debe ser NULL).
 * @param numero Número de especie a buscar, en el rango 1..150.
 * @return Puntero constante a la especie encontrada; NULL si no existe o
 *         los parámetros son inválidos.
 */
const Especie *pokedex_buscar_numero(const Pokedex *pd, int numero)
{
    int i;

    if (pd == NULL || numero < 1 || numero > POKEDEX_MAX) {
        return NULL;
    }

    for (i = 0; i < pd->cantidad; ++i) {
        if (pd->especies[i].numero == numero) {
            return &pd->especies[i];
        }
    }
    return NULL;
}

/**
 * @brief Busca una especie por su nombre exacto en la Pokédex.
 *
 * Comparación de cadenas sin distinguir mayúsculas ni acentos (RF-PDX-05).
 *
 * @param pd     Puntero a la Pokédex cargada (no debe ser NULL).
 * @param nombre Nombre de la especie a buscar (no debe ser NULL).
 * @return Puntero constante a la especie encontrada; NULL si no existe o
 *         los parámetros son inválidos.
 */
const Especie *pokedex_buscar_nombre(const Pokedex *pd, const char *nombre)
{
    char norm_busqueda[TAM_MAX_APODO * 2];
    char norm_actual[TAM_MAX_APODO * 2];
    int i;

    if (pd == NULL || nombre == NULL) {
        return NULL;
    }

    pokedex_normalizar(nombre, norm_busqueda);
    for (i = 0; i < pd->cantidad; ++i) {
        pokedex_normalizar(pd->especies[i].nombre, norm_actual);
        if (strcmp(norm_busqueda, norm_actual) == 0) {
            return &pd->especies[i];
        }
    }
    return NULL;
}

/**
 * @brief Muestra por consola todas las especies de la Pokédex en orden.
 *
 * @param pd Puntero a la Pokédex cargada (no debe ser NULL).
 */
void pokedex_mostrar_todas(const Pokedex *pd)
{
    int i;

    if (pd == NULL) {
        return;
    }

    printf("\n--- Pokédex (%d especies) ---\n", pd->cantidad);
    for (i = 0; i < pd->cantidad; ++i) {
        const Especie *e = &pd->especies[i];
        pokedex_mostrar_especie(e);
    }
}

/**
 * @brief Muestra por consola una especie (número, nombre y tipos).
 *
 * @param esp Puntero a la especie a mostrar (no debe ser NULL).
 */
void pokedex_mostrar_especie(const Especie *esp)
{
    if (esp == NULL) {
        return;
    }

    printf("%03d %-10s %-10s %-10s HP=%d ATQ=%d DEF=%d VEL=%d\n",
           esp->numero,
           esp->nombre,
           tipos_a_texto(esp->tipo_primario),
           tipos_a_texto(esp->tipo_secundario),
           esp->hp_base,
           esp->ataque_base,
           esp->defensa_base,
           esp->velocidad_base);
}

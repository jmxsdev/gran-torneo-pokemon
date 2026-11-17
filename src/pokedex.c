/* Jesus Mora - CI 28480557 */
/**
 * @file pokedex.c
 * @brief Implementación del módulo Pokédex: carga y consulta de las 150
 * especies de la 1.ª generación.
 * @author Jesus Mora
 * @date 2026-09-01
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pokedex.h"
#include "pantalla.h"
#include "validacion.h"

/**
 * Normaliza una cadena UTF-8 a ASCII plegando acentos y minusculas, para
 * comparar nombres sin distinguir mayusculas ni acentos.
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

/* Implementación de pokedex_cargar: documentación canónica en pokedex.h. */
void pokedex_cargar(Pokedex *pd, const char *ruta, bool *exito)
{
    FILE *archivo;
    char linea[TAM_MAX_LINEA];
    int numero_linea = 0;
    int cargadas = 0;
    int errores = 0;
    int vistos[POKEDEX_MAX + 1];

    if (pd == NULL || ruta == NULL) {
        *exito = false;
        return;
    }

    pd->cantidad = 0;
    memset(vistos, 0, sizeof(vistos));

    archivo = fopen(ruta, "r");
    if (archivo == NULL) {
        printf("Error: no se pudo abrir el archivo de la Pokédex: %s\n", ruta);
        *exito = false;
        return;
    }

    while (fgets(linea, (int)sizeof(linea), archivo) != NULL) {
        char *campos[8];
        int ncampos;
        int i;
        int numero;
        int hp, ataque, defensa, velocidad;
        Especie *esp;
        size_t largo;
        bool ok1, ok2;

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

        /* Division estricta por ';' (formato). se separa sin omitir
           campos vacios (';;') para no desplazar los campos siguientes y se
           rechaza cualquier exceso de campos. */
        validar_separar_campos(linea, campos, 8, &ncampos);
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

        /* Validacion de rango de numero. */
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

        /* Validacion de rangos numericos de stats. */
        if (hp <= 0 || ataque <= 0 || defensa <= 0 || velocidad <= 0) {
            printf("Error en %s linea %d: las stats base deben ser > 0.\n",
                   ruta, numero_linea);
            errores++;
            continue;
        }

        /* Validacion de tipos. D-F: sin cortocircuito, se
           consultan ambos tipos y se decide con la conjunción de los
           resultados (el cuerpo de tipos_es_valido solo escribe *salida
           en éxito). */
        esp = &pd->especies[cargadas];
        tipos_es_valido(campos[2], &esp->tipo_primario, &ok1);
        tipos_es_valido(campos[3], &esp->tipo_secundario, &ok2);
        if (!ok1 || !ok2) {
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
        *exito = false;
        return;
    }

    if (cargadas != POKEDEX_MAX) {
        printf("Error: %s debe contener exactamente %d especies y tiene "
               "%d; la Pokédex quedó vacía.\n", ruta, POKEDEX_MAX, cargadas);
        pd->cantidad = 0;
        *exito = false;
        return;
    }

    pd->cantidad = cargadas;
    *exito = true;
}

/**
 * @brief Busca una especie por su número en la Pokédex.
 *
 * No modifica ningún dato de la Pokédex.
 *
 * @param pd Puntero a la Pokédex cargada (no debe ser NULL).
 * @param numero Número de especie a buscar, en el rango 1..150.
 * @return Puntero constante a la especie encontrada; NULL si no existe o
 * los parámetros son inválidos.
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
 * Comparación de cadenas sin distinguir mayúsculas ni acentos.
 *
 * @param pd Puntero a la Pokédex cargada (no debe ser NULL).
 * @param nombre Nombre de la especie a buscar (no debe ser NULL).
 * @return Puntero constante a la especie encontrada; NULL si no existe o
 * los parámetros son inválidos.
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

/* Anchos de la tabla de la Pokédex (columnas visibles UTF-8). */
#define PDX_ANCHO_NUM    3
#define PDX_ANCHO_NOMBRE 13
#define PDX_ANCHO_TIPO   10
#define PDX_ANCHO_TOTAL  (1 + PDX_ANCHO_NUM + 2 + PDX_ANCHO_NOMBRE + 2 + \
                          PDX_ANCHO_TIPO + 2 + PDX_ANCHO_TIPO + 2 + 5 + 2 + \
                          5 + 2 + 5 + 2 + 5)
/* Ancho de la ficha individual de una especie. */
#define PDX_ANCHO_FICHA  44

/**
 * @brief Muestra por consola todas las especies de la Pokédex en una tabla.
 *
 * @param pd Puntero a la Pokédex cargada (no debe ser NULL).
 */
void pokedex_mostrar_todas(const Pokedex *pd)
{
    int i;
    char titulo[64];

    if (pd == NULL) {
        return;
    }

    snprintf(titulo, sizeof(titulo), "POKÉDEX (%d ESPECIES)", pd->cantidad);
    pantalla_linea(PDX_ANCHO_TOTAL, "═");
    pantalla_centro(titulo, PDX_ANCHO_TOTAL);
    pantalla_linea(PDX_ANCHO_TOTAL, "═");
    printf("   #  ");
    pantalla_pad("Nombre", PDX_ANCHO_NOMBRE);
    printf("  ");
    pantalla_pad("Tipo 1", PDX_ANCHO_TIPO);
    printf("  ");
    pantalla_pad("Tipo 2", PDX_ANCHO_TIPO);
    printf("  %5s  %5s  %5s  %5s\n", "HP", "ATQ", "DEF", "VEL");
    pantalla_linea(PDX_ANCHO_TOTAL, "─");
    for (i = 0; i < pd->cantidad; ++i) {
        const Especie *e = &pd->especies[i];
        printf(" %03d  ", e->numero);
        pantalla_pad(e->nombre, PDX_ANCHO_NOMBRE);
        printf("  ");
        pantalla_pad(tipos_a_texto(e->tipo_primario), PDX_ANCHO_TIPO);
        printf("  ");
        pantalla_pad(tipos_a_texto(e->tipo_secundario), PDX_ANCHO_TIPO);
        printf("  %5d  %5d  %5d  %5d\n", e->hp_base, e->ataque_base,
               e->defensa_base, e->velocidad_base);
    }
    pantalla_linea(PDX_ANCHO_TOTAL, "─");
}

/**
 * @brief Muestra por consola la ficha de una especie.
 *
 * @param esp Puntero a la especie a mostrar (no debe ser NULL).
 */
void pokedex_mostrar_especie(const Especie *esp)
{
    char titulo[48];

    if (esp == NULL) {
        return;
    }

    snprintf(titulo, sizeof(titulo), "#%03d  %s", esp->numero, esp->nombre);
    pantalla_linea(PDX_ANCHO_FICHA, "═");
    pantalla_centro(titulo, PDX_ANCHO_FICHA);
    pantalla_linea(PDX_ANCHO_FICHA, "═");
    printf("  Tipo 1:          %s\n", tipos_a_texto(esp->tipo_primario));
    printf("  Tipo 2:          %s\n", tipos_a_texto(esp->tipo_secundario));
    printf("  HP base:         %d\n", esp->hp_base);
    printf("  Ataque base:     %d\n", esp->ataque_base);
    printf("  Defensa base:    %d\n", esp->defensa_base);
    printf("  Velocidad base:  %d\n", esp->velocidad_base);
    pantalla_linea(PDX_ANCHO_FICHA, "─");
}

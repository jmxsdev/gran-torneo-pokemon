/**
 * @file archivos.c
 * @brief Implementación de la E/S de los archivos de texto del torneo (D8).
 *
 * F2: solo la carga de entrenadores (RF-ENT-03). El guardado de
 * entrenadores y la E/S de resultados/clasificación se completan en F7.
 * @author <Nombre del estudiante>
 * @date 2026-09-19
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "archivos.h"

/**
 * Indica si un entero ya está presente en un arreglo de valores vistos
 * (búsqueda lineal; se usa para detectar ids duplicados dentro del archivo).
 */
static bool entero_en_arreglo(const int *arreglo, int n, int valor)
{
    int i;

    for (i = 0; i < n; i++) {
        if (arreglo[i] == valor) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Carga entrenadores desde un archivo (RF-ENT-03).
 *
 * Formato D8 (§5.2): ID;NOMBRE;CANT;(ID_EJEMPLAR;NUM_ESPECIE;APODO;NIVEL)xCANT.
 * Cada línea se valida completa antes de registrar: id de entrenador y de
 * ejemplar únicos, especie existente en la Pokédex (RF-EQP-01) y nivel
 * dentro de 1..100. Las líneas inválidas se rechazan con mensaje y la carga
 * continúa; al final se reportan aceptados/rechazados. Los stats de los
 * ejemplares se re-derivan con D2 (no se persisten) y el contador global de
 * ids se fija al máximo id del archivo (diseño §5.2).
 *
 * @param reg  Puntero al registro que se completa (no debe ser NULL).
 * @param pd   Puntero a la Pokédex cargada (no debe ser NULL).
 * @param ruta Ruta del archivo de entrada (p. ej. RUTA_ENTRENADORES).
 * @return true si se cargó al menos un entrenador; false si el archivo no
 *         se pudo abrir, no quedó ningún entrenador o los parámetros son
 *         inválidos.
 */
bool archivos_cargar_entrenadores(RegistroEntrenadores *reg, const Pokedex *pd,
                                  const char *ruta)
{
    FILE *archivo;
    char linea[TAM_MAX_LINEA];
    int numero_linea = 0;
    int aceptados = 0;
    int rechazadas = 0;
    int max_id_ejemplar = 0;
    int ids_ent_vistos[MAX_ENTRENADORES];
    int n_ids_ent = 0;
    int ids_ej_vistos[MAX_ENTRENADORES * MAX_EQUIPO];
    int n_ids_ej = 0;

    if (reg == NULL || pd == NULL || ruta == NULL) {
        return false;
    }

    archivo = fopen(ruta, "r");
    if (archivo == NULL) {
        printf("Error: no se pudo abrir el archivo de entrenadores: %s\n",
               ruta);
        return false;
    }

    while (fgets(linea, (int)sizeof(linea), archivo) != NULL) {
        char *tok;
        int id_ent;
        int cant;
        int i;
        size_t largo;
        char nombre_ent[TAM_MAX_NOMBRE];
        int ids_ej[MAX_EQUIPO];
        int nums_ej[MAX_EQUIPO];
        int niveles_ej[MAX_EQUIPO];
        char apodos_ej[MAX_EQUIPO][TAM_MAX_APODO];

        numero_linea++;

        /* Recorte del salto de linea final (D8: lineas terminadas en \n). */
        largo = strlen(linea);
        while (largo > 0 && (linea[largo - 1] == '\n' ||
                             linea[largo - 1] == '\r')) {
            linea[--largo] = '\0';
        }
        if (largo == 0) {
            continue;   /* linea vacia: no cuenta */
        }

        /* Cabecera: ID;NOMBRE;CANT */
        tok = strtok(linea, ";");
        if (tok == NULL) {
            goto linea_invalida;
        }
        id_ent = atoi(tok);
        tok = strtok(NULL, ";");
        if (tok == NULL) {
            goto linea_invalida;
        }
        snprintf(nombre_ent, sizeof(nombre_ent), "%s", tok);
        tok = strtok(NULL, ";");
        if (tok == NULL) {
            goto linea_invalida;
        }
        cant = atoi(tok);

        if (id_ent <= 0) {
            goto linea_invalida;
        }
        if (nombre_ent[0] == '\0') {
            goto linea_invalida;
        }
        if (cant < 0 || cant > MAX_EQUIPO) {
            goto linea_invalida;
        }
        if (entrenador_buscar(reg, id_ent) != NULL) {
            goto linea_invalida;   /* id ya registrado (RF-ENT-02) */
        }
        if (entero_en_arreglo(ids_ent_vistos, n_ids_ent, id_ent)) {
            goto linea_invalida;   /* id duplicado dentro del archivo */
        }

        /* Ejemplares: (ID_EJEMPLAR;NUM_ESPECIE;APODO;NIVEL) x CANT */
        for (i = 0; i < cant; i++) {
            tok = strtok(NULL, ";");
            if (tok == NULL) {
                goto linea_invalida;
            }
            ids_ej[i] = atoi(tok);
            tok = strtok(NULL, ";");
            if (tok == NULL) {
                goto linea_invalida;
            }
            nums_ej[i] = atoi(tok);
            tok = strtok(NULL, ";");
            if (tok == NULL) {
                goto linea_invalida;
            }
            snprintf(apodos_ej[i], sizeof(apodos_ej[i]), "%s", tok);
            tok = strtok(NULL, ";");
            if (tok == NULL) {
                goto linea_invalida;
            }
            niveles_ej[i] = atoi(tok);

            if (ids_ej[i] <= 0) {
                goto linea_invalida;
            }
            if (entero_en_arreglo(ids_ej_vistos, n_ids_ej, ids_ej[i])) {
                goto linea_invalida;   /* id de ejemplar repetido */
            }
            if (pokedex_buscar_numero(pd, nums_ej[i]) == NULL) {
                goto linea_invalida;   /* RF-EQP-01: especie inexistente */
            }
            if (apodos_ej[i][0] == '\0') {
                goto linea_invalida;
            }
            if (niveles_ej[i] < NIVEL_MIN || niveles_ej[i] > NIVEL_MAX) {
                goto linea_invalida;   /* RF-EQP-05: nivel fuera de rango */
            }
            if (ids_ej[i] > max_id_ejemplar) {
                max_id_ejemplar = ids_ej[i];
            }
        }

        /* Campos de mas: el formato exige exactamente 3 + 4*CANT. */
        tok = strtok(NULL, ";");
        if (tok != NULL) {
            goto linea_invalida;
        }

        /* Commit: solo si la linea completa es valida. */
        if (!entrenador_registrar(reg, id_ent, nombre_ent)) {
            goto linea_invalida;
        }
        ids_ent_vistos[n_ids_ent++] = id_ent;
        {
            Entrenador *ent = entrenador_buscar(reg, id_ent);

            for (i = 0; i < cant; i++) {
                Ejemplar *ej = equipo_crear_ejemplar(pd, nums_ej[i],
                                                     apodos_ej[i],
                                                     niveles_ej[i],
                                                     ids_ej[i]);
                if (ej == NULL) {
                    /* Rollback: sin equipo ni entrenador a medio cargar. */
                    equipo_liberar(ent);
                    reg->cantidad--;
                    n_ids_ent--;
                    goto linea_invalida;
                }
                if (!equipo_agregar_ejemplar(ent, ej)) {
                    free(ej);
                    equipo_liberar(ent);
                    reg->cantidad--;
                    n_ids_ent--;
                    goto linea_invalida;
                }
                ids_ej_vistos[n_ids_ej++] = ids_ej[i];
            }
        }
        aceptados++;
        continue;

linea_invalida:
        printf("Línea %d rechazada: campos inválidos (no se registra).\n",
               numero_linea);
        rechazadas++;
    }

    fclose(archivo);

    /* El contador global de ids queda en el máximo del archivo (o mayor). */
    equipo_fijar_contador_id(max_id_ejemplar);
    printf("Entrenadores cargados: %d, líneas rechazadas: %d.\n",
           aceptados, rechazadas);
    return aceptados > 0;
}
/**
 * @file archivos.c
 * @brief E/S de los archivos de texto del torneo (D8).
 *
 * F2: carga de entrenadores (RF-ENT-03). F7: guardado de entrenadores,
 * carga y guardado de resultados y guardado de la clasificación.
 * @author Nombre del estudiante
 * @date 2026-09-21
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "archivos.h"
#include "resultados.h"   /* archivos_cargar_resultados delega aquí */
#include "validacion.h"

/* Máximo de campos por línea del formato §5.2: 3 + 4*MAX_EQUIPO. */
#define CAMPOS_MAX_LINEA (3 + 4 * MAX_EQUIPO + 1)

/* Busca un entrenador por id sin mutar el registro (solo lectura). */
static const Entrenador *entrenador_por_id(const RegistroEntrenadores *reg,
                                           int id)
{
    int i;
    if (reg == NULL) {
        return NULL;
    }
    for (i = 0; i < reg->cantidad; i++) {
        if (reg->entrenadores[i].id == id) {
            return &reg->entrenadores[i];
        }
    }
    return NULL;
}

/**
 * Indica si un entero ya está en un arreglo de valores vistos (detección de
 * ids duplicados dentro del archivo).
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

/* Implementación de archivos_cargar_entrenadores: documentación canónica en archivos.h. */
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
        char *campos[CAMPOS_MAX_LINEA];
        int ncampos;
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
        largo = strlen(linea);
        while (largo > 0 && (linea[largo - 1] == '\n' ||
                             linea[largo - 1] == '\r')) {
            linea[--largo] = '\0';
        }
        if (largo == 0) {
            continue;   /* linea vacia: no cuenta */
        }

        /* F8: se separa sin omitir campos vacios (';;') para no desplazar
           los campos siguientes (cierre del hallazgo SUGGESTION F2). El
           formato §5.2 exige exactamente 3 + 4*CANT campos por linea. */
        ncampos = validar_separar_campos(linea, campos, CAMPOS_MAX_LINEA);
        if (ncampos < 3 || ncampos > CAMPOS_MAX_LINEA) {
            goto linea_invalida;
        }
        for (i = 0; i < ncampos; i++) {
            if (campos[i][0] == '\0') {
                goto linea_invalida;   /* campo vacío (';;') */
            }
        }
        id_ent = atoi(campos[0]);
        snprintf(nombre_ent, sizeof(nombre_ent), "%s", campos[1]);
        cant = atoi(campos[2]);

        if (id_ent <= 0 || cant < 0 || cant > MAX_EQUIPO) {
            goto linea_invalida;
        }
        if (ncampos != 3 + 4 * cant) {
            goto linea_invalida;   /* exactamente 3 + 4*CANT campos */
        }
        if (entrenador_buscar(reg, id_ent) != NULL ||
            entero_en_arreglo(ids_ent_vistos, n_ids_ent, id_ent)) {
            goto linea_invalida;   /* id ya registrado o repetido en archivo */
        }

        /* Ejemplares: (ID_EJEMPLAR;NUM_ESPECIE;APODO;NIVEL) x CANT */
        for (i = 0; i < cant; i++) {
            ids_ej[i] = atoi(campos[3 + 4 * i]);
            nums_ej[i] = atoi(campos[4 + 4 * i]);
            snprintf(apodos_ej[i], sizeof(apodos_ej[i]), "%s",
                     campos[5 + 4 * i]);
            niveles_ej[i] = atoi(campos[6 + 4 * i]);

            if (ids_ej[i] <= 0 ||
                entero_en_arreglo(ids_ej_vistos, n_ids_ej, ids_ej[i])) {
                goto linea_invalida;
            }
            if (apodos_ej[i][0] == '\0' ||
                niveles_ej[i] < NIVEL_MIN || niveles_ej[i] > NIVEL_MAX) {
                goto linea_invalida;
            }
            if (pokedex_buscar_numero(pd, nums_ej[i]) == NULL) {
                goto linea_invalida;   /* RF-EQP-01: especie inexistente */
            }
            if (ids_ej[i] > max_id_ejemplar) {
                max_id_ejemplar = ids_ej[i];
            }
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
                if (ej == NULL || !equipo_agregar_ejemplar(ent, ej)) {
                    /* Rollback: sin equipo ni entrenador a medio cargar.
                       free(NULL) es seguro en C. */
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

/* Implementación de archivos_guardar_entrenadores: documentación canónica en archivos.h. */
bool archivos_guardar_entrenadores(const RegistroEntrenadores *reg,
                                   const char *ruta)
{
    FILE *archivo;
    int i;

    if (reg == NULL || ruta == NULL) {
        return false;
    }
    archivo = fopen(ruta, "w");
    if (archivo == NULL) {
        printf("Error: no se pudo abrir el archivo de entrenadores: %s\n",
               ruta);
        return false;
    }
    for (i = 0; i < reg->cantidad; i++) {
        const Entrenador *ent = &reg->entrenadores[i];
        const Ejemplar *ej;
        fprintf(archivo, "%d;%s;%d", ent->id, ent->nombre,
                equipo_contar(ent));
        for (ej = ent->equipo; ej != NULL; ej = ej->siguiente) {
            fprintf(archivo, ";%d;%d;%s;%d", ej->id, ej->numero_especie,
                    ej->nombre, ej->nivel);
        }
        fprintf(archivo, "\n");
    }
    fclose(archivo);
    return true;
}

/* Implementación de archivos_cargar_resultados: documentación canónica en archivos.h. */
bool archivos_cargar_resultados(Torneo *t, RegistroEntrenadores *reg,
                                const char *ruta)
{
    return resultados_cargar_archivo(t, reg, ruta);
}

/**
 * @brief Guarda los resultados aplicados del torneo en un archivo (§5.3).
 *
 * Escribe una línea por combate con resultado definido (los pendientes se
 * omiten): NUM;ID1;ID2;RESULTADO;GANADOR;KOS1;KOS2, con "-" como ganador
 * de un empate (recargable, RF-RES-01/04).
 *
 * @param t    Puntero al estado del torneo (no debe ser NULL).
 * @param ruta Ruta del archivo de salida (p. ej. RUTA_RESULTADOS).
 * @return true si se escribió el archivo; false en caso contrario.
 */
bool archivos_guardar_resultados(const Torneo *t, const char *ruta)
{
    FILE *archivo;
    int k;

    if (t == NULL || ruta == NULL) {
        return false;
    }
    archivo = fopen(ruta, "w");
    if (archivo == NULL) {
        printf("Error: no se pudo abrir el archivo de resultados: %s\n",
               ruta);
        return false;
    }
    for (k = 1; k <= TOTAL_COMBATES; k++) {
        const Combate *c = &t->combates[k - 1];
        const char *res;
        char ganador[16];
        if (c->estado == RES_PENDIENTE) {
            continue;
        }
        res = (c->estado == RES_V1) ? "V1"
              : (c->estado == RES_V2) ? "V2" : "E";
        if (c->estado == RES_EMPATE) {
            snprintf(ganador, sizeof(ganador), "-");
        } else {
            snprintf(ganador, sizeof(ganador), "%d", c->id_ganador);
        }
        fprintf(archivo, "%d;%d;%d;%s;%s;%d;%d\n", c->numero,
                c->id_entrenador1, c->id_entrenador2, res, ganador,
                c->kos1, c->kos2);
    }
    fclose(archivo);
    return true;
}

/**
 * @brief Guarda la clasificación de los 8 grupos en un archivo (RF-CLS-01).
 *
 * Formato D8 §5.4 EXACTO: cabecera [GRUPO X] + 4 líneas
 * POSICION;ID;NOMBRE;VICTORIAS;EMPATES;DERROTAS;PUNTOS por grupo (la
 * columna de derrotados solo se muestra en pantalla, no se persiste);
 * orden de la cadena completa de RF-TRN-04.
 *
 * @param t    Puntero al estado del torneo (no debe ser NULL).
 * @param reg  Puntero al registro de entrenadores (no debe ser NULL).
 * @param ruta Ruta del archivo de salida (p. ej. RUTA_CLASIFICACION).
 * @return true si se escribió el archivo; false en caso contrario.
 */
bool archivos_guardar_clasificacion(const Torneo *t,
                                    const RegistroEntrenadores *reg,
                                    const char *ruta)
{
    FILE *archivo;
    int g;

    if (t == NULL || reg == NULL || ruta == NULL) {
        return false;
    }
    if (t->estado == TORNEO_SIN_INICIAR) {
        return false;   /* sin grupos no hay clasificación que guardar */
    }
    archivo = fopen(ruta, "w");
    if (archivo == NULL) {
        printf("Error: no se pudo abrir el archivo de clasificación: %s\n",
               ruta);
        return false;
    }
    for (g = 0; g < 8; g++) {
        int filas[4][7];
        int p;
        torneo_clasificacion_grupo(t, g, filas);
        fprintf(archivo, "[GRUPO %c]\n", (char)('A' + g));
        for (p = 0; p < 4; p++) {
            const Entrenador *e = entrenador_por_id(reg, filas[p][1]);
            fprintf(archivo, "%d;%d;%s;%d;%d;%d;%d\n",
                    filas[p][0], filas[p][1],
                    e != NULL ? e->nombre : "?",
                    filas[p][2], filas[p][3], filas[p][4], filas[p][5]);
        }
    }
    fclose(archivo);
    return true;
}
/**
 * @file archivos.h
 * @brief E/S de los archivos de texto del torneo (D8): entrenadores,
 *        resultados y clasificación.
 * @author Nombre del estudiante
 * @date 2026-09-01
 */

#ifndef ARCHIVOS_H
#define ARCHIVOS_H

#include <stdbool.h>

#include "entrenador.h"
#include "pokedex.h"
#include "torneo.h"

/**
 * @brief Guarda el registro de entrenadores en un archivo (RF-ENT-03).
 *
 * Formato D8: ID;NOMBRE;CANT;(ID_EJEMPLAR;NUM_ESPECIE;APODO;NIVEL)xCANT.
 * Los contadores de torneo no se persisten (se reconstruyen de resultados).
 *
 * @param reg  Puntero al registro (no debe ser NULL).
 * @param ruta Ruta del archivo de salida (p. ej. RUTA_ENTRENADORES).
 * @param exito true si se escribió el archivo; false si no se pudo abrir o
 *              los parámetros son inválidos.
 */
void archivos_guardar_entrenadores(const RegistroEntrenadores *reg,
                                   const char *ruta, bool *exito);

/**
 * @brief Carga entrenadores desde un archivo (RF-ENT-03).
 *
 * Las líneas inválidas se rechazan con mensaje y la carga continúa; al
 * terminar se reportan aceptados/rechazados. Los stats de los ejemplares se
 * re-derivan con D2 (no se persisten).
 *
 * @param reg  Puntero al registro que se completa (no debe ser NULL).
 * @param pd   Puntero a la Pokédex cargada (no debe ser NULL).
 * @param ruta Ruta del archivo de entrada (p. ej. RUTA_ENTRENADORES).
 * @param exito true si se cargó al menos un entrenador; false si el archivo
 *              no se pudo abrir o los parámetros son inválidos.
 */
void archivos_cargar_entrenadores(RegistroEntrenadores *reg, const Pokedex *pd,
                                  const char *ruta, bool *exito);

/**
 * @brief Carga resultados desde un archivo (RF-RES-01).
 *
 * Delega el parseo y la validación en resultados_cargar_archivo.
 *
 * @param t    Puntero al estado del torneo (no debe ser NULL).
 * @param reg  Puntero al registro de entrenadores (no debe ser NULL).
 * @param ruta Ruta del archivo de resultados (p. ej. RUTA_RESULTADOS).
 * @param exito true si se aplicó al menos un resultado; false en caso
 *              contrario.
 */
void archivos_cargar_resultados(Torneo *t, RegistroEntrenadores *reg,
                                const char *ruta, bool *exito);

/**
 * @brief Guarda los resultados aplicados del torneo en un archivo (§5.3).
 *
 * Escribe una línea por combate con resultado definido (los pendientes se
 * omiten): NUM;ID1;ID2;RESULTADO;GANADOR;KOS1;KOS2, con "-" como ganador
 * de un empate (recargable, RF-RES-01/04).
 *
 * @param t    Puntero al estado del torneo (no debe ser NULL).
 * @param ruta Ruta del archivo de salida (p. ej. RUTA_RESULTADOS).
 * @param exito true si se escribió el archivo; false en caso contrario.
 */
void archivos_guardar_resultados(const Torneo *t, const char *ruta,
                                 bool *exito);

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
 * @param exito true si se escribió el archivo; false en caso contrario.
 */
void archivos_guardar_clasificacion(const Torneo *t,
                                    const RegistroEntrenadores *reg,
                                    const char *ruta, bool *exito);

#endif /* ARCHIVOS_H */
/**
 * @file validacion.h
 * @brief Lectura segura de entradas con reintentos (RF-TEC-03).
 * @author Nombre del estudiante
 * @date 2026-09-01
 */

#ifndef VALIDACION_H
#define VALIDACION_H

#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Lee un entero del usuario y lo valida en un rango cerrado.
 *
 * Muestra el prompt y reintenta indefinidamente hasta recibir una entrada
 * numérica dentro de [min, max] o detectar EOF (cierre ordenado). Ninguna
 * entrada inválida termina el programa ni cuelga el bucle (RF-TEC-03);
 * esta función jamás llama a exit.
 *
 * @param mensaje Texto que se muestra como prompt (no debe ser NULL).
 * @param min     Límite inferior aceptado (inclusive).
 * @param max     Límite superior aceptado (inclusive).
 * @return El entero validado dentro del rango; 0 si se detecta EOF (el
 *         llamador decide el cierre ordenado). Si min <= 0, el llamador
 *         debe distinguir EOF de un 0 legítimo con feof(stdin).
 */
int validar_leer_entero(const char *mensaje, int min, int max);

/**
 * @brief Igual que validar_leer_entero, con mensaje propio de rango.
 *
 * Extensión de F8: algunos prompts tienen mensajes específicos de rechazo
 * (p. ej. "Cantidad inválida: debe estar entre 1 y 6."); msg_invalido se
 * imprime tal cual ante un valor fuera de [min, max] y se reintenta.
 *
 * @param mensaje      Texto que se muestra como prompt (no debe ser NULL).
 * @param min          Límite inferior aceptado (inclusive).
 * @param max          Límite superior aceptado (inclusive).
 * @param msg_invalido Mensaje específico de rechazo (puede ser NULL para
 *                     usar el mensaje genérico de rango).
 * @return El entero validado dentro del rango; 0 si se detecta EOF.
 */
int validar_leer_entero_msg(const char *mensaje, int min, int max,
                            const char *msg_invalido);

/**
 * @brief Lee una cadena del usuario con longitud acotada.
 *
 * Muestra el prompt y lee una línea sin el '\n' final; nunca desborda el
 * buffer (recorta a n-1 caracteres + NUL y descarta el resto de la línea
 * para no desincronizar las lecturas siguientes). No exige cadena no vacía:
 * el llamador decide la política (p. ej. apodo vacío = nombre de especie).
 *
 * @param mensaje Texto que se muestra como prompt (no debe ser NULL).
 * @param buf     Buffer donde se escribe la cadena (no debe ser NULL).
 * @param n       Capacidad del buffer en bytes (debe ser > 0).
 * @param exito   true si se leyó una línea; false ante EOF (el llamador
 *                decide el cierre ordenado).
 */
void validar_leer_cadena(const char *mensaje, char *buf, size_t n,
                         bool *exito);

/**
 * @brief Separa una línea en campos por ';' SIN omitir campos vacíos.
 *
 * A diferencia de strtok, los separadores consecutivos (';;') producen
 * campos vacíos detectables en lugar de un desplazamiento silencioso de los
 * campos siguientes (hallazgo SUGGESTION F2). La línea se modifica en el
 * lugar (se insertan NULs); los punteros resultantes apuntan dentro de ella.
 *
 * @param linea      Línea a separar (no debe ser NULL; se modifica).
 * @param campos     Arreglo donde se guardan los punteros a cada campo.
 * @param max_campos Capacidad del arreglo (debe ser > 0).
 * @param n          Cantidad real de campos contados (incluye los vacíos);
 *                   puede superar max_campos cuando la línea trae más campos
 *                   que la capacidad, para que el llamador detecte el exceso.
 */
void validar_separar_campos(char *linea, char *campos[], int max_campos,
                            int *n);

#endif /* VALIDACION_H */
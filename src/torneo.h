/**
 * @file torneo.h
 * @brief Estado del torneo: grupos, calendario 1-48, clasificación y
 *        eliminatorias 49-64 (RF-TRN-01..06, RF-ELM-01..05, RF-RES-03).
 * @author <Nombre del estudiante>
 * @date 2026-09-01
 */

#ifndef TORNEO_H
#define TORNEO_H

#include <stdbool.h>
#include <stddef.h>

#include "constantes.h"
#include "entrenador.h"

/** @brief Estado de la máquina de estados del torneo (diseño §6.1). */
typedef enum {
    TORNEO_SIN_INICIAR,      /* antes de torneo_armar_grupos */
    TORNEO_GRUPOS,           /* combates 1..48 en curso */
    TORNEO_ELIMINATORIAS,    /* combates 49..64 en curso */
    TORNEO_FINALIZADO        /* resultado 64 aplicado */
} EstadoTorneo;

/** @brief Estado de un resultado de combate (RF-RES-02). */
typedef enum {
    RES_V1,          /* gana el entrenador 1 del combate */
    RES_V2,          /* gana el entrenador 2 del combate */
    RES_EMPATE,      /* solo válido en fase de grupos (1..48) */
    RES_PENDIENTE    /* sin resultado cargado */
} EstadoResultado;

/**
 * @brief Un combate del torneo, indexado por su número (1..64).
 *
 * Los participantes los resuelve el sistema (RF-RES-03), nunca el usuario.
 */
typedef struct {
    int   numero;                /* 1..64 */
    int   id_entrenador1, id_entrenador2;   /* resueltos por el sistema */
    EstadoResultado estado;
    int   id_ganador;            /* id válido; 0 si empate; sin sentido si pendiente */
    int   kos1, kos2;            /* Pokémon derrotados (criterio 3 de RF-TRN-04) */
} Combate;

/**
 * @brief Estado completo del torneo.
 *
 * combates[64] se indexa con numero-1; id_clasificados[16] guarda el orden
 * 1A, 2A, 1B, 2B, ..., 1H, 2H tras la fase de grupos (RF-TRN-06).
 */
typedef struct {
    EstadoTorneo estado;
    Combate combates[TOTAL_COMBATES];
    int  id_clasificados[16];
} Torneo;

/**
 * @brief Resultado de un combate cargado por teclado o archivo (RF-RES-01).
 *
 * Esquema del archivo (D8): NUM_COMBATE;ID_ENT1;ID_ENT2;RESULTADO;GANADOR;KOS1;KOS2.
 */
typedef struct {
    int numero;              /* 1..64 */
    int id_entrenador1, id_entrenador2;
    EstadoResultado resultado;   /* RES_V1, RES_V2 o RES_EMPATE */
    int id_ganador;          /* 0 si empate; debe coincidir con resultado */
    int kos1, kos2;
} ResultadoCargado;

/**
 * @brief Arma los 8 grupos A..H a partir del registro de entrenadores.
 *
 * Exige exactamente MAX_ENTRENADORES (32) entrenadores registrados (D10);
 * la distribución es determinista por orden de registro (posición k =>
 * grupo k/4, puesto k%4).
 *
 * @param t   Puntero al estado del torneo (no debe ser NULL).
 * @param reg Puntero al registro (no debe ser NULL).
 * @return true si se armaron los grupos; false si el conteo no es 32 o
 *         los parámetros son inválidos.
 */
bool torneo_armar_grupos(Torneo *t, const RegistroEntrenadores *reg);

/**
 * @brief Resuelve los participantes del combate indicado (RF-RES-03).
 *
 * Usa la tabla estática de fuentes del bracket: CLASIFICADO(grupo, puesto),
 * GANADOR(n) o PERDEDOR(n). El usuario nunca inventa participantes.
 *
 * Escribe los ids en *id1 y *id2. Si el combate no existe o una fuente
 * aún no está disponible (p. ej. un G# sin resolver), los ids quedan sin
 * definir y la capa de validación lo reporta como "combate no disponible
 * aún" (RF-RES-03, tabla de validación §8.1).
 *
 * @param t      Puntero al estado del torneo (no debe ser NULL).
 * @param numero Número de combate, en el rango 1..64.
 * @param id1    Puntero donde se escribe el id del participante 1.
 * @param id2    Puntero donde se escribe el id del participante 2.
 */
void torneo_participantes(const Torneo *t, int numero, int *id1, int *id2);

/**
 * @brief Aplica un resultado validado al estado del torneo.
 *
 * En grupos actualiza puntos 3/1/0 y contadores; en eliminatoria avanza
 * G#/P#; produce las transiciones GRUPOS -> ELIMINATORIAS -> FINALIZADO.
 *
 * @param t   Puntero al estado del torneo (no debe ser NULL).
 * @param reg Puntero al registro de entrenadores (no debe ser NULL).
 * @param r   Puntero al resultado ya validado (no debe ser NULL).
 * @param msg Buffer donde se escribe un mensaje de resultado (no NULL).
 * @param n   Capacidad del buffer msg.
 * @return true si el resultado se aplicó; false si es inconsistente con
 *         el estado (p. ej. duplicado o ronda incorrecta).
 */
bool torneo_aplicar_resultado(Torneo *t, RegistroEntrenadores *reg,
                              const ResultadoCargado *r, char *msg, size_t n);

/**
 * @brief Ordena los grupos por la cadena completa de desempate (RF-TRN-04).
 *
 * Criterios: 1) puntos desc, 2) victorias desc, 3) pokemon_derrotados desc,
 * 4) enfrentamiento directo (D9), 5) id asc. Rellena id_clasificados[16]
 * con el 1.º y 2.º de cada grupo (RF-TRN-06): ver torneo_clasificados.
 *
 * @param t Puntero al estado del torneo (no debe ser NULL).
 */
void torneo_ordenar_grupos(Torneo *t);

/**
 * @brief Determina los clasificados de cada grupo (RF-TRN-06).
 *
 * Clasifican las posiciones 1 y 2 de cada grupo; el resultado queda en
 * id_clasificados[16] en el orden 1A, 2A, 1B, 2B, ..., 1H, 2H. La
 * ordenación es la cadena completa de RF-TRN-04 (determinista).
 *
 * @param t Puntero al estado del torneo (no debe ser NULL).
 */
void torneo_clasificados(Torneo *t);

/**
 * @brief Muestra la clasificación de los 8 grupos por consola (RF-CLS-01).
 *
 * Por grupo imprime la cabecera [GRUPO X] y 4 filas con el formato
 * POSICION;ID;NOMBRE;VICTORIAS;EMPATES;DERROTAS;PUNTOS;DERROTADOS,
 * ordenadas por la cadena de RF-TRN-04.
 *
 * @param t   Puntero al estado del torneo (no debe ser NULL).
 * @param reg Puntero al registro de entrenadores (no debe ser NULL).
 */
void torneo_mostrar_clasificacion(const Torneo *t,
                                  const RegistroEntrenadores *reg);

/**
 * @brief Muestra el calendario de enfrentamientos de la fase de grupos.
 *
 * Lista los combates 1..48 con su grupo y los participantes resueltos por
 * el sistema (RF-RES-03): NUM;GRUPO;ID1;NOMBRE1;ID2;NOMBRE2.
 *
 * @param t   Puntero al estado del torneo (no debe ser NULL).
 * @param reg Puntero al registro de entrenadores (no debe ser NULL).
 */
void torneo_mostrar_enfrentamientos(const Torneo *t,
                                    const RegistroEntrenadores *reg);

/**
 * @brief Muestra el bracket de la eliminatoria (combates 49-64).
 *
 * Lista cada combate con su ronda, la etiqueta de origen de cada
 * participante (1A, G49, P61, ...), los ids y nombres resueltos por el
 * sistema y el ganador (0 si está pendiente). Si el torneo finalizó,
 * imprime además las posiciones finales (RF-ELM-05).
 *
 * @param t   Puntero al estado del torneo (no debe ser NULL).
 * @param reg Puntero al registro de entrenadores (no debe ser NULL).
 */
void torneo_mostrar_resultados(const Torneo *t,
                               const RegistroEntrenadores *reg);

/**
 * @brief Muestra el campeón del torneo (RF-ELM-05).
 *
 * Solo existe campeón cuando el torneo finalizó (combate 64 aplicado);
 * en otro caso informa que aún no hay campeón.
 *
 * @param t   Puntero al estado del torneo (no debe ser NULL).
 * @param reg Puntero al registro de entrenadores (no debe ser NULL).
 */
void torneo_mostrar_campeon(const Torneo *t,
                            const RegistroEntrenadores *reg);

/**
 * @brief Muestra las posiciones finales del torneo (RF-ELM-05).
 *
 * Campeón = G64, subcampeón = P64, tercero = G63, cuarto = P63.
 *
 * @param t   Puntero al estado del torneo finalizado (no debe ser NULL).
 * @param reg Puntero al registro de entrenadores (no debe ser NULL).
 */
void torneo_mostrar_posiciones_finales(const Torneo *t,
                                       const RegistroEntrenadores *reg);

#endif /* TORNEO_H */
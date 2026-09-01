/**
 * @file constantes.h
 * @brief Constantes de dominio del torneo en un solo lugar (diseño §1.2).
 * @author <Nombre del estudiante>
 * @date 2026-09-01
 */

#ifndef CONSTANTES_H
#define CONSTANTES_H

/* Capacidad de la Pokédex: exactamente 150 especies (RF-PDX-01). */
#define POKEDEX_MAX 150
/* Alias descriptivo de POKEDEX_MAX (mismo valor). */
#define MAX_ESPECIES POKEDEX_MAX

/* Registro máximo de entrenadores: 32 (RF-ENT-01, D10). */
#define MAX_ENTRENADORES 32

/* Catálogo de tipos: 18 (RF-CMB-05); CANT_TIPOS dimensiona la matriz 18x18. */
#define CANT_TIPOS 18
/* Alias descriptivo de CANT_TIPOS (mismo valor). */
#define MAX_TIPOS CANT_TIPOS

/* Tamaño máximo de equipo por entrenador (rango válido 1..6, RF-EQP-05). */
#define MAX_EQUIPO 6
/* Tamaño de equipo exigido en los combates del torneo (spec fija 3, D6). */
#define TAM_EQUIPO_TORNEO 3

/* Rango de niveles de los ejemplares (RF-EQP-05, D7). */
#define NIVEL_MIN 1
#define NIVEL_MAX 50

/* Total de combates del torneo: 48 de grupos + 16 de eliminatoria (D10). */
#define TOTAL_COMBATES 64

/* Tope de turnos de un combate: 1 turno = intercambio completo (D4). */
#define MAX_TURNOS_COMBATE 20

/* Tamaños de buffer para nombres y líneas de archivo (D8). */
#define TAM_MAX_NOMBRE 32  /* nombre de entrenador */
#define TAM_MAX_APODO 16   /* nombre de especie y apodo de ejemplar */
#define TAM_MAX_LINEA 256  /* buffer de fgets para los archivos de texto */

/* Rutas de los archivos de datos (D8): separador ';', UTF-8, sin encabezados. */
#define RUTA_POKEDEX "data/pokedex.txt"
#define RUTA_ENTRENADORES "data/entrenadores.txt"
#define RUTA_RESULTADOS "data/resultados.txt"
#define RUTA_CLASIFICACION "data/clasificacion.txt"

#endif /* CONSTANTES_H */
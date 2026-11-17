/* Jesus Mora - CI 28480557 */
/**
 * @file constantes.h
 * @brief Constantes de dominio del torneo en un solo lugar.
 * @author Jesus Mora
 * @date 2026-09-01
 */

#ifndef CONSTANTES_H
#define CONSTANTES_H

/* Capacidad de la Pokédex: exactamente 150 especies. */
#define POKEDEX_MAX 150
/* Alias descriptivo de POKEDEX_MAX (mismo valor). */
#define MAX_ESPECIES POKEDEX_MAX

/* Registro máximo de entrenadores: 32. */
#define MAX_ENTRENADORES 32

/* Catálogo de tipos: 18; CANT_TIPOS dimensiona la matriz 18x18. */
#define CANT_TIPOS 18
/* Alias descriptivo de CANT_TIPOS (mismo valor). */
#define MAX_TIPOS CANT_TIPOS

/* Tamaño máximo de equipo por entrenador (rango válido 1..6). */
#define MAX_EQUIPO 6
/** El torneo combate con el equipo completo (máximo MAX_EQUIPO). */
#define TAM_EQUIPO_TORNEO 6

/* Rango de niveles de los ejemplares. */
#define NIVEL_MIN 1
/** Nivel máximo permitido (decisión libre del equipo, avalada por el docente). */
#define NIVEL_MAX 100

/* Total de combates del torneo: 48 de grupos + 16 de eliminatoria. */
#define TOTAL_COMBATES 64

/* Combates de la fase de grupos (1..48); el resto (49..64) es eliminatoria. */
#define COMBATES_GRUPO 48

/* Tope de turnos de un combate: 1 turno = intercambio completo. */
#define MAX_TURNOS_COMBATE 20

/* Tamaños de buffer para nombres y líneas de archivo. */
#define TAM_MAX_NOMBRE 32  /* nombre de entrenador */
#define TAM_MAX_APODO 16   /* nombre de especie y apodo de ejemplar */
#define TAM_MAX_LINEA 256  /* buffer de fgets para los archivos de texto */

/* Rutas de los archivos de datos: separador ';', UTF-8, sin encabezados. */
#define RUTA_POKEDEX "data/pokedex.txt"
#define RUTA_EFECTIVIDAD "data/efectividad.txt"
#define RUTA_ENTRENADORES "data/entrenadores.txt"
#define RUTA_RESULTADOS "data/resultados.txt"
#define RUTA_CLASIFICACION "data/clasificacion.txt"

#endif /* CONSTANTES_H */
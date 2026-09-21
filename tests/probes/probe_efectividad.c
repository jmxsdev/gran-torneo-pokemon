/**
 * @file probe_efectividad.c
 * @brief Probe de efectividad de tipos y reglamento de combate (F9, RF-PRB-01):
 *        multiplicadores 2/1/0.5/0 y productos ×4/×0.25 (RF-CMB-05, D1),
 *        daño mínimo 1, orden por velocidad (D3), empate por 20 turnos en
 *        grupos (D4), cadena anti-empate HP -> nivel -> entrenador 1 (D5)
 *        y victoria por agotamiento con y sin reemplazo (RF-CMB-04).
 *
 * Compila contra los módulos reales (todos los .c de src/, sin main.c):
 *   gcc -std=c99 -Wall -Wextra -I src -o probe_efectividad \
 *       src/archivos.c src/combate.c src/entrenador.c src/equipo.c \
 *       src/pokedex.c src/resultados.c src/tipos.c src/torneo.c \
 *       src/validacion.c tests/probes/probe_efectividad.c
 *
 * Los valores esperados se calcularon con la fórmula D1 exacta y se
 * verificaron en el lote F3; el motor de combate no cambió desde entonces.
 *
 * @author <Nombre del estudiante>
 * @date 2026-09-21
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "combate.h"
#include "equipo.h"
#include "entrenador.h"
#include "pokedex.h"
#include "tipos.h"
#include "constantes.h"

static int casos = 0;
static int fallos = 0;

static void verificar(int condicion, const char *nombre)
{
    casos++;
    printf("%s: %s\n", condicion ? "PASS" : "FAIL", nombre);
    if (!condicion) {
        fallos++;
    }
}

/* Stub de selección: elige siempre el primer Pokémon disponible. */
static int stub_seleccionar(const Entrenador *ent, const char *rol)
{
    (void)ent;
    (void)rol;
    return 1;
}

/* Crea un entrenador manual con equipo vacío. */
static Entrenador *crear_entrenador(int id, const char *nombre)
{
    Entrenador *ent = (Entrenador *)malloc(sizeof(Entrenador));
    ent->id = id;
    snprintf(ent->nombre, sizeof(ent->nombre), "%s", nombre);
    ent->victorias = 0;
    ent->empates = 0;
    ent->derrotas = 0;
    ent->puntos = 0;
    ent->pokemon_derrotados = 0;
    ent->equipo = NULL;
    return ent;
}

/* Crea y agrega un ejemplar con stats controladas (no pasa por la Pokédex). */
static void crear_ejemplar(Entrenador *ent, Tipo t1, Tipo t2, int hp,
                           int atq, int def, int vel, int nivel,
                           const char *nombre)
{
    Ejemplar *ej = (Ejemplar *)malloc(sizeof(Ejemplar));
    ej->id = equipo_siguiente_id();
    ej->numero_especie = 0;
    snprintf(ej->nombre, sizeof(ej->nombre), "%s", nombre);
    ej->nivel = nivel;
    ej->hp_max = hp;
    ej->ataque = atq;
    ej->defensa = def;
    ej->velocidad = vel;
    ej->hp_actual = hp;
    ej->tipo_primario = t1;
    ej->tipo_secundario = t2;
    ej->siguiente = NULL;
    (void)equipo_agregar_ejemplar(ent, ej);
}

/* Libera el equipo y el entrenador. */
static void liberar_entrenador(Entrenador *ent)
{
    equipo_liberar(ent);
    free(ent);
}

int main(void)
{
    Pokedex pd;
    const Especie *esp;
    Ejemplar *atacante;
    Ejemplar *defensor;
    Entrenador *a;
    Entrenador *b;
    ResultadoCombate res;
    Ejemplar rapido;
    Ejemplar lento;

    tipos_inicializar();
    if (!pokedex_cargar(&pd, RUTA_POKEDEX)) {
        printf("FAIL: no se pudo cargar la Pokédex (%s).\n", RUTA_POKEDEX);
        return 1;
    }

    /* ---- Multiplicadores exactos con especies reales (D2/D1) ---------- */
    esp = pokedex_buscar_numero(&pd, 4);   /* Charmander, Fuego */
    atacante = equipo_crear_ejemplar(&pd, esp->numero, "Charmander", 50, 1);
    esp = pokedex_buscar_numero(&pd, 1);   /* Bulbasaur, Planta/Veneno */
    defensor = equipo_crear_ejemplar(&pd, esp->numero, "Bulbasaur", 50, 2);
    verificar(combate_calcular_danio(atacante, defensor) == 46,
              "efectividad ×2 Fuego->Planta/Veneno: daño 46");
    free(atacante);
    free(defensor);

    esp = pokedex_buscar_numero(&pd, 39);  /* Jigglypuff, Normal/Hada */
    atacante = equipo_crear_ejemplar(&pd, esp->numero, "Jigglypuff", 50, 3);
    esp = pokedex_buscar_numero(&pd, 1);   /* Bulbasaur */
    defensor = equipo_crear_ejemplar(&pd, esp->numero, "Bulbasaur", 50, 2);
    verificar(combate_calcular_danio(atacante, defensor) == 21,
              "efectividad ×1 Normal->Planta/Veneno: daño 21");
    free(atacante);
    free(defensor);

    esp = pokedex_buscar_numero(&pd, 39);  /* Jigglypuff, Normal */
    atacante = equipo_crear_ejemplar(&pd, esp->numero, "Jigglypuff", 50, 3);
    esp = pokedex_buscar_numero(&pd, 74);  /* Geodude, Roca/Tierra */
    defensor = equipo_crear_ejemplar(&pd, esp->numero, "Geodude", 50, 4);
    verificar(combate_calcular_danio(atacante, defensor) == 7,
              "efectividad ×0.5 Normal->Roca/Tierra: daño 7");
    free(atacante);
    free(defensor);

    esp = pokedex_buscar_numero(&pd, 25);  /* Pikachu, Eléctrico */
    atacante = equipo_crear_ejemplar(&pd, esp->numero, "Pikachu", 50, 5);
    esp = pokedex_buscar_numero(&pd, 74);  /* Geodude, Roca/Tierra */
    defensor = equipo_crear_ejemplar(&pd, esp->numero, "Geodude", 50, 4);
    verificar(combate_calcular_danio(atacante, defensor) == 0,
              "efectividad ×0 Eléctrico->Tierra: daño 0 (sin efecto)");
    free(atacante);
    free(defensor);

    esp = pokedex_buscar_numero(&pd, 4);   /* Charmander, Fuego */
    atacante = equipo_crear_ejemplar(&pd, esp->numero, "Charmander", 50, 1);
    esp = pokedex_buscar_numero(&pd, 46);  /* Paras, Bicho/Planta */
    defensor = equipo_crear_ejemplar(&pd, esp->numero, "Paras", 50, 6);
    verificar(combate_calcular_danio(atacante, defensor) == 88,
              "efectividad ×4 Fuego->Bicho/Planta: daño 88");
    free(atacante);
    free(defensor);

    esp = pokedex_buscar_numero(&pd, 4);   /* Charmander, Fuego */
    atacante = equipo_crear_ejemplar(&pd, esp->numero, "Charmander", 50, 1);
    esp = pokedex_buscar_numero(&pd, 138); /* Omanyte, Roca/Agua */
    defensor = equipo_crear_ejemplar(&pd, esp->numero, "Omanyte", 50, 7);
    verificar(combate_calcular_danio(atacante, defensor) == 4,
              "efectividad ×0.25 Fuego->Roca/Agua: daño 4");
    free(atacante);
    free(defensor);

    /* ---- Daño mínimo 1 con mult > 0 (RF-CMB-03) ------------------------ */
    esp = pokedex_buscar_numero(&pd, 4);   /* Charmander, Fuego */
    atacante = equipo_crear_ejemplar(&pd, esp->numero, "Charmander", 1, 16);
    esp = pokedex_buscar_numero(&pd, 138); /* Omanyte, Roca/Agua */
    defensor = equipo_crear_ejemplar(&pd, esp->numero, "Omanyte", 1, 17);
    verificar(combate_calcular_danio(atacante, defensor) == 1,
              "daño mínimo 1 con mult 0.25 y base 2");
    free(atacante);
    free(defensor);

    /* ---- Orden por velocidad (D3, RF-CMB-02) --------------------------- */
    rapido.nivel = 50;
    rapido.ataque = 50;
    rapido.defensa = 50;
    rapido.velocidad = 60;
    rapido.tipo_primario = TIPO_NORMAL;
    rapido.tipo_secundario = TIPO_NINGUNO;
    lento = rapido;
    lento.velocidad = 45;
    verificar(combate_ataca_primero(&rapido, &lento) == true,
              "D3 mayor velocidad: ataca primero el local (60 vs 45)");
    verificar(combate_ataca_primero(&lento, &rapido) == false,
              "D3 menor velocidad: ataca primero el visitante (45 vs 60)");
    lento.velocidad = 60;
    verificar(combate_ataca_primero(&rapido, &lento) == true,
              "D3 igual velocidad: empate -> ataca el entrenador 1");

    /* ---- Empate por 20 turnos en grupos (D4, RF-CMB-03) ---------------- */
    a = crear_entrenador(1, "Local");
    crear_ejemplar(a, TIPO_FANTASMA, TIPO_NINGUNO, 100, 10, 10, 10, 10, "F1");
    crear_ejemplar(a, TIPO_FANTASMA, TIPO_NINGUNO, 100, 10, 10, 10, 10, "F2");
    b = crear_entrenador(2, "Visita");
    crear_ejemplar(b, TIPO_NORMAL, TIPO_NINGUNO, 100, 10, 10, 10, 10, "N1");
    crear_ejemplar(b, TIPO_NORMAL, TIPO_NINGUNO, 100, 10, 10, 10, 10, "N2");
    verificar(combate_ejecutar(a, b, false, stub_seleccionar, &res) == true,
              "D4 ejecución de combate en grupos OK");
    verificar(res.empate == true && res.id_ganador == 0 &&
              res.kos_local == 0 && res.kos_visita == 0,
              "D4 empate tras 20 turnos (ambos equipos vivos)");
    liberar_entrenador(a);
    liberar_entrenador(b);

    /* ---- Anti-empate por mayor HP total (D5a) -------------------------- */
    a = crear_entrenador(1, "Local");
    crear_ejemplar(a, TIPO_FANTASMA, TIPO_NINGUNO, 100, 10, 10, 10, 10, "F1");
    crear_ejemplar(a, TIPO_FANTASMA, TIPO_NINGUNO, 100, 10, 10, 10, 10, "F2");
    b = crear_entrenador(2, "Visita");
    crear_ejemplar(b, TIPO_NORMAL, TIPO_NINGUNO, 80, 10, 10, 10, 30, "N1");
    crear_ejemplar(b, TIPO_NORMAL, TIPO_NINGUNO, 80, 10, 10, 10, 30, "N2");
    verificar(combate_ejecutar(a, b, true, stub_seleccionar, &res) == true,
              "D5a ejecución en eliminatoria OK");
    verificar(res.empate == false && res.id_ganador == 1,
              "D5a gana por mayor HP total (200 vs 160) -> local");
    liberar_entrenador(a);
    liberar_entrenador(b);

    /* ---- Anti-empate por mayor nivel total (D5b) ----------------------- */
    a = crear_entrenador(1, "Local");
    crear_ejemplar(a, TIPO_FANTASMA, TIPO_NINGUNO, 100, 10, 10, 10, 10, "F1");
    crear_ejemplar(a, TIPO_FANTASMA, TIPO_NINGUNO, 100, 10, 10, 10, 10, "F2");
    b = crear_entrenador(2, "Visita");
    crear_ejemplar(b, TIPO_NORMAL, TIPO_NINGUNO, 100, 10, 10, 10, 15, "N1");
    crear_ejemplar(b, TIPO_NORMAL, TIPO_NINGUNO, 100, 10, 10, 10, 15, "N2");
    verificar(combate_ejecutar(a, b, true, stub_seleccionar, &res) == true &&
              res.empate == false && res.id_ganador == 2,
              "D5b HP empatado -> gana por mayor nivel total (30 vs 20)");
    liberar_entrenador(a);
    liberar_entrenador(b);

    /* ---- Anti-empate por entrenador 1 (D5c) ---------------------------- */
    a = crear_entrenador(1, "Local");
    crear_ejemplar(a, TIPO_FANTASMA, TIPO_NINGUNO, 100, 10, 10, 10, 10, "F1");
    crear_ejemplar(a, TIPO_FANTASMA, TIPO_NINGUNO, 100, 10, 10, 10, 10, "F2");
    b = crear_entrenador(2, "Visita");
    crear_ejemplar(b, TIPO_NORMAL, TIPO_NINGUNO, 100, 10, 10, 10, 10, "N1");
    crear_ejemplar(b, TIPO_NORMAL, TIPO_NINGUNO, 100, 10, 10, 10, 10, "N2");
    verificar(combate_ejecutar(a, b, true, stub_seleccionar, &res) == true &&
              res.empate == false && res.id_ganador == 1,
              "D5c HP y nivel empatados -> gana el entrenador 1");
    liberar_entrenador(a);
    liberar_entrenador(b);

    /* ---- Victoria por agotamiento sin reemplazo (RF-CMB-04) ------------ */
    a = crear_entrenador(1, "Local");
    crear_ejemplar(a, TIPO_AGUA, TIPO_NINGUNO, 10, 10, 10, 10, 10, "W1");
    b = crear_entrenador(2, "Visita");
    crear_ejemplar(b, TIPO_ELECTRICO, TIPO_NINGUNO, 100, 100, 50, 50, 50, "E1");
    verificar(combate_ejecutar(a, b, false, stub_seleccionar, &res) == true &&
              res.empate == false && res.id_ganador == 2 &&
              res.kos_visita == 1 && res.kos_local == 0,
              "CMB-04 victoria por agotamiento (KO sin reemplazo)");
    liberar_entrenador(a);
    liberar_entrenador(b);

    /* ---- Victoria por agotamiento con reemplazo (RF-CMB-04) ------------ */
    a = crear_entrenador(1, "Local");
    crear_ejemplar(a, TIPO_AGUA, TIPO_NINGUNO, 10, 10, 10, 10, 10, "W1");
    crear_ejemplar(a, TIPO_AGUA, TIPO_NINGUNO, 10, 10, 10, 10, 10, "W2");
    b = crear_entrenador(2, "Visita");
    crear_ejemplar(b, TIPO_ELECTRICO, TIPO_NINGUNO, 100, 100, 50, 50, 50, "E1");
    verificar(combate_ejecutar(a, b, false, stub_seleccionar, &res) == true &&
              res.empate == false && res.id_ganador == 2 &&
              res.kos_visita == 2,
              "CMB-04 KO con reemplazo: se elige otro Pokémon y se continúa");
    liberar_entrenador(a);
    liberar_entrenador(b);

    /* ---- Validación de parámetros ------------------------------------- */
    a = crear_entrenador(1, "Local");
    crear_ejemplar(a, TIPO_NORMAL, TIPO_NINGUNO, 50, 20, 20, 20, 10, "N1");
    verificar(combate_ejecutar(a, NULL, false, stub_seleccionar, &res) ==
              false, "validación: entrenador NULL -> false");
    verificar(combate_ejecutar(a, a, false, NULL, &res) == false,
              "validación: seleccionar NULL -> false");
    b = crear_entrenador(2, "Visita");   /* equipo vacío */
    verificar(combate_ejecutar(a, b, false, stub_seleccionar, &res) == false,
              "validación: equipo vacío -> false");
    liberar_entrenador(a);
    liberar_entrenador(b);

    printf("\nPROBE EFECTIVIDAD: %d/%d comprobaciones PASS%s\n",
           casos - fallos, casos, fallos == 0 ? " (todas)" : "");
    return fallos == 0 ? 0 : 1;
}
# Apuntes técnicos para la defensa oral

**Fecha**: 2026-09-21
**Propósito**: respuestas claras a las preguntas técnicas surgidas durante el
desarrollo, con el código real del proyecto como evidencia. Cada sección cierra
con una frase lista para usar en la defensa.

---

## 1. La compilación: Makefile y convenciones

En clase se compila con `gcc main.c -o main` porque es **un solo archivo**. Este
proyecto tiene 10 módulos, y el comando manual completo sería:

```bash
gcc -std=c99 -Wall -Wextra -o build/torneo src/*.c
```

El `Makefile` es **ese comando, escrito una sola vez y automatizado**:

```makefile
CC      := gcc
CFLAGS  := -std=c99 -Wall -Wextra
TARGET  := build/torneo
SRCS    := $(wildcard src/*.c)

.PHONY: all run clean entrega

all: $(TARGET)

$(TARGET): $(SRCS)
	mkdir -p build
	$(CC) $(CFLAGS) -o $@ $(SRCS)

run: all
	./$(TARGET)

clean:
	rm -rf build
```

Convenciones usadas (estándar GNU Make):
- `CC` y `CFLAGS`: nombres convencionales que cualquier lector de Makefiles espera.
- `$(wildcard src/*.c)`: incluye todos los `.c` automáticamente (agregar un
  módulo no requiere tocar el Makefile).
- `all` como primer objetivo → `make` a secas compila.
- `.PHONY`: declara que `all/run/clean/entrega` son tareas, no archivos.
- `$@`: variable automática = nombre del objetivo.
- `build/`: el binario sale fuera del árbol fuente; `clean` lo borra.
- `-std=c99` fija el estándar del curso; `-Wall -Wextra` con **cero warnings** es
  el gate de calidad del proyecto. `entrega` es la única tarea con nombre en
  español (desviación deliberada por el idioma del proyecto).

Comandos: `make` (compila), `make run`, `make clean`, `make entrega`.

Otros archivos de configuración:
- **`.gitignore`**: excluye lo regenerable (`build/`, `dist/`,
  `data/clasificacion.txt` que es salida del programa, `docs/doxygen/`, `tools/`).
- **`Doxyfile`**: configuración de Doxygen (`OUTPUT_LANGUAGE=Spanish`,
  `INPUT=src docs`); se generó con `doxygen -g Doxyfile` y se editó.
- **`tests/run_tests.sh`**: ejecutor de la batería (compila, corre casos
  stdin→diff y reporta PASS/FAIL).
- **`scripts/empaquetar.sh` / `scripts/verificar_entrega.sh`**: generan y
  verifican el zip de entrega.

> **Frase de defensa**: "El Makefile encapsula el comando gcc del proyecto con
> las banderas del curso y cero warnings como gate; el binario se genera en
> `build/`, fuera del código fuente, y agregar un módulo no requiere editarlo
> porque usa `wildcard`."

---

## 2. `static` en C

Demostrado experimentalmente en `docs/experimento-static.md` (con `nm`). Resumen:

- En **funciones y variables globales**: enlace interno = la función es
  **privada de su archivo**; su nombre no se exporta a los demás módulos.
- En **variables locales**: duración estática (vive todo el programa, conserva
  su valor entre llamadas).
- `main.c` tiene 17 funciones `static` (sus ayudantes internos); `main` no es
  static porque el runtime de C la llama desde fuera.
- En `nm`, los static aparecen con `t` minúscula (local) y los públicos con `T`
  mayúscula (global). Quitarle el static a un ayudante produce "definiciones
  múltiples" si otro archivo define el mismo nombre.

> **Frase de defensa**: "`static` da enlace interno: la función es privada de su
> archivo. Encapsula la implementación, evita colisiones de nombres entre
> módulos y se comprueba con `nm` (símbolos locales `t`). Es el `private` de C."

---

## 3. `feof(stdin)` y el cierre ordenado ante EOF

- `stdin` = flujo de entrada estándar (`FILE *` de `<stdio.h>`): teclado o
  tubería (`./torneo < caso.in`).
- `feof(flujo)` = verdadero si el **indicador de fin de entrada ya se activó**.
  No predice: solo reporta algo que ya pasó (una lectura tocó el final).
- EOF ocurre con **Ctrl+D** en terminal o al agotarse la entrada por tubería.

El problema que resuelve en `main.c`: `leer_entero_validado` devuelve `0` cuando
`scanf` golpea EOF, pero `0` también es un valor que el usuario puede teclear.
El llamador distingue ambos casos:

```c
id = leer_id_entrenador("Ingrese el id del entrenador: ");
if (id == 0 && feof(stdin)) {
    return;   /* EOF: cierre ordenado */
}
if (id <= 0) {
    printf("Id inválido: debe ser un entero positivo.\n");
    return;
}
```

Sin este chequeo, al agotarse la entrada (tests, redirecciones) el programa
respondería "Id inválido" indefinidamente en vez de cerrar con código 0. La
batería F8 verifica "EOF a media entrada → exit 0, sin colgar".

> **Frase de defensa**: "`feof` consulta si la última lectura tocó el fin de la
> entrada; lo usamos para distinguir un 0 tecleado de un EOF y cerrar
> ordenadamente — ninguna entrada termina el programa de forma inesperada
> (RF-TEC-03)."

---

## 4. `void`: tres significados en el proyecto

| Posición | Significado | Ejemplo del proyecto |
|---|---|---|
| Antes del nombre (retorno) | No devuelve nada | `static void mostrar_menu(void)` |
| En la lista de parámetros `(void)` | No recibe nada | `leer_opcion(void)` |
| Retorno `void` + parámetros por referencia | Procedimiento multi-salida: entrega sus resultados escribiendo los punteros | `void validar_leer_cadena(..., bool *exito)` |

Son dimensiones independientes:

```c
static void mostrar_menu(void)                    // no retorna, no recibe
static void consultar_pokedex(const Pokedex *pd)  // no retorna, RECIBE un puntero
static int  leer_opcion(void)                     // retorna int, no recibe
void validar_leer_cadena(const char *msg, char *buf, size_t n,
                         bool *exito)             // no retorna; ESCRIBE *exito
```

El **tercer uso** es la convención del curso (refactor 2026-09-25): cuando una
operación produce más de un resultado — p. ej. `validar_leer_cadena` escribe el
`buf` y además el éxito — se declara `void` y las salidas viajan por parámetros
por referencia, con el éxito al final (`bool *exito`). Es la diferencia entre
**función** (devuelve un único valor con `return`) y **procedimiento** (entrega
sus resultados por referencia). Las 23 funciones que devolvían `bool`/`int` y
además mutaban parámetros se convirtieron a este patrón; las puras (un solo
`return`) se conservan como funciones.

La opción 1 del menú ejecuta `consultar_pokedex(const Pokedex *pd)`: recibe un
**puntero** a la Pokédex (no una copia — sería inútil copiar 150 especies) y el
`const` garantiza que solo la **lee**, jamás la modifica (inmutabilidad de la
Pokédex, requisito central).

`(void)` en vez de `()`: en C, `f()` significa "parámetros no especificados"
(estilo antiguo, sin verificación); `f(void)` es la forma explícita y segura.
Convención usada en todo el proyecto.

> **Frase de defensa**: "`void` como retorno significa que no devuelve nada;
> `(void)` en los parámetros significa que no recibe nada; y `void` con
> parámetros por referencia es el procedimiento multi-salida de la convención:
> la operación entrega sus resultados escribiendo los punteros, con el éxito al
> final. Lo que una función recibe lo define su lista de parámetros, no el
> `void`."

---

## 5. La bandera `entrenadores_sucios` (patrón *dirty flag*)

Responde a una pregunta: **¿los datos en memoria difieren de los del disco?**

```c
static bool entrenadores_sucios = false;   // arranca limpio

// Se marca en los TRES puntos donde se MUTAN los entrenadores:
entrenadores_sucios = true;   // registrar entrenador (línea 184)
entrenadores_sucios = true;   // formar equipo automático (246)
entrenadores_sucios = true;   // crear equipo manual (299)

// Al salir (opción 12) se consulta:
static void guardar_al_salir(const RegistroEntrenadores *reg)
{
    bool exito;

    if (entrenadores_sucios) {            // solo si hubo cambios
        archivos_guardar_entrenadores(reg, RUTA_ENTRENADORES, &exito);
        if (exito) {
            printf("Entrenadores guardados en %s.\n", RUTA_ENTRENADORES);
        } else {
            printf("Aviso: no se pudieron guardar los entrenadores.\n");
        }
    }
}
```

Por qué no guardar siempre:
1. Una sesión de **solo lectura** (consultar Pokédex, ver clasificación) deja el
   archivo **byte a byte intacto** (sus sha256 no cambian).
2. Un guardado ciego **normalizaría el orden** de los equipos (las listas
   enlazadas insertan al frente, el orden mostrado es inverso al del archivo).
3. Escribir en disco solo cuando hay cambios reales reduce I/O y riesgo de
   corrupción a mitad de escritura.

Es el mismo patrón del asterisco de los editores de texto ("modificado, sin
guardar").

> **Frase de defensa**: "Es una bandera de cambios pendientes: separa el estado
> en memoria del estado en disco. Solo se persiste si hubo mutaciones reales, lo
> que protege los archivos de ejemplo y permite que las pruebas verifiquen que
> las sesiones de solo lectura no alteran ningún dato."

---

## 6. `snprintf` y la seguridad de buffers

```c
char msg_cantidad[64];
snprintf(msg_cantidad, sizeof(msg_cantidad),
         "Cantidad inválida: debe estar entre 1 y %d.", MAX_EQUIPO);
```

- `snprintf` formatea texto con `%d`, `%s`, etc. **hacia un string** (no a
  pantalla como `printf`), garantizando no escribir más de `tamaño` bytes
  (incluido el `\0`); si no cabe, **trunca**. `sprintf` haría lo mismo pero
  **sin límite** → desbordamiento de buffer.
- `sizeof(msg_cantidad)` = capacidad del buffer, de forma que si el array cambia
  de tamaño la llamada no se toca.
- El mensaje se construye dinámicamente porque el límite viene de
  `constantes.h` (`MAX_EQUIPO`): si el máximo del equipo cambia, el mensaje se
  actualiza solo (fuente única de verdad).
- Regla del proyecto: **nunca `sprintf`; siempre `snprintf` con `sizeof`**.

> **Frase de defensa**: "`snprintf` escribe en un buffer respetando su tamaño —
> nunca desborda. Construimos los mensajes con la constante real
> (`MAX_EQUIPO`) para que no queden desactualizados si el valor cambia."

---

## 7. Structs y TDA: `RestriccionesEquipo` es un formulario, no lógica

En C un `struct` es **solo datos** (no tiene métodos, eso es de otros lenguajes).
El struct es un **formulario**; la lógica vive en las **funciones que lo
reciben**:

1. **`main.c` llena el formulario** (`formar_equipo_automatico`, opción 3):
   `restricciones.cantidad`, `nivel_total_max`, `min_tipos`, etc.
2. **`main.c` lo entrega**: `equipo_formar_backtracking(pd, &restricciones, ...)`.
3. **`equipo.c` lo valida**: la función rechaza cantidad fuera de 1..MAX_EQUIPO,
   nivel ≤ 0, tipos fuera de rango o Pokédex vacía.
4. **`equipo.c` lo usa**: `bt_rec` lee los campos en cada poda y decisión
   (`r->nivel_total_max`, `r->min_tipos`, …) para buscar el equipo.

Cada condición de la sección 1.5 del enunciado (cantidad determinada, nivel
total máximo, mínimo de tipos, estadísticas) es un **campo del struct**. Se pasa
como `const RestriccionesEquipo *` (puntero): evita copiar el struct completo
(incluye un arreglo de 150 enteros) y el `const` garantiza que solo se lee.

El conjunto **tipo + funciones del módulo** es el TDA: quien lo usa no necesita
saber *cómo* se buscan las combinaciones, solo llena el formulario y llama.

> **Frase de defensa**: "`RestriccionesEquipo` es un formulario de datos, no
> lógica: `main.c` lo llena con las respuestas del usuario, y las funciones de
> `equipo.c` lo validan y lo consumen en el backtracking. Es la traducción
> directa de las condiciones del enunciado a un TDA."

---

## 8. Los TDA del proyecto (uno por módulo)

Un **Tipo de Dato Abstracto (TDA)** = un dato + sus operaciones, empaquetados.
En C se materializa como **módulo**: el `.h` es el *contrato* público y el `.c`
la *implementación encapsulada*. Quien usa el TDA solo conoce el contrato y
**jamás toca los campos internos**.

| TDA | Dato (struct) | Operaciones públicas | Quién lo usa |
|---|---|---|---|
| Pokédex | `Especie` + tabla de 150 | `pokedex_cargar`, `buscar_numero/nombre`, `mostrar_*` | main |
| Catálogo de tipos | `enum Tipo` + matriz 18×18 | `tipos_inicializar`, `tipos_es_valido`, `tipos_multiplicador` | pokedex, combate |
| Registro de entrenadores | `Entrenador[32]` | `entrenador_registrar`, `buscar`, `mostrar_todos` | main |
| Equipo/Ejemplar | `Ejemplar` (lista enlazada) | `crear`, `agregar`, `validar`, `liberar`, `asignar`, `formar_backtracking` | main, archivos |
| Combate | `ResultadoCombate`, callback | `calcular_danio`, `ataca_primero`, `ejecutar` | main |
| Torneo | `Combate[64]` + estado | `armar_grupos`, `participantes`, `aplicar_resultado`, `ordenar`, `clasificados`, `mostrar_*` | main, resultados |
| Archivos | — | `cargar/guardar_*` | main |
| Validación | — | `validar_leer_entero/cadena`, `separar_campos` | todos |

**Pruebas de encapsulamiento en el propio código**:
- `equipo.h` §asignar: *"Encapsula el TDA: solo este módulo toca el campo
  `siguiente` de los ejemplares."* — `main.c` nunca escribe `ej->siguiente`,
  solo llama `equipo_agregar_ejemplar(ent, ej)`.
- La matriz de efectividad es `static` en `tipos.c` (MINOR-1): nadie la lee
  directamente, solo vía `tipos_multiplicador(...)`.

**Beneficio**: si mañana la lista enlazada se cambia por un arreglo dinámico,
ningún otro módulo se entera. El TDA es lo que hace el refactor "de un solo
resultado" posible: se tocó el interior de cada módulo sin cambiar contratos
de comportamiento.

> **Frase de defensa**: "Cada módulo es un TDA: un tipo de dato con sus
> operaciones, contrato en el `.h` e implementación encapsulada en el `.c`.
> El usuario del TDA jamás manipula los campos internos — por eso cambiar la
> representación interna no afecta al resto del sistema."

## 9. El backtracking a fondo (RF-EQP-04)

### El problema

Formar un equipo que cumpla restricciones (cantidad, nivel total máximo,
mínimo de tipos, estadísticas, integrantes permitidos) eligiendo entre 150
especies. Fuerza bruta: C(150,6) ≈ 1,4·10¹⁰ — inviable. Backtracking =
**exploración con retroceso**: construir paso a paso y, cuando una parcial ya
no puede alcanzar una solución válida (podas), retroceder y probar la
alternativa.

### La función real (`src/equipo.c`, `bt_rec`)

```c
static void bt_rec(pd, r, tipos_sufijo, i, restantes, nivel_acum,
                   tipos_dist, ataque_acum, mascara,
                   parcial, creados, exito)
```

- **Estado**: viaja en los parámetros — `i` especie actual, `restantes`
  pendientes, acumuladores de nivel/tipos/ataque, máscara de tipos vistos y la
  lista `parcial` en construcción.
- **Casos base**: `restantes == 0` → ¿cumple TODO? (`tipos_dist >= min_tipos &&
  ataque_acum >= ataque_total_min`); `i >= cantidad` → fracaso.
- **Podas** (donde el algoritmo "piensa"):
  - Poda 1 — cantidad: no quedan especies suficientes.
  - Poda 2 — cota inferior de nivel: `nivel_acum + restantes*NIVEL_MIN >
    nivel_total_max` → imposible por presupuesto (el caso "6 Pokémon, nivel
    total máx 5" muere en profundidad 1).
  - Poda 3 — cota de tipos con sufijo precomputado: los tipos aún alcanzables
    no bastan para `min_tipos`.
  - Poda 3b — tipos por ejemplar restante: `tipos_dist + 2*restantes <
    min_tipos` (cada ejemplar aporta a lo sumo 2 tipos).
  - Integrantes — si la especie no está en `especies_permitidas`, solo se
    puede excluir.
- **Ramificar**: k copias de la especie i (1..`restantes` si
  `permitir_repetidas`; solo 1 si no); crear los ejemplares (fórmula D2,
  nivel mínimo factible, o máximo si hay objetivo de ataque); recursión
  `bt_rec(i+1, restantes-k, ...)`; salida temprana `if (*exito) return;`.
- **Retroceder**: liberar las k copias (sin fugas) y probar excluir la
  especie. La pila del sistema guarda cada estado parcial: retornar =
  deshacer el último paso. Por eso no hay pila explícita.

### Dos trazas reales

**Traza A — éxito con retroceso**: restricciones: 2 Pokémon, sin repetidas,
mínimo 3 tipos distintos. Candidatos en orden: Charmander (Fuego), Squirtle
(Agua), Bulbasaur (Planta/Veneno).

1. `i=0` Charmander → incluir: tipos=1 → recursión `i=1, restantes=1`.
2. `i=1` Squirtle → incluir: tipos=2 → `restantes=0`: **¿2 ≥ 3? NO** → fracaso.
3. **Retroceder**: se libera el ejemplar de Squirtle y se prueba excluirlo:
   recursión `i=2, restantes=1` (Charmander sigue incluido).
4. `i=2` Bulbasaur → incluir: aporta 2 tipos nuevos → tipos=3 → `restantes=0`:
   **¿3 ≥ 3? SÍ** → ¡SOLUCIÓN! `{Charmander, Bulbasaur}`.

El algoritmo intentó `{Charmander, Squirtle}`, falló, **deshizo** a Squirtle y
encontró la alternativa válida. Ese "probar → fallar → liberar → alternativa"
ES el backtracking.

**Traza B — poda instantánea**: mínimo 5 tipos con solo 2 Pokémon. En la raíz,
poda 3b: `0 + 2*2 = 4 < 5` → cierto → se informa "no existe solución" sin
explorar nada. Con estos candidatos el máximo real son 3 tipos (Charmander es
mono-tipo), así que "mínimo 4" también falla — pero en ese caso la búsqueda
recorre las combinaciones y el **caso base** las descarta (la poda 3b solo
corta cuando ni el óptimo teórico de 2 tipos por ejemplar alcanza).

> **Frase de defensa**: "El backtracking explora combinaciones con podas:
> corta cualquier rama que matemáticamente no pueda alcanzar la solución y
> retrocede liberando lo probado. Garantiza encontrar un equipo que cumpla
> TODAS las restricciones o informar que no existe, sin devolver nunca un
> equipo inválido."

## 10. Cómo modificar el sistema: el mapa de módulos

**Método general (4 pasos)**:
1. Traduce el cambio a un requisito: `grep -rn "RF-" src/` o
   `docs/trazabilidad-requisitos.md` (cada RF apunta a su módulo).
2. Abre la cabecera (`.h`) del módulo: el contrato Doxygen dice qué hace cada
   función.
3. Edita la implementación (`.c`). Si cambias la *firma*, actualiza `.h` +
   todos los llamadores (`grep -rn nombre_funcion src/ tests/`).
4. Verifica: `make` (cero warnings) + `bash tests/run_tests.sh` (23 PASS). Si
   cambias la salida visible, regenera los goldens `tests/casos/*.esperado`.

**Mapa "quiero cambiar X → módulo"**:

| Quieres cambiar… | Archivo | Función/constante |
|---|---|---|
| La fórmula de daño | `src/combate.c` | `combate_calcular_danio` (D1) |
| Las stats de los ejemplares | `src/equipo.c` | `equipo_crear_ejemplar` (D2) |
| Puntuación o desempates | `src/torneo.c` | `torneo_aplicar_resultado`, `torneo_ordenar_grupos` |
| El bracket de eliminatorias | `src/torneo.c` | tabla `ORIGEN_PARTICIPANTE` |
| La efectividad de tipos | `src/tipos.c` + `data/efectividad.txt` | matriz + `tipos_multiplicador` |
| El formato de un archivo | `src/archivos.c` + parser del módulo | funciones `archivos_*` |
| El menú (textos u opciones) | `src/main.c` | `mostrar_menu` + `switch` |
| Un límite (equipo, niveles, rutas) | `src/constantes.h` | un solo lugar, por diseño |
| La validación de entrada | `src/validacion.c` | `validar_leer_*` |
| Una regla de negocio nueva | módulo del dominio + llamada desde `main.c` | la función del TDA |

**Lección del refactor ya vivido**: el cambio de "un solo resultado" tocó 23
firmas (8 módulos + 2 probes) y **los 20 goldens no cambiaron** — porque el
*comportamiento* no cambió. Cambio de interfaz ≠ cambio de comportamiento, y
las pruebas distinguen ambos.

**Ejercicios típicos de la defensa**:
1. "Cambia el criterio D9" → `torneo_ordenar_grupos` en `src/torneo.c` (1
   función) + batería.
2. "Haz la variación de stats 0–31" → `equipo_crear_ejemplar` en
   `src/equipo.c` (cambiar `% 16` por `% 32`) + tabla del informe.
3. "Agrega la opción 9 del historial" → registrar combates + `case 9` en
   `main.c` + goldens.

---

## Referencias

- `docs/experimento-static.md` — demostración experimental de `static` con `nm`.
- `docs/informe-tecnico.md` — arquitectura, fórmulas, D1–D10 y diagramas.
- `docs/planificacion.md` — plan por fases y checklist de entrega.

# Convenciones de documentación Doxygen

**Proyecto**: El Gran Torneo Pokémon · **Curso**: Programación I / Algoritmos I
**Documento**: DOC-01 (spec `documentacion`) · **Vigencia**: desde F0, obligatoria en todo el código
**Idioma**: español neutro y profesional (requisito explícito del proyecto)

---

## 1. Propósito

Este documento fija la convención Doxygen obligatoria para todo archivo `.c` y `.h`
del proyecto. Su objetivo es que la documentación generada sea uniforme, esté en
español y permita la defensa oral del informe técnico sin ambigüedades.

## 2. Reglas obligatorias

| # | Regla |
|---|-------|
| 1 | Todo archivo `.c` y `.h` lleva un bloque `@file` de cabecera. |
| 2 | Toda función pública lleva bloque con `@brief`, `@param` (uno por parámetro) y `@return`. |
| 3 | Las funciones `static` llevan un comentario breve de una línea (no bloque Doxygen completo). |
| 4 | El texto de los comentarios se redacta en español neutro y profesional. |
| 5 | Los identificadores C se escriben en español `snake_case` SIN acentos (p. ej. `clasificacion`, `validacion`, `ejemplar`, `torneo`). |
| 6 | El ancho máximo de línea es 99 columnas. |
| 7 | Se usan los bloques `@file`, `@brief`, `@param`, `@return` y `@author`. |
| 8 | `@date` indica la fecha de creación del archivo (formato AAAA-MM-DD). |
| 9 | Las referencias a requisitos del enunciado se anotan entre paréntesis (p. ej. `(RF-PDX-05)`) para trazabilidad. |

## 3. Encabezado estándar de archivo

Aplicable por igual a `.c` y `.h`:

```c
/**
 * @file pokedex.c
 * @brief Implementación del módulo Pokédex: carga y consulta de las 150 especies.
 * @author <Nombre del estudiante>
 * @date 2026-09-01
 */
```

Reglas del bloque:

- `@file` usa el nombre exacto del archivo.
- `@brief` resume la responsabilidad única del módulo en una frase.
- `@author` se rellena con el nombre del estudiante en la entrega final.
- `@date` es la fecha de creación, no la de última modificación.

## 4. Encabezado estándar de función pública

Toda función pública (declarada en un `.h` e implementada en su `.c`) documenta
sus parámetros y su valor de retorno:

```c
/**
 * @brief Busca una especie por su número en la Pokédex.
 *
 * No modifica ningún dato de la Pokédex (RF-PDX-05).
 *
 * @param pd     Puntero a la Pokédex cargada (no debe ser NULL).
 * @param numero Número de especie a buscar, en el rango 1..150.
 * @return Puntero constante a la especie encontrada; NULL si no existe o
 *         los parámetros son inválidos.
 */
const Especie *pokedex_buscar_numero(const Pokedex *pd, int numero);
```

Reglas del bloque:

- `@param` lista cada parámetro en el mismo orden de la firma, con su significado
  y las restricciones (rangos, valores prohibidos, si admite NULL).
- `@return` describe todos los valores posibles, incluido el caso de error.
- Se puede añadir un párrafo libre tras `@brief` para precondiciones, efectos
  secundarios o referencias a requisitos.
- Las funciones sin parámetros omiten `@param`; las `void` omiten `@return`.

## 5. Funciones estáticas

Las funciones `static` llevan un comentario de una línea que explique su papel
dentro del módulo (no repiten la firma):

```c
/* Recorre la lista enlazada y libera cada ejemplar (uso interno del módulo). */
static void equipo_liberar_lista(Ejemplar *cabeza);
```

## 6. Estructuras, enums y constantes

- Las `typedef struct` se documentan con un comentario `/** @brief ... */` justo
  antes de la declaración; cada campo lleva un comentario de fin de línea
  explicando su invariante.
- Los `enum` documentan el significado de cada valor con comentario de fin de línea.
- Las constantes de dominio viven únicamente en `src/constantes.h`, con un
  comentario que cite el requisito que la origina.

## 7. Verificación

- `Doxyfile` define `OUTPUT_LANGUAGE = Spanish`, `INPUT_ENCODING = UTF-8` y
  `EXTRACT_ALL = YES` (DOC-01).
- Regeneración: `doxygen Doxyfile` en la raíz del proyecto; la salida HTML no se
  versiona.
- Revisión rápida manual del código fuente: el ancho de línea de los bloques
  Doxygen no supera 99 columnas y no hay acentos en identificadores C (sí los
  hay en el texto de cadenas y comentarios).
# Experimento: ¿qué hace `static` en C? (demostración con este proyecto)

**Fecha**: 2026-09-21
**Propósito**: material de estudio y de defensa oral. Demuestra con evidencia real
(compilador y `nm`) qué significa `static` en funciones dentro de C.

---

## 1. Concepto

`static` no tiene relación con punteros ni con "direcciones compartidas". Es una
etiqueta de **visibilidad y duración** con dos usos distintos según dónde aparezca:

| Dónde se usa | Significado | Efecto |
|---|---|---|
| Función o variable global | **Enlace interno** | Solo visible dentro de su archivo `.c`; no se exporta a los demás |
| Variable local (dentro de una función) | **Duración estática** | Vive todo el programa y conserva su valor entre llamadas; su alcance sigue siendo local |

Todas las funciones — static o no — tienen una dirección en el segmento de texto.
Lo que cambia con `static` es **la visibilidad del nombre** ante el enlazador
(linker), no su existencia.

---

## 2. Los tres experimentos

Se hicieron sobre una **copia sandbox** del proyecto en `/tmp` (el repositorio real
no se modifica). La función usada fue `mostrar_menu`, la única static de `main.c`
que dibuja el menú principal.

### Experimento A — otro archivo intenta llamarla

Se crea `espia.c`:

```c
void mostrar_menu(void);
void probar(void) { mostrar_menu(); }
```

y se compila todo junto:

```bash
gcc -std=c99 -Wall -Wextra -o demo_a src/*.c espia.c
```

Resultado real:

```
/usr/bin/ld: espia.c:(.text+0x5): referencia a `mostrar_menu' sin definir
exit=1
```

**Conclusión**: la función existe, tiene dirección y está compilada… pero su nombre
no se exporta fuera de `main.c`. El linker no la encuentra. Eso es enlace interno.

### Experimento B — sin `static`, la misma función es visible

Se elimina el `static` de `mostrar_menu` en `main.c` (un solo cambio) y se repite
la compilación:

```
exit=0
--- nm demo_b: ---
0000000000003977 T mostrar_menu
```

**Conclusión**: el único cambio fue el `static`. Con él: invisible. Sin él: visible
(símbolo `T`, global).

### Experimento C — dos funciones con el mismo nombre

**C1**: se restaura el `static` y se añade `menu2.c` con su **propia**
`mostrar_menu`. Compilación:

```
exit=0
--- nm demo_c1: DOS símbolos con el MISMO nombre, direcciones distintas ---
0000000000003977 t mostrar_menu     ← la de main.c (privada)
00000000000092f6 T mostrar_menu     ← la de menu2.c (pública)
```

**Dos funciones con el mismo nombre conviven en el mismo binario** porque la de
`main.c` es privada de su archivo.

**C2**: se quita el `static` (ambas globales) y se recompila:

```
menu2.c:(.text+0x0): definiciones múltiples de `mostrar_menu';
main.c:(.text+0x0): primero se definió aquí
exit=1
```

**Conclusión**: sin `static`, el linker no puede resolver el nombre duplicado.
`static` es lo que evita las colisiones de nombres entre módulos.

---

## 3. La lectura de `nm`

`nm binario` lista la tabla de símbolos. La diferencia está en la caja:

| Marca | Significado |
|---|---|
| `T` mayúscula | Símbolo **global** (enlace externo): visible para todos los archivos |
| `t` minúscula | Símbolo **local** (enlace interno = `static`): privado de su archivo |

En el binario de este proyecto:

```
0000000000003306 t bt_rec              ← static: recursión interna de equipo.c
0000000000003977 t mostrar_menu        ← static: privada de main.c
000000000000249e T combate_ejecutar    ← pública: declarada en combate.h
00000000000052a9 T pokedex_cargar      ← pública: declarada en pokedex.h
0000000000004d38 T main                ← pública: la llama el sistema operativo
```

Todas tienen dirección; la marca solo distingue quién puede llamarlas por nombre.

---

## 4. Regla práctica usada en el proyecto

- **Ayudantes internos de un módulo** → `static` (los 17 de `main.c`, los `bt_*`
  de `equipo.c`, los lectores internos de cada módulo).
- **API que otros módulos necesitan** → sin `static`, declarada en el `.h`
  (`pokedex_cargar`, `combate_ejecutar`, etc.).
- `main` **no puede** ser static: el runtime de C la llama desde fuera del archivo.

`static` es lo más cercano que C tiene a un método `private` de programación
orientada a objetos.

---

## 5. Guion de defensa (una frase)

> "`static` en una función le da enlace interno: la función es privada de su
> archivo y su nombre no se exporta a los demás módulos. Eso encapsula la
> implementación y evita colisiones de nombres; se comprueba con `nm`: los static
> aparecen como símbolos locales `t`, y si les quitas el static dos archivos con la
> misma función producen 'definiciones múltiples' en el linker."

---

## 6. Reproducción

El experimento completo se puede repetir con (desde la raíz del proyecto; usa una
copia para no tocar el repo):

```bash
rm -rf /tmp/demo-static && mkdir -p /tmp/demo-static
cp -r src data /tmp/demo-static/ && cd /tmp/demo-static
cp src/main.c src/main.c.orig

# A: llamar a la static desde otro archivo → referencia sin definir
printf 'void mostrar_menu(void);\nvoid probar(void) { mostrar_menu(); }\n' > espia.c
gcc -std=c99 -Wall -Wextra -o demo_a src/*.c espia.c   # falla: exit 1

# B: quitar static → compila y el símbolo pasa a ser global (T)
sed -i 's/^static void mostrar_menu/void mostrar_menu/' src/main.c
gcc -std=c99 -Wall -Wextra -o demo_b src/*.c espia.c   # exit 0
nm demo_b | grep -w mostrar_menu

# C: restaurar static + segundo archivo con el mismo nombre → conviven (t y T)
cp src/main.c.orig src/main.c
printf 'void mostrar_menu(void) { }\n' > menu2.c
gcc -std=c99 -Wall -Wextra -o demo_c1 src/*.c menu2.c  # exit 0
nm demo_c1 | grep -w mostrar_menu

# C2: quitar static → "definiciones múltiples"
sed -i 's/^static void mostrar_menu/void mostrar_menu/' src/main.c
gcc -std=c99 -Wall -Wextra -o demo_c2 src/*.c menu2.c  # falla: exit 1
```

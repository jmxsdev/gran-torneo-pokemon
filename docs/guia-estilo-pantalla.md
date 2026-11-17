# Guía de estilo de pantalla (UI/UX)

**Fecha**: 2026-11-16
**Decisión**: D19 del change `simulacion-torneo`
**Propósito**: unificar la presentación de las consultas por consola para
mejorar la experiencia de uso. Los **archivos de datos** (formato D8,
separador `;`) **NO cambian**: esta guía rige solo lo que el usuario ve en
pantalla.

---

## 1. Reglas

| # | Regla | Detalle |
|---|-------|---------|
| 1 | **Título de sección** | Regla `═` (ancho de la tabla), título centrado en MAYÚSCULAS, subtítulo opcional centrado con contexto, regla `═` |
| 2 | **Tablas** | Cabecera de columnas + regla `─` + filas + regla `─` + pie de resumen opcional |
| 3 | **Nombres con id** | `Nombre (id N)` en una sola columna (nunca `id;Nombre`) |
| 4 | **Estados en palabras** | `Victoria` / `Empate` / `Pendiente` (nunca `V1`/`E` en pantalla) |
| 5 | **Padding por caracteres** | El ancho de columna se mide en **caracteres visibles UTF-8** (las tildes cuentan 1 columna), nunca en bytes |
| 6 | **Archivos intactos** | `data/*.txt` conserva el formato D8 (`;`) para lectura por máquina |

---

## 2. Módulo de soporte: `src/pantalla.c/.h`

Procedimientos públicos (convención RF-CNV-02):

```c
int  pantalla_ancho(const char *texto);              /* columnas visibles (UTF-8) */
void pantalla_pad(const char *texto, int ancho);     /* texto + espacios hasta 'ancho' columnas */
void pantalla_centro(const char *texto, int ancho);  /* texto centrado en 'ancho' columnas */
void pantalla_linea(int ancho, const char *simbolo); /* regla de 'ancho' símbolos ("═" o "─") */
```

`pantalla_ancho` cuenta un carácter visible por cada byte **inicial** UTF-8
(ignora los bytes de continuación `0x80..0xBF`), de modo que `Pokémon` mide 7
columnas y no 8.

---

## 3. Ejemplos

### Historial de combates (opción 9)

```
════════════════════════════════════════════════════════════════════════════════════════════════════════
                                     HISTORIAL DE COMBATES
                              Resueltos: 20  ·  Pendientes: 44
════════════════════════════════════════════════════════════════════════════════════════════════════════
   #   Fase            Entrenador 1             Entrenador 2             Resultado    Ganador               KOs
────────────────────────────────────────────────────────────────────────────────────────────────────────────
   1   Grupo A         Ash (id 1)               Misty (id 2)             Victoria     Ash (id 1)            3-1
   2   Grupo A         Ash (id 1)               Brock (id 3)             Victoria     Brock (id 3)          2-2
   3   Grupo A         Ash (id 1)               Gary (id 4)              Empate       -                     2-2
 ...
  49   Octavos         Misty (id 2)             Clemont (id 17)          Pendiente    -                     0-0
  64   Final           (por definir)            (por definir)            Pendiente    -                     0-0
────────────────────────────────────────────────────────────────────────────────────────────────────────────
                                 Resumen: 20 resueltos · 44 pendientes
```

### Clasificación (opción 7) — mismo estilo

```
════════════════════════════════════════════════════════
                      CLASIFICACIÓN
════════════════════════════════════════════════════════
 GRUPO A
 Pos  Entrenador            V    E    D    Puntos   Derrotados
 ───────────────────────────────────────────────────────────
 1    Misty (id 2)          2    0    1    6        8
 2    Ash (id 1)            1    1    1    4        5
 3    Gary (id 4)           1    1    1    4        4
 4    Brock (id 3)          1    0    2    3        2
```

---

## 4. Despliegue

| Lote | Alcance |
|------|---------|
| **L3** | Opción 9 (historial de combates) — módulo piloto con este estilo |
| **L7** | Opciones 1 (Pokédex), 4 (entrenadores), 5 (equipos), 7 (clasificación), 8 (calendario), 10 (bracket) y 11 (campeón) — con regeneración de los goldens afectados |

Cada pantalla conserva su **mensaje de rechazo** actual (p. ej. "El torneo no
está armado.") sin terminar el programa (RF-TEC-03).

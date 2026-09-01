# Informe técnico — El Gran Torneo Pokémon

**Proyecto**: El Gran Torneo Pokémon — Sistema de administración de torneo (Primera Generación)
**Curso**: Programación I / Algoritmos I — Universidad de Carabobo
**Lenguaje**: C (ISO C99) · `gcc -std=c99 -Wall -Wextra` (cero warnings)
**Fecha de inicio**: 2026-09-01 · **Documento**: DOC-03 (spec `documentacion`)

> **Aviso de crecimiento incremental**: este informe se construye fase a fase.
> Esta versión (F0) contiene solo la semilla: portada, índice previsto y la
> sección de arquitectura con los diagramas del diseño. Cada fase posterior
> agrega su sección (decisiones D1–D10 justificadas, formatos de archivo,
> verificación y conclusiones) hasta el informe final de F10.

---

## Índice previsto

1. Introducción y objetivos
2. Arquitectura del sistema (diagramas de módulos y flujos)
3. Decisiones de diseño D1–D10 (justificación para la defensa oral)
4. Algoritmos (recursividad + backtracking de formación de equipos)
5. Formatos de los archivos de texto (`data/*.txt`)
6. Estrategia de verificación (batería scriptada)
7. Requisitos técnicos del enunciado (funciones, structs, TDA, archivos, cadenas)
8. Conclusiones y trabajo futuro

*(Las secciones 3 a 8 se completan en las fases F1–F10.)*

---

## 2. Arquitectura del sistema

El sistema se organiza en 10 módulos más `main`. `main` solo orquesta el menú y
el flujo (RF-TEC-02); cada módulo tiene una responsabilidad única y las
dependencias solo apuntan "hacia abajo" (sin ciclos entre cabeceras).

### 2.1 Mapa de módulos y dependencias

```mermaid
graph TD
    MAIN["main.c — menú y flujo (sin lógica de negocio)"]
    MAIN --> VAL["validacion.c — entradas seguras"]
    MAIN --> PDX["pokedex.c — 150 especies"]
    MAIN --> ENT["entrenador.c — registro/consulta"]
    MAIN --> EQP["equipo.c — ejemplares + backtracking"]
    MAIN --> CMB["combate.c — reglamento y daño"]
    MAIN --> TRN["torneo.c — grupos, bracket, campeón"]
    MAIN --> RES["resultados.c — carga y validación"]
    MAIN --> ARC["archivos.c — E/S de los 4 .txt"]
    EQP --> PDX
    EQP --> ENT
    CMB --> EQP
    CMB --> TIP["tipos.c — 18 tipos y efectividad"]
    TRN --> ENT
    RES --> TRN
    ARC --> PDX
    ARC --> ENT
    ARC --> TRN
    PDX --> TIP
```

### 2.2 Flujo de combate (D1, D3, D4, D5)

El combate 1 vs 1 es determinista: sin azar en ninguna ruta. El daño usa la
fórmula cerrada D1 con el multiplicador de tipos; el empate por límite de turnos
se resuelve distinto en grupos (D4) y en eliminatoria (D5).

```mermaid
flowchart TD
    A["Preparar: hp_actual = hp_max (ambos equipos)"] --> B["Selección inicial de cada entrenador"]
    B --> C{"¿Ambos con disponibles?"}
    C -- "no" --> Z["Fin: gana quien conserva Pokémon"]
    C -- "sí" --> D["Orden D3: mayor velocidad;\nempate → entrenador 1"]
    D --> E["Daño D1:\nbase = (2*nivel/5+2)*ataque/defensa + 2"]
    E --> F["Efectividad: producto tipo1×tipo2 del defensor\n(×0.25/×0.5/×1/×2/×4; 0 ⇒ daño 0)"]
    F --> G["Aplicar daño a hp_actual (piso 0)"]
    G --> H{"¿HP = 0?"}
    H -- "sí" --> I["KO: atacante suma derrotado;\ndefensor selecciona reemplazo"]
    I --> J{"¿Sin disponibles?"}
    J -- "sí" --> Z
    J -- "no" --> K{"¿20 turnos completos?"}
    H -- "no" --> K
    K -- "no" --> C
    K -- "sí" --> L{"¿Fase de grupos?"}
    L -- "sí" --> M["EMPATE (D4): 1 punto c/u"]
    L -- "no" --> N["D5: HP total → nivel total → entrenador 1"]
```

### 2.3 Backtracking de formación de equipos (RF-EQP-04)

La formación automática explora combinaciones de especies con podas tempranas
(cantidad, nivel total, tipos alcanzables) y propaga la primera solución.

```mermaid
flowchart TD
    A["bt_rec(i, restantes, nivel_acum, tipos...)"] --> B{"restantes == 0?"}
    B -- "sí" --> C{"tipos >= min_tipos Y ataque >= objetivo?"}
    C -- "sí" --> D["SOLUCIÓN: propagar true"]
    C -- "no" --> E["retroceder: false"]
    B -- "no" --> F{"i > 149 O poda cantidad\n(restantes > disponibles)?"}
    F -- "sí" --> E
    F -- "no" --> G{"poda nivel:\nacum + restantes*1 > max?"}
    G -- "sí" --> E
    G -- "no" --> H{"poda tipos:\ntipos + nuevos_max < min_tipos?"}
    H -- "sí" --> E
    H -- "no" --> I["Incluir especie i:\ncrear ejemplar (nivel mínimo factible)"]
    I --> J["bt_rec(i+1, restantes-1, ...)"]
    J -- "true" --> D
    J -- "false" --> K["liberar ejemplar creado"]
    K --> L["Excluir especie i:\nbt_rec(i+1, restantes, ...)"]
    L -- "true" --> D
    L -- "false" --> E
```

### 2.4 Torneo y eliminatorias (bracket 49–64)

Fase de grupos round-robin (combates 1–48) y eliminatorias con bracket de mapeo
fijo (49–64). Los participantes los resuelve el sistema, nunca el usuario
(RF-RES-03).

```mermaid
flowchart TD
    subgraph Octavos
        C49["49: 1A-2B"] ; C50["50: 1C-2D"] ; C51["51: 1E-2F"] ; C52["52: 1G-2H"]
        C53["53: 1B-2A"] ; C54["54: 1D-2C"] ; C55["55: 1F-2E"] ; C56["56: 1H-2G"]
    end
    subgraph Cuartos
        C57["57: G49-G50"] ; C58["58: G53-G54"] ; C59["59: G51-G52"] ; C60["60: G55-G56"]
    end
    subgraph Semifinal
        C61["61: G57-G58"] ; C62["62: G59-G60"]
    end
    C63["63: TERCER LUGAR — P61-P62"]
    C64["64: FINAL — G61-G62"]
    C49 --> C57 ; C50 --> C57
    C53 --> C58 ; C54 --> C58
    C51 --> C59 ; C52 --> C59
    C55 --> C60 ; C56 --> C60
    C57 --> C61 ; C58 --> C61
    C59 --> C62 ; C60 --> C62
    C61 -->|"P61"| C63 ; C62 -->|"P62"| C63
    C61 --> C64 ; C62 --> C64
    C63 --> T3["3.º = G63 · 4.º = P63"]
    C64 --> T1["Campeón = G64 · Subcampeón = P64"]
```

---

## 3. Decisiones de diseño (D1–D10)

*Se completa en F10* (tarea F10.1). Las decisiones quedan cerradas y justificadas
en `openspec/changes/gran-torneo-pokemon/design.md` §3 para la defensa oral.

**Decisiones pendientes del docente** (afectan a D7, D6 y D9; ver
`docs/planificacion.md` §4): rango de niveles 1–50 vs 1–100, equipo de torneo 3 vs 6
y confirmación del criterio adicional de desempate D9.
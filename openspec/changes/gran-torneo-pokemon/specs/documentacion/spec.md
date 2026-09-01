# Documentación Specification

## Purpose

Convención de documentación Doxygen, plan de control de planificación e informe técnico incremental con diagramas Mermaid.

## Requirements

### Requirement: Convención Doxygen (DOC-01)

El proyecto MUST incluir un `Doxyfile` y una guía de convenciones en `docs/` que fije los bloques `@file`, `@brief`, `@param`, `@return` y `@author` en español.

#### Scenario: Doxyfile presente

- GIVEN el repositorio
- WHEN se inspecciona la raíz
- THEN existe un `Doxyfile` configurado

#### Scenario: Guía de convenciones

- GIVEN la guía en `docs/`
- WHEN se revisa
- THEN documenta el uso de `@file`, `@brief`, `@param`, `@return` y `@author` en español

### Requirement: Plan de control de planificación (DOC-02)

El proyecto MUST mantener `docs/planificacion.md` con el seguimiento de lo hecho y lo pendiente, alineado al plan de 14 días, marcando cada ítem con estado (hecho/pendiente).

#### Scenario: Estados de planificación

- GIVEN `docs/planificacion.md`
- WHEN se revisa el estado de una fase
- THEN cada ítem indica claramente hecho o pendiente

### Requirement: Informe técnico incremental (DOC-03)

El proyecto MUST mantener `docs/informe-tecnico.md`, construido incrementalmente durante el desarrollo, documentando cada decisión abierta (D1–D10) con su justificación y diagramas Mermaid de los algoritmos.

#### Scenario: Diagrama Mermaid del bracket

- GIVEN el informe técnico
- WHEN se revisa la sección de eliminatorias
- THEN incluye un diagrama Mermaid del bracket 49–64

#### Scenario: Decisión documentada

- GIVEN la decisión D1 (fórmula de daño)
- WHEN se revisa el informe
- THEN aparece con su justificación para la defensa oral

### Requirement: Idioma de la documentación (DOC-04)

Toda la documentación del proyecto MUST estar en español neutro y profesional.

#### Scenario: Documentación en español

- GIVEN los documentos del proyecto
- WHEN se leen
- THEN están redactados en español neutro y profesional

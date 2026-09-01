# Menú Specification

## Purpose

Menú principal con las opciones del torneo y flujo general controlado por `main`.

## Requirements

### Requirement: Opciones del menú (RF-MEN-01)

El menú principal MUST ofrecer las opciones: 1. Consultar Pokédex, 2. Registrar entrenador, 3. Crear equipo, 4. Consultar entrenadores, 5. Consultar equipos, 6. Cargar resultados, 7. Consultar clasificación, 8. Consultar enfrentamientos, 9. Consultar historial de combates, 10. Mostrar resultados del torneo, 11. Mostrar campeón, 12. Salir. El menú MAY modificarse según la implementación.

#### Scenario: Navegación del menú

- GIVEN el sistema en ejecución
- WHEN se muestra el menú
- THEN presenta las 12 opciones y espera la selección del usuario

#### Scenario: Opción inexistente

- GIVEN el menú mostrado
- WHEN el usuario ingresa una opción inexistente
- THEN se informa del error
- AND se vuelve a mostrar el menú sin terminar el programa

### Requirement: `main` limitado a flujo y menú (RF-TEC-02)

La funcionalidad MUST estar separada en funciones y módulos; `main` controla solo el flujo general y el menú, sin concentrar toda la lógica.

#### Scenario: Separación de responsabilidades

- GIVEN el código del proyecto
- WHEN se inspecciona `main`
- THEN `main` solo orquesta el flujo
- AND delega la lógica en módulos

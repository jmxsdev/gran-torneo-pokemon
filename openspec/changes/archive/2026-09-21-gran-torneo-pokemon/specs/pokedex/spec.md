# Pokédex Specification

## Purpose

Carga, catálogo y consulta de las 150 especies de la Primera Generación y de los 18 tipos. La Pokédex es la única fuente de datos de especies y es inmutable durante el torneo.

## Requirements

### Requirement: Catálogo de 150 especies (RF-PDX-01)

La Pokédex MUST contener exclusivamente las 150 especies de la Primera Generación, del #001 Bulbasaur al #150 Mew.

#### Scenario: Carga completa de la Pokédex

- GIVEN el archivo `data/pokedex.txt` con 150 especies válidas
- WHEN el sistema inicia
- THEN carga las 150 especies
- AND el conteo de especies es exactamente 150

#### Scenario: Número fuera de rango

- GIVEN la Pokédex cargada
- WHEN el usuario consulta el número #000 o #151
- THEN el sistema informa que el Pokémon no existe
- AND no termina el programa

### Requirement: Atributos mínimos por especie (RF-PDX-02)

Cada entrada MUST incluir como mínimo: número, nombre, tipo primario, tipo secundario, HP base, ataque base, defensa base y velocidad base.

#### Scenario: Consulta completa de una especie

- GIVEN la Pokédex cargada
- WHEN el usuario consulta la información completa del #001
- THEN muestra número, nombre, tipos, HP, ataque, defensa y velocidad base

### Requirement: Catálogo de 18 tipos (RF-PDX-03)

El sistema MUST manejar los 18 tipos (Normal, Fuego, Agua, Planta, Eléctrico, Hielo, Lucha, Veneno, Tierra, Volador, Psíquico, Bicho, Roca, Fantasma, Dragón, Siniestro, Acero, Hada). Una especie MUST tener 1 o 2 tipos; el segundo tipo se representa vacío o `-` cuando es único.

#### Scenario: Especie con un solo tipo

- GIVEN la Pokédex cargada
- WHEN se consulta una especie con un solo tipo
- THEN el segundo tipo se muestra como `-`

#### Scenario: Especie con dos tipos

- GIVEN la Pokédex cargada
- WHEN se consulta Bulbasaur
- THEN muestra los tipos Planta y Veneno

### Requirement: Inmutabilidad de la Pokédex (RF-PDX-04)

Los datos de especie MUST permanecer inmutables: un combate modifica solo el HP actual del ejemplar, MUST NOT el HP base de la especie.

#### Scenario: Combate no altera la especie

- GIVEN un ejemplar de Bulbasaur con HP actual dañado en combate
- WHEN se consulta la especie Bulbasaur en la Pokédex
- THEN el HP base de la especie permanece sin cambios

### Requirement: Consultas de la Pokédex (RF-PDX-05)

La opción de consulta MUST permitir mostrar toda la Pokédex, buscar por número, buscar por nombre y consultar la información completa. La consulta MUST NOT modificar datos.

#### Scenario: Búsqueda por nombre existente

- GIVEN la Pokédex cargada
- WHEN el usuario busca por nombre "Mew"
- THEN muestra la información completa de Mew

#### Scenario: Búsqueda inexistente

- GIVEN la Pokédex cargada
- WHEN el usuario busca un nombre inexistente
- THEN informa que no existe
- AND no modifica ningún dato

### Requirement: Lectura desde archivo (RF-PDX-06)

Las 150 especies MUST leerse desde `data/pokedex.txt` al inicio; MUST NOT estar escritas en el código fuente.

#### Scenario: Archivo de Pokédex ausente

- GIVEN el archivo `data/pokedex.txt` no existe o no se puede abrir
- WHEN el sistema inicia
- THEN informa del error
- AND no termina abruptamente

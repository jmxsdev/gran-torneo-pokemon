# Makefile — El Gran Torneo Pokemon
# Compilacion estricta: gcc -std=c99 -Wall -Wextra, cero warnings como gate.
# El binario y los objetos se generan en build/ (fuera del arbol fuente).

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

# F11.1: genera dist/gran-torneo-pokemon-entrega.zip (paquete de entrega único).
entrega:
	bash scripts/empaquetar.sh
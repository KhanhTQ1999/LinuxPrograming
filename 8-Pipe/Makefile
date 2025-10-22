# Compiler
CC = gcc

#Directory where include all my library
LINKING_DIR = ../Library

# Build options
DEBUGFLAGS   = -O0 -D _DEBUG
RELEASEFLAGS = -O2 -D NDEBUG

# C Flags
FLAGS =  -Wl,-rpath=$(LINKING_DIR) -L$(LINKING_DIR) -lCmnBase

# Main source
MAIN_SRC = main.c
TARGET = main

all:
	$(CC) ${MAIN_SRC} $(FLAGS) $(DEBUGFLAGS) -o ${TARGET}
.PHONY: all

clean:
	rm -f $(TARGET)
.PHONY: clean
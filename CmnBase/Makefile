# Compiler
CC = gcc

#Directory where include all my library
LINKING_DIR = ../Library

# Library information
SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h)
OBJECTS = $(SOURCES:.c=.o)
TARGET = libCmnBase.so

# Build options
DEBUGFLAGS   = -O0 -D _DEBUG
RELEASEFLAGS = -O2 -D NDEBUG

# C Flags
FLAGS = -fPIC -DLOG_USE_COLOR

all: ${SOURCES}
	$(CC) ${SOURCES} $(FLAGS) $(DEBUGFLAGS) -shared -o $(TARGET)
	mv $(TARGET) $(LINKING_DIR)
.PHONY: all

clean:
	rm -f $(LINKING_DIR)/$(TARGET)
.PHONY: clean
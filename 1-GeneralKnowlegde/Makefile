# Compiler
CC = gcc

#Directory where include all my library
LINKING_DIR = ../Library

#Define strutils library's information
LIB_SOURCES = $(filter-out main.c, $(wildcard *.c))
LIB_HEADERS = $(wildcard *.h)
LIB_OBJECTS = $(LIB_SOURCES:.c=.o)
SHARED_LIB = libstrutils.so
STATIC_LIB = libstrutils.a

# Build options
DEBUGFLAGS   = -O0 -D _DEBUG
RELEASEFLAGS = -O2 -D NDEBUG

# C Flags
LIB_FLAGS = -fPIC
FLAGS = -Wl,-rpath=$(LINKING_DIR) -L$(LINKING_DIR) -lstrutils -lCmnBase

MAIN_SRC = main.c

all: shared static
.PHONY: all

#Build strutils as dynamic lib and move it to linking directory
$(SHARED_LIB):
	$(CC) ${LIB_SOURCES} $(LIB_FLAGS) $(DEBUGFLAGS) -shared -o $(SHARED_LIB)
	mv $(SHARED_LIB) $(LINKING_DIR)

#Build strutils as static lib and move it to linking directory
$(STATIC_LIB): $(LIB_OBJECTS)
	 ar rcs $@ $^
	 mv $(STATIC_LIB) $(LINKING_DIR)

shared: $(SHARED_LIB)
	$(CC) $(MAIN_SRC) $(FLAGS) $(DEBUGFLAGS) -o main_shared
.PHONY: shared

static: $(STATIC_LIB)
	$(CC) $(MAIN_SRC) $(FLAGS) $(DEBUGFLAGS) -o main_static
.PHONY: static

#Remove execute, obj file and strutils library in linking directory
clean:
	rm -f main_shared main_static $(LIB_OBJECTS) $(LINKING_DIR)/$(SHARED_LIB) $(LINKING_DIR)/$(STATIC_LIB)
.PHONY: clean
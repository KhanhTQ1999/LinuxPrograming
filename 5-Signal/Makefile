# Compiler
CC = gcc

#Directory where include all my library
LINKING_DIR = ../Library

# Build options
DEBUGFLAGS   = -O0 -D _DEBUG
RELEASEFLAGS = -O2 -D NDEBUG

# C Flags
FLAGS = -Wl,-rpath=$(LINKING_DIR) -L$(LINKING_DIR) -lCmnBase -lpthread

# Main source
TASK1_SRC = task1.c
TARGET1 = task1

TASK2_SRC = task2.c
TARGET2 = task2

TASK3_SRC = task3.c
TARGET3 = task3

TASK4_SRC = task4.c
TARGET4 = task4

all: task1 task2 task3 task4
.PHONY: all

task1: $(TASK1_SRC)
	$(CC) ${TASK1_SRC} $(FLAGS) $(DEBUGFLAGS) -o $(TARGET1)
.PHONY: task1

task2: $(TASK2_SRC)
	$(CC) ${TASK2_SRC} $(FLAGS) $(DEBUGFLAGS) -o $(TARGET2)
.PHONY: task2

task3: $(TASK3_SRC)
	$(CC) ${TASK3_SRC} $(FLAGS) $(DEBUGFLAGS) -o $(TARGET3)
.PHONY: task3

task4: $(TASK4_SRC)
	$(CC) ${TASK4_SRC} $(FLAGS) $(DEBUGFLAGS) -o $(TARGET4)
.PHONY: task4

clean:
	rm -f $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4)
.PHONY: clean
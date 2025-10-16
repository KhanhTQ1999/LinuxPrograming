# Name of the output executables
WORKER = worker
DISPATCHER = dispatcher

# Compiler and compilation flags
CC = gcc

# Build options
DEBUGFLAGS   = -O0 -D _DEBUG
RELEASEFLAGS = -O2 -D NDEBUG

#Directory where include all my library
LINKING_DIR = ../Library

# C Flags
CFLAGS = -Wl,-rpath=$(LINKING_DIR) -L$(LINKING_DIR) -lCmnBase -lstrutils -lpthread -lrt $(DEBUGFLAGS)

# Source files for worker and dispatcher
WORKER_SRC = worker.c
DISPATCHER_SRC = dispatcher.c

# Default target: build both worker and dispatcher
all: $(WORKER) $(DISPATCHER)

# Build the worker executable from worker.c
$(WORKER): $(WORKER_SRC)
	$(CC) $(WORKER_SRC) $(CFLAGS) -o $(WORKER)

# Build the dispatcher executable from dispatcher.c
$(DISPATCHER): $(DISPATCHER_SRC)
	$(CC) $(DISPATCHER_SRC) $(CFLAGS) -o $(DISPATCHER)

# Clean target: remove the compiled executables
clean:
	rm -f $(WORKER) $(DISPATCHER)
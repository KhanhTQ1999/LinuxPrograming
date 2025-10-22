# Name of the output executables
PRODUCER = producer
CONSUMER = consumer

# Compiler and compilation flags
CC = gcc

# Build options
DEBUGFLAGS   = -O0 -D _DEBUG
RELEASEFLAGS = -O2 -D NDEBUG

#Directory where include all my library
LINKING_DIR = ../Library

# C Flags
CFLAGS = -Wl,-rpath=$(LINKING_DIR) -L$(LINKING_DIR) -lCmnBase -lstrutils -lpthread -lrt $(DEBUGFLAGS)

# Source files for producer and consumer
PRODUCER_SRC = posix_shm_producer.c
CONSUMER_SRC = posix_shm_consumer.c

# Default target: build both producer and consumer
all: $(PRODUCER) $(CONSUMER)

# Build the producer executable from posix_shm_producer.c
$(PRODUCER): $(PRODUCER_SRC)
	$(CC) $(PRODUCER_SRC) $(CFLAGS) -o $(PRODUCER)

# Build the consumer executable from posix_shm_consumer
$(CONSUMER): $(CONSUMER_SRC)
	$(CC) $(CONSUMER_SRC) $(CFLAGS) -o $(CONSUMER)

# Clean target: remove the compiled executables
clean:
	rm -f $(PRODUCER) $(CONSUMER)
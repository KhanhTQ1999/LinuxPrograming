# Name of the output executables
SERVER = server
CLIENT = client

# Compiler and compilation flags
CC = gcc
CFLAGS = -Wall -Wextra -O2

# Source files for server and client
SERVER_SRC = server.c
CLIENT_SRC = client.c

# Default target: build both server and client
all: $(SERVER) $(CLIENT)

# Build the server executable from server.c
$(SERVER): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $(SERVER) $(SERVER_SRC)

# Build the client executable from client.c
$(CLIENT): $(CLIENT_SRC)
	$(CC) $(CFLAGS) -o $(CLIENT) $(CLIENT_SRC)

# Clean target: remove the compiled executables
clean:
	rm -f $(SERVER) $(CLIENT)
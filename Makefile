CC = gcc
CFLAGS = -Wall -Wextra -pthread -Iinclude

SERVER_SRC = \
	src/server/server.c \
	src/server/connection.c \
	src/server/network_utils.c \
	src/server/thread_manager.c \
	shared/logger.c

SERVER_BIN = server

all: $(SERVER_BIN)

$(SERVER_BIN): $(SERVER_SRC)
	$(CC) $(CFLAGS) $(SERVER_SRC) -o $(SERVER_BIN)

clean:
	rm -f $(SERVER_BIN)
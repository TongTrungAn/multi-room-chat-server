CC = gcc
CFLAGS = -Wall -Wextra -pthread -Iinclude

SERVER_SRC = \
	src/server/server.c \
	src/server/connection.c \
	src/server/network_utils.c \
	src/server/thread_manager.c \
	shared/logger.c

SERVER_BIN = server

CLIENT_SRC = \
	src/client/client.c \
	src/client/parser.c

CLIENT_BIN = client

all: $(SERVER_BIN) $(CLIENT_BIN)

$(SERVER_BIN): $(SERVER_SRC)
	$(CC) $(CFLAGS) $(SERVER_SRC) -o $(SERVER_BIN)

$(CLIENT_BIN): $(CLIENT_SRC)
	$(CC) $(CFLAGS) $(CLIENT_SRC) -o $(CLIENT_BIN)

clean:
	rm -f $(SERVER_BIN) $(CLIENT_BIN)
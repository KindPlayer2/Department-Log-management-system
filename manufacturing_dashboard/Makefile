CC = gcc
CFLAGS = -Wall -Wextra -std=c99
SRC = src/daemon.c src/logger.c src/utils.c
INC = -Iinclude
BIN = bin/daemon

all:
	$(CC) $(CFLAGS) $(INC) $(SRC) -o $(BIN)

install:
	cp $(BIN) /usr/local/bin/
	cp scripts/daemon_init.sh /etc/init.d/manufacturing_daemon
	chmod +x /etc/init.d/manufacturing_daemon

clean:
	rm -f $(BIN) *.o


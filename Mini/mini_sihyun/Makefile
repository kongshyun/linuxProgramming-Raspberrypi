CC = gcc

all: server client

server: server_daemon.c
	$(CC) -o server_daemon server_daemon.c

client: client_.c
	$(CC) -o client_ client_.c

run_server: server
	./server_

run_client: client
	./client_ 127.0.0.1

clean:
	rm -f server_daemon_ client_


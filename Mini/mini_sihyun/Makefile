CC = gcc

all: server client

server: server_.c
	$(CC) -o server_ server_.c

client: client_.c
	$(CC) -o client_ client_.c

run_server: server
	./server_

run_client: client
	./client_ 127.0.0.1

clean:
	rm -f server_ client_


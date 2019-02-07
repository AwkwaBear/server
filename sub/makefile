all: client server

client: client.o
	gcc client.c -o client367

server: server.o
	gcc server.o -o server367

clean:
	rm -f *.o

real_clean: real_clean
	rm -f client server

all: clean build 

build:
	gcc -g -o client client.c
	gcc -g -o server server.c

clean:
	-rm client
	-rm server

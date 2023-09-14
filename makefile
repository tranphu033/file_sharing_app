CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 

all: client server

client: tcp_client.c
	${CC} tcp_client.c -o client $(shell pkg-config --cflags --libs gtk4 gmodule-2.0) 

server: tcp_server.o
	${CC} tcp_server.o -o server $(shell mysql_config --cflags --libs)

tcp_server.o: tcp_server.c
	${CC} ${CFLAGS} tcp_server.c

clean:
	rm -f *.o *~

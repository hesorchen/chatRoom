VPATH:=src inc

.PHONY:all
all:server client
server:server.c Read.h Write.h
	gcc -o ./exe/server $< -L/usr/local/lib/ -lhiredis
client:client.c Read.h Write.h
	gcc -o ./exe/client $< -L/usr/local/lib/ -lhiredis


startup:
	@./server
game:
	@./player
compile: player server
server: server.o handshake.o
	@gcc -o server server.o handshake.o
player: player.o handshake.o
	@gcc -o player player.o handshake.o
player.o: player.c handshake.h 
	@gcc -c player.c
server.o: server.c handshake.h
	@gcc -c server.c
handshake.o: handshake.c handshake.h 
	@gcc -c handshake.c
clean:
	@rm *.o
	@rm player
	@rm server

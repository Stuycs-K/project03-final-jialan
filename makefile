startup:
	@./server
game:
	@./player
compile: player server
server: server.o
	@gcc -o server server.o
player: player.o 
	@gcc -o player player.o
player.o: player.c
	@gcc -c player.c
server.o: server.c
	@gcc -c server.c
clean:
	@rm *.o
	@rm player
	@rm server

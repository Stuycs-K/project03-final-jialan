#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

#define WKP "Lobby"
#define MAX_PLAYERS 5

//basic version
struct players {
  char* ID;
  int wins;
}

char** list_players() {
  char** players;
  players = (char**)malloc(sizeof(char*)*5); //max 5 players
  for (int i=0; i<MAX_PLAYERS;i++) {
    players[i] = (char*)malloc(sizeof(char));
  }
  return players;
}

//create a lobby
int create_lobby() {
  int lobby_read;

  printf("Created lobby.\n");
  if (mkfifo(WKP,0644)==-1) {
    perror("WKP mkfifo error");
    exit(1);
  }
  lobby_read = open(WKP, O_RDONLY);
  remove(WKP);

  return lobby_read;
}

//struct players *p

int lobby() {
  char client_name[256];
  int lobby_read = create_lobby();

  int r = read(lobby_read, client_name, sizeof(client_name));
  printf("Player %s is available...\n", client_name);
  //add players

}

int player_connect(int *to_lobby) {
  char pipe_name[256];
  sprintf(pipe_name, "%d", getpid());
  if (mkfifo(pipe_name, 0644)==-1) {
    perror("private pipe mkfifo error");
    exit(1);
  }
  *to_lobby = open(WKP, O_WRONLY);
  int w = write(*to_lobby, pipe_name, sizeof(pipe_name));
  printf("Player %s: Conneting to lobby...\n", pipe_name);

}

int player_action() {
  char buffer[256];
  printf("Enter...\n");
  fgets(buffer, 255, stdin);
  char* action;
  char* name; // player id
  sscanf(buffer, "%[0-9] %[^\n]", action, name);
  printf("Attempting to %s to %s...\n", action, name);
  if (strcmp(action, "match")==0) {
    request_match(name);
    //open name pipe?
  }
}

// waiting for a player to exist


// can send request for a match
// open the new pipe, and start game
// communication between the players
// best of three

//tournament (even num)
//randomly matched (tell each person)

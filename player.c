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


int player_match() {
  int to_server;
  int from_server;
  from_server = client_handshake( &to_server );
  char buffer[30];
  //server sends player the private pipe which they will communicate with other player
  //for now just the server
  for (int i=0; i<3; i++) {
    read(from_server, buffer, sizeof(buffer));
    printf("Server sent: %s", buffer);
    fgets(buffer, 30, stdin);
    write(to_server, buffer, sizeof(buffer));
  }
  // char pipe_name[256];
  // sprintf(pipe_name, "%d", getpid());
  // if (mkfifo(pipe_name, 0644)==-1) {
  //   perror("private pipe mkfifo error");
  //   exit(1);
  // }
  // *to_lobby = open(WKP, O_WRONLY);
  // int w = write(*to_lobby, pipe_name, sizeof(pipe_name));
  // printf("Player %s: Conneting to lobby...\n", pipe_name);

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

//player and server connect
//server creates WKP for all players to go onto-- basically the forking lab
//player confirm connection to server

// can send request for a match
// open the new pipe, and start game
// communication between the players
// best of three

//tournament (even num)
//randomly matched (tell each person)

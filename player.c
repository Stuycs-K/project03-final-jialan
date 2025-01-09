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

int player_match() {
  int to_server;
  int from_server;
  int sent = 0;
  char buffer[30];
  
  from_server = client_handshake( &to_server );

  fd_set read_fds;
  FD_ZERO(&read_fds);
  FD_SET(to_server, &read_fds);
  FD_SET(STDIN_FILENO, &read_fds);

  //stdin player 1 --> server
  //stdin player 2 --> server
  
  
  if (FD_ISSET(STDIN_FILENO, &read_fds)) {
    fgets(buffer, 30, stdin);
    write(to_server, buffer, sizeof(buffer));
    printf("Your choice: %s", buffer);
    sent=1;
  }

  if (FD_ISSET(from_server, &read_fds)) {
    read(from_server, buffer, sizeof(buffer));
    //only print when made choice or time up
    if (sent) {
      printf("Opponent choice: %s", buffer);
    }
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

// char** list_players() {
//   char** players;
//   players = (char**)malloc(sizeof(char*)*5); //max 5 players
//   for (int i=0; i<MAX_PLAYERS;i++) {
//     players[i] = (char*)malloc(sizeof(char));
//   }
//   return players;
// }


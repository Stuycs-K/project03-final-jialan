#include "handshake.h"

//player_match
int main() {
  int to_server;
  int from_server;
  int sent = 0;
  char buffer[30];
  
  from_server = client_handshake( &to_server );

  return 0
  // fd_set read_fds;
  // FD_ZERO(&read_fds);
  // FD_SET(to_server, &read_fds);
  // FD_SET(STDIN_FILENO, &read_fds);

  // //stdin player 1 --> server
  // //stdin player 2 --> server
  
  
  // if (FD_ISSET(STDIN_FILENO, &read_fds)) {
  //   fgets(buffer, 30, stdin);
  //   write(to_server, buffer, sizeof(buffer));
  //   printf("Your choice: %s", buffer);
  //   sent=1;
  // }

  // if (FD_ISSET(from_server, &read_fds)) {
  //   read(from_server, buffer, sizeof(buffer));
  //   //only print when made choice or time up
  //   if (sent) {
  //     printf("Opponent choice: %s", buffer);
  //   }
  // }

}

// int player_action() {
//   char buffer[256];
//   printf("Enter...\n");
//   fgets(buffer, 255, stdin);
//   char* action;
//   char* name; // player id
//   sscanf(buffer, "%[0-9] %[^\n]", action, name);
//   printf("Attempting to %s to %s...\n", action, name);
//   if (strcmp(action, "match")==0) {
//     request_match(name);
//     //open name pipe?
//   }
// }


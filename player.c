#include "handshake.h"

int main() {
  int from_server;
  int to_server;
  from_server = client_handshake( &to_server );

  char buffer[256];
  printf("Enter...\n");
  fgets(buffer, 255, stdin);
  char* action;
  char* pipe_name; // player id
  //sscanf(buffer, "%[0-9] %[^\n]", action, pipe_name);
  printf("Attempt to match with player %s", buffer);
  //printf("Attempting to %s to %s...\n", action, pipe_name);
  //if (strcmp(action, "match")==0) {
    //write(to_server, pipe_name, sizeof(pipe_name));
    write(to_server, buffer, sizeof(buffer));
    printf("Sent request to player %s", buffer);
  //}

}

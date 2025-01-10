#include "handshake.h"

int main() {
  int to_client;
  int from_client;

  from_client = server_handshake( &to_client );
  return 0;
  // char client_name[256];
  // int lobby_read = create_lobby();

  // int r = read(lobby_read, client_name, sizeof(client_name));
  // printf("Player %s is available...\n", client_name);
  //add players

}

int lobby() {
  char client_name[256];
  int lobby_read = create_lobby();

  int r = read(lobby_read, client_name, sizeof(client_name));
  printf("Player %s is available...\n", client_name);
  //add players

}
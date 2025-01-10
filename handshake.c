#include "handshake.h"

int server_setup() {
  int from_client = 0;
  printf("Starting up game...\n");
  if (mkfifo(WKP, 0644)==-1) {
    perror("WKP mkfifo error");
    exit(1);
  }
  from_client = open(WKP, O_RDONLY);
  remove(WKP);
  return from_client;
}

int server_handshake(int* to_client) {
  char client_name[256];
  int from_client = server_setup();
  int r = read(from_client, client_name, sizeof(client_name));
  if (r==-1) {
    perror("Error server reading from WKP");
    exit(1);
  }
  *to_client = open(client_name, O_WRONLY);
  int rand_num = 3417;
  int w = write(*to_client, &rand_num, sizeof(int));
  if (w==-1) {
    perror("error server writing to private pipe");
    exit(1);
  }
  int received_num;
  r = read(from_client, &received_num, sizeof(int));
  if (received_num==rand_num+1) {
    printf("Successfully booted up\n");
  }
  return from_client;
}

int client_handshake(int* to_server) {
  char pipe_name[256];
  sprintf(pipe_name, "%d", getpid());
  if (mkfifo(pipe_name, 0644)==-1) {
    perror("private pipe mkfifo");
    exit(1);
  }
  *to_server = open(WKP, O_WRONLY);

  int w = write(*to_server, pipe_name, sizeof(pipe_name));
  if (w==-1) {
    perror("error client writing to WKP");
    exit(1);
  }
  int from_server;
  int rand_num;
  from_server = open(pipe_name, O_RDONLY);
  int r = read(from_server, &rand_num, sizeof(int));
  if (r==-1) {
    perror("error client reading from private pipe");
    exit(1);
  }
  remove(pipe_name);
  int ack = rand_num+1;
  w = write(*to_server, &ack, sizeof(int));
  if (w==-1) {
    perror("error client writing back to server");
    exit(1);
  }
  printf("Successfully connected to the lobby...\n");
  close(from_server);
  return from_server;
}

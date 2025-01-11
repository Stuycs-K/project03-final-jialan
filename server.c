#include "handshake.h"

#define MAX_CLIENTS 2
#define BUFFER_SIZE 10

int main() {
    int to_client;
    int from_client;
    char buffer[BUFFER_SIZE];

    from_client = server_handshake( &to_client );

    fd_set read_fds;
    int client_fds[MAX_CLIENTS];
    int listen_socket = from_client;
    int client_count=0;

	while (1) {
    //add WKP to set
	FD_ZERO(&read_fds);
	FD_SET(listen_socket, &read_fds);
    if (select(listen_socket+1, &read_fds, NULL, NULL, NULL)==-1) {
    	perror("select error");
    	exit(1);
    }

    //add clients to set
    // for (int i=0; i<MAX_CLIENTS; i++)  {
      // FD_SET(client_fds[i], &read_fds);
    // }

    //Server listens for new clients (connection) on WKP
    printf("Waiting for connections...\n");
    int pid;
    if (FD_ISSET(listen_socket, &read_fds)) {
      char pipe_name[BUFFER_SIZE];
      read(listen_socket, &pid, sizeof(pid));
      //printf("Player %d connected to lobby...\n", pid);
      sprintf(pipe_name, "%d", pid);
  	//add to fd to listen for
  	client_fds[client_count] = open(pipe_name, O_RDONLY); //nonblock?
  	FD_SET(client_fds[client_count], &read_fds);
    client_count++;
    }

    for (int i=0; i<MAX_CLIENTS;i++) {
   	if (FD_ISSET(client_fds[i], &read_fds)) {
   		read(client_fds[i], buffer, sizeof(buffer));
   		printf("Player choice: %s\n", buffer);
   		}
    }
   }
   return 0;
}

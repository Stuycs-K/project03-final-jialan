#include "handshake.h"

#define MAX_CLIENTS 2
#define BUFFER_SIZE 10

//player_match
int main() {
  int to_server;
  int from_server;
  char buffer[BUFFER_SIZE];
  
  //from_server = client_handshake( &to_server );

  
  fd_set read_fds;

  int client_fds[MAX_CLIENTS];

  int listen_socket;

  listen_socket = server_setup();


  //FD_SET(STDIN_FILENO, &read_fds);

  if (select(listen_socket+1, &read_fds, NULL, NULL, NULL)==-1) {
  	perror("select error");
  	exit(1);
  }

  //Server listens for new clients (connection)
  int pid;
  if (FD_ISSET(listen_socket, &read_fds)) {
  	char pipe_name[BUFFER_SIZE];
    read(from_server, &pid, sizeof(pid));
    printf("Player %d connected to lobby...\n", pid);
    sprintf(pipe_name, "%d", pid);
	//add to fd to listen for
	client_fds[client_count] = open(pipe_name, O_RDONLY); //nonblock?
  }

  while (1) {
  	FD_ZERO(&read_fds);
  	FD_SET(listen_socket, &read_fds);

	//setting all the clients
  	for (int i=0; i<2; i++)  {
    	FD_SET(client_fds[i], &read_fds);
  	}
  	if (FD_ISSET(client_fds[i], &read_fds)) {
  		read(client_fds[i], buffer, sizeof(buffer));
  		printf("Player choice: %s\n", buffer);
  	}
  	
  }  

}

// int player_action() {
// char buffer[256];
// printf("Enter...\n");
// fgets(buffer, 255, stdin);
// char* action;
// char* name; // player id
// sscanf(buffer, "%[0-9] %[^\n]", action, name);
// printf("Attempting to %s to %s...\n", action, name);
// if (strcmp(action, "match")==0) {
// request_match(name);
// //open name pipe?
// }
// }


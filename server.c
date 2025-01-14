#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include "handshake.h"

#define WKP "lobby"
#define BUFFER_SIZE 256
#define MAX_CLIENTS 2

static void sighandler(int signo) {
  if (signo == SIGINT) {
    remove(WKP);
    kill(getpid(), SIGTERM);
  }
}

int main() {
	signal(SIGINT, sighandler);
    int wkp_fd;
    int client_fds[MAX_CLIENTS];
    int client_names[MAX_CLIENTS];
    int client_count = 0;
    char buffer[BUFFER_SIZE];
    char client_pipe_name[BUFFER_SIZE];
    char* actions[MAX_CLIENTS];
    fd_set read_fds;

    // server setup
    wkp_fd = server_setup();

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(wkp_fd, &read_fds);
        int max_fd = wkp_fd;

        // add clients to set
        for (int i = 0; i < client_count; i++) {
            FD_SET(client_fds[i], &read_fds);
            if (client_fds[i] > max_fd) {
                max_fd = client_fds[i];
            }
        }

        int activity = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
        if (activity < 0) {
            perror("select error");
            break;
        }

        // check if WKP has a new connection
        if (FD_ISSET(wkp_fd, &read_fds)) {
        	int client_pid;
            int bytes_read = read(wkp_fd, &client_pid, sizeof(client_pid));
            sprintf(client_pipe_name, "%d", client_pid);

            if (bytes_read > 0) {
                printf("New client connected: %s\n", client_pipe_name);
                if (client_count < MAX_CLIENTS) {
                    int client_fd = open(client_pipe_name, O_RDONLY);
                    client_fds[client_count] = client_fd;
                    client_names[client_count] = client_pid;
                    client_count++;
                } else {
                    printf("Max clients reached. Cannot accept %s\n", client_pipe_name);
                    break;
                }
            }
        }

        // check if any client FDs have data to read
        int response = 0;
        for (int i = 0; i < client_count; i++) {
            if (FD_ISSET(client_fds[i], &read_fds)) {
                int bytes_read = read(client_fds[i], buffer, sizeof(buffer) - 1);
                if (bytes_read > 0) {
                    buffer[bytes_read] = '\0';
                    printf("Received from client %d: %s\n", client_names[i], buffer);
                    close(client_fds[i]);

                    if (response>2) {
                      strcpy(actions[i],buffer);
                      printf("%s in action\n", actions[i]);
                      response++;
                    }
                    else {
                      //write back to client
                        sprintf(client_pipe_name, "%d", client_names[i]);
                        sprintf(buffer, "Player %s chose %s", client_pipe_name, actions[i]);

                        int to_client = open(client_pipe_name, O_WRONLY);
                        write(to_client, buffer, sizeof(buffer));
                    }


                 }
                // else if (bytes_read == 0) {
                    // printf("Client %d disconnected\n", client_names[i]);
                    // close(client_fds[i]);
                    // client_fds[i] = client_fds[--client_count];
                    // i--;
                // }
            }
        }
    }
    return 0;
}

int game_logic() {
  //read from player 1 and 2

}

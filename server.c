#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>

#define WKP "lobby"
#define BUFFER_SIZE 256
#define MAX_CLIENTS 2

struct clients {
	char pipe_name[BUFFER_SIZE];
	int client_fd;
}

struct clients * init() {
	struct clients * client_list = (struct clients*)malloc(MAX_CLIENTS*sizeof(struct clients));
	for (int i=0; i<MAX_CLIENTS;i++) {
		*(client_list + i) = NULL;
	}
	return client_list;
}

int main() {
    int wkp_fd, client_fds[MAX_CLIENTS] = {0};
    int client_count = 0;
    fd_set read_fds;
    char buffer[BUFFER_SIZE];

    // Create and open the WKP
    mkfifo(WKP, 0644);
    wkp_fd = open(WKP, O_RDONLY);
    if (wkp_fd==-1) {
        perror("Failed to open WKP");
        exit(1);
    }
    printf("Server started. Waiting for connections...\n");

    while (1) {
        FD_ZERO(&read_fds); //init set
        FD_SET(wkp_fd, &read_fds); // add WKP to set
        int max_fd = wkp_fd;

		// add clients to set
        for (int i = 0; i < client_count; i++) {
            FD_SET(client_fds[i], &read_fds);
            if (client_fds[i] > max_fd) {
                max_fd = client_fds[i];
            }
        }

        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL)==-1) {
            perror("select error");
            exit(1);
        }

        // check if WKP has a new connection
        struct clients* client_list = init();
        if (FD_ISSET(wkp_fd, &read_fds)) {
            char client_pipe_name[BUFFER_SIZE];
            int bytes_read = read(wkp_fd, client_pipe_name, sizeof(client_pipe_name));
            printf("New client connected: %s\n", client_pipe_name);
            if (client_count < MAX_CLIENTS) {
                int client_fd = open(client_pipe_name, O_RDONLY);
                client_fds[client_count++] = client_fd;
                strncpy(*(client_list + client_count)->pipe_name, client_pipe_name, 10);
                *(client_list + client_count)->client_fd = client_count;
                } 
            else {
                printf("Max clients reached. Cannot accept %s\n", client_pipe_name);
            }
        }

        // check if any client FDs have data to read
        for (int i = 0; i < client_count; i++) {
            if (FD_ISSET(client_fds[i], &read_fds)) {
                int bytes_read = read(client_fds[i], buffer, sizeof(buffer) - 1);
                if (bytes_read > 0) {
                    buffer[bytes_read] = '\0';
                    printf("Received from client %d: %s\n", i + 1, buffer);
                } else if (bytes_read == 0) {
                    printf("Client %d disconnected\n", i + 1);
                    close(client_fds[i]);
                    client_fds[i] = client_fds[--client_count];
                    i--;
                }
            }
        }
    }

    close(wkp_fd);
    remove(WKP);
    return 0;
}

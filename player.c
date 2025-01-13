#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define WKP "lobby"
#define BUFFER_SIZE 256

int main() {
	char pipe_name[BUFFER_SIZE];
	int pid = getpid();
    sprintf(pipe_name, "%d", pid);
    
    mkfifo(pipe_name, 0644);

    int wkp_fd = open(WKP, O_WRONLY);
    if (wkp_fd < 0) {
        perror("Failed to connect to WKP");
        exit(1);
    }

    write(wkp_fd, &pid, sizeof(pid));
    close(wkp_fd);

    // Send messages to the server
    int client_fd = open(pipe_name, O_WRONLY);
    if (client_fd < 0) {
        perror("Failed to open client pipe");
        exit(1);
    }

    char buffer[BUFFER_SIZE];
    printf("Enter...\n");
    fgets(buffer, BUFFER_SIZE, stdin);
    printf("Attempt to match with player %s", buffer);
    write(client_fd, buffer, sizeof(buffer));
    printf("Sent request to player %s", buffer);

    close(client_fd);
    remove(pipe_name);  // Clean up
    return 0;
}

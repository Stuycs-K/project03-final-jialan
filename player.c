#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

#define WKP "lobby"
#define BUFFER_SIZE 256

char pipe_name[BUFFER_SIZE];

static void sighandler(int signo) {
  if (signo == SIGINT) {
  	//sprintf(pipe_name, "%d", getpid());
    remove(pipe_name);
    kill(getpid(), SIGTERM);
  }
}

int main() {
    //creating private pipe
    signal(SIGINT, sighandler);
    sprintf(pipe_name, "%d", getpid());
    mkfifo(pipe_name, 0644);

    //connecting to server
    int wkp_fd = open(WKP, O_WRONLY);
    if (wkp_fd < 0) {
        perror("Failed to connect to server (WKP)");
        exit(1);
    }
    write(wkp_fd, pipe_name, sizeof(pipe_name));
    close(wkp_fd);

    //writing to server
    int client_fd = open(pipe_name, O_WRONLY);
    if (client_fd < 0) {
        perror("Failed to open client pipe");
        exit(1);
    }
    char buffer[BUFFER_SIZE];
    printf("Enter: ");
    fgets(buffer, BUFFER_SIZE, stdin);
    write(client_fd, buffer, sizeof(buffer));
    printf("Sent %s to player\n", buffer);
    close(client_fd);

    //reading from server
    printf("Waiting on opponent...\n");
    char buffer2[BUFFER_SIZE];
    sleep(1);
    int from_server = open(pipe_name, O_RDONLY);
    read(from_server, buffer2, sizeof(buffer2));
    printf("Received from server: %s\n", buffer2);
    
	close(from_server);
    remove(pipe_name);
    return 0;
}

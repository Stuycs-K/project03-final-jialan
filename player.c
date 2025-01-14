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

static void sighandler(int signo) {
  if (signo == SIGINT) {
    char pipe_name[BUFFER_SIZE];
    sprintf(pipe_name, "%d", getpid());
    remove(pipe_name);
    kill(getpid(), SIGTERM);
  }
}

int main() {
	signal(SIGINT, sighandler);
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

    int client_fd = open(pipe_name, O_WRONLY);
    if (client_fd < 0) {
        perror("Failed to open client pipe");
        exit(1);
    }

    char buffer[BUFFER_SIZE];
    printf("Enter...\n");
    fgets(buffer, BUFFER_SIZE, stdin);
    //printf("Attempt to match with player %s", buffer);
    write(client_fd, buffer, sizeof(buffer));
    printf("Sent %s to player", buffer);

    close(client_fd);

	//reading from server
	int from_server = open(pipe_name, O_RDONLY);
	read(from_server, buffer, sizeof(buffer));
	printf("Received from server: %s", buffer);
	
    
    remove(pipe_name);
    return 0;
}

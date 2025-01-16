#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

#define WKP "lobby"
#define BUFFER_SIZE 50
#define PIPE_SIZE 10

char pipe_name[PIPE_SIZE];
char pipe_name2[PIPE_SIZE];

static void sighandler(int signo) {
  if (signo == SIGINT) {
    remove(pipe_name);
    remove(pipe_name2);
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
    // pipe_name[strlen(pipe_name)] = '\0';
    write(wkp_fd, pipe_name, sizeof(pipe_name));
    close(wkp_fd);

    //writing to server
    int client_fd = open(pipe_name, O_WRONLY);
    if (client_fd < 0) {
        perror("Failed to open client pipe");
        exit(1);
    }
    printf("Player %s\n", pipe_name);
    char buffer[BUFFER_SIZE];
    printf("Enter: ");
    fgets(buffer, BUFFER_SIZE, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    write(client_fd, buffer, sizeof(buffer));
    printf("Sent %s to opponent\n", buffer);
    close(client_fd);

    //reading from server
    printf("Waiting on opponent...\n");
    char buffer2[BUFFER_SIZE];

    strcat(pipe_name2, pipe_name);
    strcat(pipe_name2, "_2");
    mkfifo(pipe_name2, 0644);
    //printf("new pipe name: %s\n", pipe_name2);

    int from_server = open(pipe_name2, O_RDONLY);
    int bytes_read = read(from_server, buffer2, sizeof(buffer2));
    printf("Received from server: %s\n", buffer2);
    //printf("bytes read: %d\n", bytes_read);

    close(from_server);
    remove(pipe_name);
    remove(pipe_name2);
    return 0;
}

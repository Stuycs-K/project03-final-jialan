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
	char pipe_name[10];
	sprintf(pipe_name, "%d", getpid());
    mkfifo(pipe_name, 0644);

    int wkp_fd = open(WKP, O_WRONLY);
    if (wkp_fd < 0) {
        perror("Failed to connect to WKP");
        exit(1);
    }
    write(wkp_fd, pipe_name, strlen(pipe_name)+1);
    close(wkp_fd);

	//write to server
    int client_fd = open(pipe_name, O_WRONLY);
    if (client_fd < 0) {
        perror("Failed to open client pipe");
        exit(1);
    }

  char buffer[10];
  printf("Enter...\n");
  fgets(buffer, 10, stdin);
  printf("Attempt to match with player %s", buffer);
  write(client_fd, buffer, sizeof(buffer));
  printf("Sent request to player %s", buffer);

	
   close(client_fd);
   remove(pipe_name);
   return 0;
}

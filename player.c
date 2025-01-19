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

    printf("Type your game ID (3 digits) or enter for guest mode: ");
    fgets(pipe_name, PIPE_SIZE, stdin);
    pipe_name[strcspn(pipe_name, "\n")] = 0;
    if (strlen(pipe_name)==0) {
      sprintf(pipe_name, "%d", getpid());
      printf("Guest mode: (%s)\n", pipe_name);
      mkfifo(pipe_name, 0644);
    }
    else {
      // sprintf(pipe_name, "%d", getpid());
      struct stat stat_buf;
      if (stat(pipe_name, &stat_buf)==0) {
      	printf("Player ID is in session.\n");
      	exit(1);
      }
      else { //pipe doesn't exist yet
      	mkfifo(pipe_name, 0644);
      	}

      int return_player=0;
      char line_buff[256];
      char match[PIPE_SIZE];
      int win_count;
      // FILE *w = fopen("player_log.txt", "w");
      // fclose(w);
      FILE *fd = fopen("player_log.txt", "a+");
      while (fgets(line_buff, sizeof(line_buff), fd)) {
        sscanf(line_buff, "%s %d", match, &win_count);
        printf("PIPE_NAME: %s VS MATCH: %s\n", pipe_name, match);
        if (strcmp(pipe_name, match)==0) {
          printf("Welcome back player %s (%d wins)\n", match, win_count);
          return_player = 1;
          break;
        }
        else {
        	continue;
        }
      }
      if (return_player!=1) {
        printf("Welcome new player %s!\n", pipe_name);
        fprintf(fd, "%s %d\n", pipe_name, 0);  
        }
        fclose(fd);
    }

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
    //printf("Player %s\n", pipe_name);
    char buffer[BUFFER_SIZE];
    while (1) {
    printf("Enter: ");
    fgets(buffer, BUFFER_SIZE, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    if (!strcmp(buffer, "rock") || !strcmp(buffer, "paper") || !strcmp(buffer, "scissors")) {
      break;
      }
    else {
      printf("Not a valid choice: (rock/paper/scissors)\n");
    	}
    }
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

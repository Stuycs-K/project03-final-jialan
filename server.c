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
#define BUFFER_SIZE 50
#define PIPE_SIZE 10
#define MAX_CLIENTS 4

int client_fds[MAX_CLIENTS];
char* client_names[MAX_CLIENTS];
char *actions[MAX_CLIENTS];
int response = 0;
int client_count = 0;
int responses[MAX_CLIENTS];
// int wins[MAX_CLIENTS];
fd_set read_fds;

static void sighandler(int signo) {
  if (signo == SIGINT) {
    remove(WKP);
    kill(getpid(), SIGTERM);
  }
}

void check_connections(int wkp_fd) {
    // check if WKP has a new connection
    char client_pipe_name[PIPE_SIZE];
    if (FD_ISSET(wkp_fd, &read_fds)) {
        int bytes_read = read(wkp_fd, client_pipe_name, sizeof(client_pipe_name));
        if (bytes_read > 0) {
            printf("New client connected: %s\n", client_pipe_name);
            if (client_count < MAX_CLIENTS) {
                int client_fd = open(client_pipe_name, O_RDONLY);
                client_fds[client_count] = client_fd;
                client_names[client_count] = malloc(strlen(client_pipe_name)+1);
                strcpy(client_names[client_count], client_pipe_name);
                client_count++;
            } else {
                printf("Max clients reached. Cannot accept client %s\n", client_pipe_name);
                exit(1);
            }
        }
    }
}

int add_clients(int max_fd) {
    for (int i = 0; i < client_count; i++) {
        FD_SET(client_fds[i], &read_fds);
        if (client_fds[i] > max_fd) {
            max_fd = client_fds[i];
        }
    }
    return max_fd;
}

void store_action(int i, char* buffer) {
    if (responses[i] == 0) {
        actions[i] = malloc(strlen(buffer)+1);
        strcpy(actions[i], buffer);
        responses[i] = 1;
        response++;
    }
}

int write_to_players(int i, char* buffer) {
    int temp_fd = dup(client_fds[i]);
    close(client_fds[i]);
    for (int opponent=0; opponent < client_count; opponent++) {
        if (opponent != i) {
            sprintf(buffer, "Player %s chose %s. ", client_names[i], actions[i]);
            int to_client = open(client_names[opponent], O_WRONLY);
            if (to_client==-1) {
                perror("Opening to client write error");
            }
            int bytes_write = write(to_client, buffer, sizeof(buffer));
            printf("Wrote to clients: %s", buffer);
            if (bytes_write==-1) {
                perror("Writing to client error");
            }
            printf("Bytes wrote %d\n", bytes_write);
            close(to_client);
        }
    }
    response = 0;
    return temp_fd;
}


void save_wins(int i) {
    if (strlen(client_names[i])==3) {
        char line_buff[256];
        char match[PIPE_SIZE];
        int win_count, match_num, line;
        printf("Updating player_log.txt");
        FILE *fd = fopen("player_log.txt", "r+");
        while (fgets(line_buff, sizeof(line_buff), fd)) {
            sscanf(line_buff, "%d %d %d", &line, &match_num, &win_count);
            sprintf(match, "%d", match_num);
            if (strcmp(client_names[i], match)==0) {
                printf("Writing to %d: %d, %d", line, match_num, win_count+1);
                int info[3] = {line, match_num, win_count+1};
                fseek(fd, line, SEEK_SET);
                fwrite(info, sizeof(int), 3, fd);
                break;
            }
        }
        fclose(fd);
    }
}

void win_conditions(int i, int opponent) {
  if (strcmp(actions[i], "paper")==0) {
    if (strcmp(actions[opponent], "rock")==0) {
      responses[i] = 1;
      responses[opponent] = -1;
      save_wins(i);
    }
    else if (strcmp(actions[i], "scissors")==0) {
      if (strcmp(actions[opponent], "paper")==0)
      responses[i] = 1;
      responses[opponent] = -1;
      save_wins(i);
    }
    else if (strcmp(actions[i], "rock")==0) {
      if (strcmp(actions[opponent], "scissors")==0) {
        responses[i] = 1;
        responses[opponent] = -1;
        save_wins(i);
      }
    }
    else if (strcmp(actions[i], actions[opponent])==0){ //draw
      responses[i] = 0;
      responses[opponent] = 0;
    }
    else { //opponent wins
      responses[i] = -1;
      responses[opponent] = 1;
      save_wins(opponent);
    }
  }
}


void write_to_players2(int i, char* buffer) {
  int opponent;
  if (i==(client_count-1)) {
    opponent=i-1;
  }
  else {
    opponent=i+1;
  }
  printf("\n---Player %s vs Player %s---\n", client_names[i], client_names[opponent]);

  //checking who won
  win_conditions(i, opponent);
  char buffer2[BUFFER_SIZE];

  sprintf(buffer2, "Player %s chose %s.", client_names[i], actions[i]);
  // to opponent
  if (responses[opponent] > 0) {
    strcat(buffer2, "You won!");
  }
  else if (responses[opponent]==0) {
    strcat(buffer2, "It's a draw!");
  }
  else {
    strcat(buffer2, "You lost!");
  }
  char client_pipe2[BUFFER_SIZE];
  sprintf(client_pipe2, "%s_2", client_names[opponent]);
  printf("Client pipe new: %s\n", client_pipe2);
  int to_client = open(client_pipe2, O_WRONLY);
  if (to_client==-1) {
    perror("Opening to client write error");
  }
  int bytes_write = write(to_client, buffer2, sizeof(buffer2));
  printf("Wrote to client %s: %s", client_names[opponent], buffer2);
  if (bytes_write==-1) {
    perror("Writing to client error");
  }
  //printf("Bytes wrote %d\n\n", bytes_write);
  close(to_client);
  response = 0;
}

void clear_previous_game() {
  for (int i=0;i<client_count;i++ ){
    responses[i] = 0;
  }
  client_count = client_count-2;
  printf("\nNumber of clients left: %d", client_count);
}

int main() {
    signal(SIGINT, sighandler);
    int wkp_fd;
    char buffer[BUFFER_SIZE];

    // server setup
    wkp_fd = server_setup();
    printf("Server Log:\n");
    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(wkp_fd, &read_fds);
        int max_fd = wkp_fd;

        max_fd = add_clients(max_fd);

        int activity = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
        if (activity < 0) {
            perror("Select error");
            break;
        }

        check_connections(wkp_fd);

        // check if any client FDs have data to read
        for (int i = 0; i < client_count; i++) {
            if (FD_ISSET(client_fds[i], &read_fds)) {
                int bytes_read = read(client_fds[i], buffer, sizeof(buffer));
                if (bytes_read > 0) {
                    buffer[bytes_read] = '\0';
                    printf("Received from client %s: %s\n", client_names[i], buffer);

                    store_action(i, buffer);
                    printf("Stored action from client %s: %s\n", client_names[i], actions[i]);

                    // write to clients when both are done
                    printf("Response count: %d\n", response);
                    if (response>=2) {
                      for (int i=0; i<2; i++) {
                        write_to_players2(i, buffer);
                      }
                      clear_previous_game();
                      printf("\n---A game has ended---\n");
                    }
                }
            }
        }
    }
    return 0;
}

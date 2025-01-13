#include "handshake.h"

int server_setup() {
  printf("Setting up server...\n");
  int wkp_fd;
  mkfifo(WKP, 0644);
  wkp_fd = open(WKP, O_RDONLY);
  if (wkp_fd==-1) {
      perror("Failed to open WKP");
      exit(1);
  }
  printf("Waiting for connections...\n");
  
  // remove(WKP);
  return wkp_fd;

}

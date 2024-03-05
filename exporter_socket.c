#include <assert.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <regex.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>



int get_exporter_socket(char *hostname) {
  int port_no = 1337;
  int err_code = 0;
  char *msg = "test";
  struct hostent *server;
  struct sockaddr_in serv_addr;
  int socket_fd;
  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  server = gethostbyname(hostname);
  if (server == NULL){
    close(socket_fd);
    printf("Could not resolve host: \n");
    herror("error: ");
    
    exit(1);

  }
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port_no);
  memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
  err_code = connect(socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  if (err_code < 0) {
    close(socket_fd);
    printf("connection failed!!!!!!!!!!!!!\n");
    perror("more specifically: ");
    exit(1);
  }

  return socket_fd;
}

int send_stats(char *msg, int socket_fd) {
  int bytes;
  int sent = 0;
  int total = strlen(msg);
  do {
    bytes = write(socket_fd, msg + sent, total - sent);
    if (bytes == 0)
      break;
    sent += bytes;

  } while (sent < total);
  return 1;
}

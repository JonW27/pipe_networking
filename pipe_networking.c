#include "pipe_networking.h"


/*=========================
  server_handshake
  args: int * to_client

  Performs the client side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.

  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(int *to_client) {
  // create server FIFO
  mkfifo("to_server", 600);
  int fd = open("to_server", O_RDONLY, 644);
  close(fd[WRITE]);

  // read client message
  char msg[128];
  read(fd, msg, sizeof(msg));

  // remove WKP
  remove("to_server");

  // send acknowledgement msg
  *to_client = open(msg, O_WRONLY, 644);
  write(*to_client, ACK, sizeof(ACK));

  // close client connection
  close(*to_client);
  
  return fd;
}


/*=========================
  client_handshake
  args: int * to_server

  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.

  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
  // create client FIFO
  char * fifo_name = "to_client";
  mkfifo(fifo_name, 600);

  // connect to server FIFO
  *to_server = open("to_server", O_WRONLY, 644);
  close(*to_server[READ]);
  write(*to_server, fifo_name, sizeof(fifo_name));

  // remove client FIFO
  int fd = open(fifo_name, O_RDWR, 644);
  remove(fifo_name);

  // send acknowledgement msg
  write(*to_server, ACK, sizeof(ACK));

  // data transmission can happen here
  
  // client is done, exits
  close(*to_server);
  
  return fd;
}

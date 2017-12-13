#include "pipe_networking.h"


/*=========================
  server_handshake
  args: int * to_client

  Performs the client side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.

  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(int *to_client) {

  printf("===start server===\n");
  printf("waiting for client connection...\n");
  
  // create server FIFO
  char fifo_name[] = "to_server";
  mkfifo(fifo_name, 0644);
  int fd = open(fifo_name, O_RDONLY, 0644);

  printf("===after creating server fifo===\n");
  
  // read client fifo_name
  char msg[128];
  read(fd, msg, sizeof(msg));

  printf("read message: %s\n", msg);
  printf("===after reading client fifo_name===\n");
  
  // remove WKP
  remove(fifo_name);

  printf("===after removing WKP===\n");
  
  // send acknowledgement msg
  *to_client = open(msg, O_WRONLY, 0644);
  write(*to_client, ACK, sizeof(ACK));

  printf("sending acknowledgement message: %s\n", ACK);
  printf("===after sending acknowledgement msg===\n");
  
  //read client msg
  read(fd, msg, sizeof(msg));

  printf("read message: %s\n", msg);
  printf("===after reading client msg===\n");
  
  // close client connection
  close(*to_client);

  printf("===end server===\n");
  
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

  printf("===start client===\n");
  
  // create client FIFO
  char fifo_name[] = "to_client";
  mkfifo(fifo_name, 0644);

  printf("===after create client fifo===\n");
  
  // connect to server FIFO
  *to_server = open("to_server", O_WRONLY, 0644);
  write(*to_server, fifo_name, sizeof(fifo_name));

  printf("writing fifo_name message: %s\n", fifo_name);
  printf("===after connect to server fifo===\n");
  
  // read client message
  int fd = open(fifo_name, O_RDONLY, 0644);
  char msg[128];
  read(fd, msg, sizeof(msg));

  printf("read message: %s\n", msg);
  printf("===after read client message===\n");
  
  // remove client FIFO
  remove(fifo_name);

  printf("===after removing client fifo===\n");
  
  // send acknowledgement msg
  write(*to_server, ACK, sizeof(ACK));

  printf("sending acknowledgement message: %s\n", ACK);
  printf("===after sending acknowledgement msg===\n");
  
  // data transmission can happen here
  
  // client is done, exits
  close(*to_server);

  printf("===end client===\n");
  
  return fd;
}

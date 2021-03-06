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

  // data transmission
  int CLIENT_UP = 1;
  while(CLIENT_UP){
    char buf[128];
    read(fd, buf, sizeof(buf));
    printf("Server received data %s\n", buf);
    CLIENT_UP = strncmp("exit\n", buf, 6);
    int i = 0;
    while(buf[i]){
      buf[i] = toupper(buf[i]);
      i++;
    }
    printf("Server changed data to %s\n", buf);
    write(*to_client, buf, sizeof(buf));
  }

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
  int loop = 1;
  while(loop){
    char in[128];
    printf("<ENTER TEXT>: ");
    fgets(in, sizeof(in), stdin);
    loop = strncmp(in, "exit\n", 6);
    write(*to_server, in, sizeof(in));
    read(fd, in, sizeof(in));
    printf("<Received server response: %s", in);
  }

  // client is done, exits
  close(*to_server);

  printf("===end client===\n");

  return fd;
}

#include "pipe_networking.h"

/*=========================
  server_handshake
  args: int * to_client

  Performs the client side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.

  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(int *to_client) {
  printf("server: making upstream (WKP) pipe\n");
  mkfifo("to_server",0644);

  int upstream = open("to_server",O_RDONLY);
  char pipe_name[HANDSHAKE_BUFFER_SIZE];
  read(upstream, pipe_name, HANDSHAKE_BUFFER_SIZE);
  remove("to_server");
  printf("server: read downstream pipe name and removed upstream pipe\n");

  int f = fork();
  if(f){ //server then returns, and remakes WKP
    return 0;
  }
  int downstream = open(pipe_name,O_WRONLY);
  write(downstream,ACK,HANDSHAKE_BUFFER_SIZE);
  printf("subserver: sent client acknowledgement message %s\n",ACK);

  char ack_message[HANDSHAKE_BUFFER_SIZE];
  read(upstream, ack_message, HANDSHAKE_BUFFER_SIZE);
  printf("subserver: read message %s\n", ack_message);

  *to_client = downstream;
  return upstream;
}


/*=========================
  client_handshake
  args: int * to_server

  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.

  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
  mkfifo("to_client",0644);
  printf("client: made downstream pipe\n");

  int upstream = open("to_server",O_WRONLY);
  char pipe_name[HANDSHAKE_BUFFER_SIZE] = "to_client";
  write(upstream,pipe_name,HANDSHAKE_BUFFER_SIZE);
  printf("client: sent server downstream pipe name\n");

  int downstream = open("to_client",O_RDONLY);
  char ack_message[HANDSHAKE_BUFFER_SIZE];
  read(downstream,ack_message,HANDSHAKE_BUFFER_SIZE);
  printf("client: read message %s\n", ack_message);
  remove("to_client");

  write(upstream, ack_message, HANDSHAKE_BUFFER_SIZE);

  *to_server = upstream;
  return downstream;
}

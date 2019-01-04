#include "pipe_networking.h"

static void sighandler(int signo){
  if (signo == SIGINT) {
    remove("to_server");
    exit(0);
  }
}

int main() {
  signal(SIGINT,sighandler);
  int to_client;
  int from_client;

  while(1){
    from_client = server_handshake( &to_client );
    while(from_client){ //server passes over this while loop, and initiates another handshake
      char message[BUFFER_SIZE];
      int p = read(from_client,message,BUFFER_SIZE);

      if(!p){
        return 0; //if client not there, subserver terminates
      }

      int i = 0;
      while(message[i]){
        message[i] = toupper(message[i]);
        i++;
      }

      write(to_client,message,BUFFER_SIZE);
    }
  }

}

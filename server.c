#include <arpa/inet.h>		// for inet_addr, bind, listen, accept, socket
#include <sys/socket.h>
#include <netinet/in.h>		// for structures relating to IPv4 addresses
#include <stdio.h>			// for print functions
#include <stdlib.h>			// for malloc, free, exit
#include <string.h>			// for strcpy, memset
#include <unistd.h>			// for read, write, close  
#include <signal.h>     // for sigaction

#include "serv_functions.h" //helper functions for server

//TODO: hash table for functions?
//TODO: how to read commands to server during server execution?
//TODO: multiple clients with different connections?
//TODO: loggin in?
//TODO: file storage 
//TODO: encryption of file storage

//IPv4-only sockaddr_in struct:
//struct sockaddr_in {
//  short int     sin_family;       address family, which is AF_INET for IPv4
//  unsigned short int  sin_port;   port number, 16 bit
//  struct in_addr      sin_addr;   Struct containing the 4-byte IP address
//  unsigned char       sin_zero[8]; padding of 0's to make it the same size as a struct sockaddr

const size_t MAX_INPUT_SIZE = 256;
const int PORT_NUM = 8080;
const int BACKLOG = 10;

void sigchild_handler(int signum) {
  (void) signum; // suppress unused var warning
  while(waitpid(-1, NULL, WNOHANG) > 0);
}


int main (int argc, char const* argv[]) {
  struct sockaddr_in serv_address; //struct containing IP address, portnum
  struct sockaddr_in client_address;
  serv_address.sin_family = AF_INET; //use IPv4
  serv_address.sin_addr.s_addr = htonl(INADDR_ANY); //listen for requests from any IP on computer
  serv_address.sin_port = htons(PORT_NUM); //server listens on port 8080 
  
  //int sockfd = socket(domain, type, protocol)
  int test_socket = socket(AF_INET, SOCK_STREAM, 0); //AFINET: IPv4, SOCK_STREAM: TCP (bytestream), 0: IP 
  printf("Starting server\n");
  
  //int bind(int sockfd, sockaddr *addr, socklen_t addrlen);
  bind(test_socket, (struct sockaddr*) &serv_address, sizeof(struct sockaddr_in));
  //enable socket reuse when rerunning server
  int yes = 1;
  setsockopt(test_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
  //int listen(int sockfd, int backlog). 
  listen(test_socket, BACKLOG); //up to {BACKLOG} pending connections in queue to be accepted.
  
  //for reaping child processes
  struct sigaction sa; 
  sa.sa_handler = sigchild_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  sigaction(SIGCHLD, &sa, NULL);
  
  sleep(1);
  printf("listening on port %d\n", PORT_NUM);


//  char serv_cmd[MAX_INPUT_SIZE];

//if (fgets(serv_cmd, MAX_INPUT_SIZE, stdin) 
//size_t cmd_size = strlen(serv_cmd);
//if ((cmd_size > 0) && (serv_cmd[cmd_size-1] == '\n')) {
  //serv_cmd[cmd_size-1] = '\0';
//}
//if (strcmp(serv_cmd, "close") == 0) {
  //break;
//}
while(1) {
    socklen_t cli_address_len = sizeof(struct sockaddr_in);
    int client_socket = accept(test_socket, (struct sockaddr*) &client_address, &cli_address_len);
    if (client_socket == -1) {
      printf("connection failed\n");
      continue;
    }  
    //could probably make this a function
    if (!fork()) { //successful fork() returns 0
      close(test_socket); //parent's listening socket unneeded; only handles client_socket, not new connections
      printf("created connection with client on socket number %d\n", client_socket);     

      Command* cmd_map = init_cmdmap();
      char* client_msg;
      char* client_cmd;
      while (1) {
        client_msg = receiveMsg(client_socket);
        client_cmd = readCommand(client_msg);
        if (strcmp(client_cmd, "exit") == 0) {
          sendMsg("connection terminated\n", client_socket);
          free(client_msg);
          free(client_cmd);
          free(cmd_map);
          printf("closing connection on socket %d\n", client_socket);
          close(client_socket);
          exit(0);
        }
        executeCommand(client_cmd, client_msg, cmd_map, client_socket);
      }
      free(cmd_map);
      free(client_msg);
      free(client_cmd);
      close(client_socket);
      exit(0); 
    }
    
    close(client_socket);
  }

  printf("shutting down server\n");
  close(test_socket);
  return 0;
}



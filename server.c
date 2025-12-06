#include <arpa/inet.h>		// for inet_addr, bind, listen, accept, socket
#include <netinet/in.h>		// for structures relating to IPv4 addresses
#include <stdio.h>			// for print functions
#include <stdlib.h>			// for malloc, free, exit
#include <string.h>			// for strcpy, memset
#include <unistd.h>			// for read, write, close  
#include "serv_functions.h" //helper functions for server

//TODO: have a const size array of Command structs, iterate through them all to match cmd name. 
//if match, then exec func pointer
//would be faster if you could hash? but then you'd need a hash table with custom hash for Command struct...
//TODO multiple clients with different connections?
//TODO: temp command
//TODO: file storage 
//TODO: encryption of file storage

const int PORT_NUM = 8080;

int main (int argc, char const* argv[]) {
  struct sockaddr_in serv_address;
  struct sockaddr_in client_address;
  serv_address.sin_family = AF_INET;
  serv_address.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_address.sin_port = htons(PORT_NUM);
  //printf("listening on port %d\n", serv_address.sin_port); this doesn't work; 
  int test_socket = socket(AF_INET, SOCK_STREAM, 0);
  printf("Starting server\n");
  bind(test_socket, (struct sockaddr*) &serv_address, sizeof(struct sockaddr_in));
  listen(test_socket, 2);
  sleep(4);
  printf("listening on port %d\n", PORT_NUM);
  socklen_t cli_address_len = sizeof(struct sockaddr_in);
  int client_socket = accept(test_socket, (struct sockaddr*) &client_address, &cli_address_len);
  printf("created connection with client on socket number %d\n", client_socket);  

  char* client_msg;
  char* client_cmd;
  while (1) {
    client_msg = receiveMsg(client_socket);
    client_cmd = readCommand(client_msg);
    if (strcmp(client_cmd, "exit") == 0) {
      printf("terminating connection with client\n");
      sendMsg("connection terminated\n", client_socket);
      free(client_msg);
      free(client_cmd);
      printf("closing client_socket\n");
      close(client_socket);
      printf("closing server socket\n");
      close(test_socket);
      return 0;
    }
    executeCommand(client_cmd, client_msg, client_socket);
  }
  
  close(client_socket);
  close(test_socket);
  return 0;
}



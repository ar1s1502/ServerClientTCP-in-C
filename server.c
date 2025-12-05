#include <arpa/inet.h>		// for inet_addr, bind, listen, accept, socket
#include <netinet/in.h>		// for structures relating to IPv4 addresses
#include <stdio.h>			// for print functions
#include <stdlib.h>			// for malloc, free, exit
#include <string.h>			// for strcpy, memset
#include <unistd.h>			// for read, write, close  

    free(client_msg);
    return 0;
 }
  
int main (int argc, char const* argv[]) {
  struct sockaddr_in serv_address;
  struct sockaddr_in client_address;
  serv_address.sin_family = AF_INET;
  serv_address.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_address.sin_port = htons(8080);
  //printf("listening on port %d\n", serv_address.sin_port); this doesn't work; 
  int test_socket = socket(AF_INET, SOCK_STREAM, 0);
  bind(test_socket, (struct sockaddr*) &serv_address, sizeof(struct sockaddr_in));
  listen(test_socket, 2);

  socklen_t cli_address_len = sizeof(struct sockaddr_in);
  int client_socket = accept(test_socket, (struct sockaddr*) &client_address, &cli_ad    dress_len);
  
  char* client_msg = receiveMsg(client_socket);
  char* client_cmd = readCommand(client_msg);
  executeCommand(client_cmd, client_msg, client_socket);
 
  client_msg = receiveMsg(client_socket);
  client_cmd = readCommand(client_msg);
  executeCommand(client_cmd, client_msg, client_socket);
     
  close(client_socket);
  //read(test_socket)
  close(test_socket);
  return 0;
}


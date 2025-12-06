#include <arpa/inet.h>		// for inet_addr, bind, listen, accept, socket
#include <netinet/in.h>		// for structures relating to IPv4 addresses
#include <stdio.h>			// for print functions
#include <stdlib.h>			// for malloc, free, exit
#include <string.h>			// for strcpy, memset
#include <unistd.h>			// for read, write, close  


// initialize a command struct?
typedef struct {
  char* cmd;
  int (*func_ptr) (char*);    
} Command;
//have a const size array of Command structs, iterate through them all to match cmd name. 
//if match, then exec func pointer
//would be faster if you could hash? but then you'd need a hash table with custom hash for Command struct...
char* echo = "echo";
char* temp = "temp";
char* exit_cmd = "exit";
const int PORT_NUM = 8080;

int sendMsg(char* msg, int socket_num) {
  size_t msg_size = strlen(msg) + 1; //+ 1 for null
  printf("sending size %zu message to client at socket num %d: %s\n", msg_size, socket_num, msg);
  write(socket_num, &msg_size, sizeof(size_t));
  write(socket_num, msg, msg_size);
  return 0;
}

char* receiveMsg(int socket_num) {
  size_t msg_size;
  read(socket_num, &msg_size, sizeof(msg_size));
  //printf("msg_size: %zu\n", msg_size);
  char* buf = (char*) malloc(msg_size);
  read(socket_num, buf, msg_size);     
  buf[msg_size-1] = '\0';
  printf("received size %zu message from client on %d socket_num: %s\n", msg_size, socket_num, buf);
  return buf;
 }

size_t read_whitespace(char* msg) {
  int whitespace = 0;
  while (msg[whitespace] == ' ') {
    whitespace++;
  }
  return whitespace;
}
  
char* readCommand(char* client_msg) {
  size_t whitespace = read_whitespace(client_msg);
  size_t end = whitespace;
  while (end <= strlen(client_msg) && client_msg[end] != ' ') {
    end++;
  }
  size_t cmd_size = (size_t) (end - whitespace + 1);
  char* client_cmd = malloc(cmd_size);
  strlcpy(client_cmd, client_msg + whitespace, cmd_size);
  printf("client cmd: %s\n", client_cmd); 
  return client_cmd;
}

int executeCommand(char* cmd, char* msg, int socket_num) {
  int success;
  if (strcmp(cmd, echo) == 0) {
    size_t whitespace = read_whitespace(msg + strlen(echo));
    sendMsg(msg + strlen(cmd) + whitespace, socket_num);
  }
  else {
    char* fail_msg = "unknown command";
    printf("%s\n", fail_msg);
    sendMsg(fail_msg, socket_num);
    success = -1;
  }
  free(msg);
  free(cmd);
  return success;
}

int main (int argc, char const* argv[]) {
  struct sockaddr_in serv_address;
  struct sockaddr_in client_address;
  serv_address.sin_family = AF_INET;
  serv_address.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_address.sin_port = htons(PORT_NUM);
  //printf("listening on port %d\n", serv_address.sin_port); this doesn't work; 
  int test_socket = socket(AF_INET, SOCK_STREAM, 0);
  bind(test_socket, (struct sockaddr*) &serv_address, sizeof(struct sockaddr_in));
  listen(test_socket, 2);
  printf("Starting server on port %d\n", PORT_NUM);
  socklen_t cli_address_len = sizeof(struct sockaddr_in);
  int client_socket = accept(test_socket, (struct sockaddr*) &client_address, &cli_address_len);
  printf("created connection with client on socket number %d\n", client_socket);  

  char* client_msg;
  char* client_cmd;
  while (1) {
    client_msg = receiveMsg(client_socket);
    client_cmd = readCommand(client_msg);
    if (strcmp(client_cmd, exit_cmd) == 0) {
      printf("terminating connection with client\n");
      sendMsg("connection terminated", client_socket);
      free(client_msg);
      free(client_cmd);
      close(client_socket);
      close(test_socket);
      return 0;
    }
    executeCommand(client_cmd, client_msg, client_socket);
  }
  //char* client_msg = receiveMsg(client_socket);
  //char* client_cmd = readCommand(client_msg);
  //executeCommand(client_cmd, client_msg, client_socket);
 
  //client_msg = receiveMsg(client_socket);
  //client_cmd = readCommand(client_msg);
  //executeCommand(client_cmd, client_msg, client_socket);
     
  close(client_socket);
  //read(test_socket)
  close(test_socket);
  return 0;
}


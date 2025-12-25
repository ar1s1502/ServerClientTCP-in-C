#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

int SERV_PORT_NUM = 8080;
char* exit_cmd = "exit";
size_t MAX_INPUT_SIZE = 256;

int sendMsg(char* msg, int socket_num) {
  size_t msg_size = strlen(msg);
  if (msg_size > 0 && msg[msg_size-1] == '\n') {
    msg[msg_size-1] = '\0';
  }
  msg_size = strlen(msg) + 1; //+ 1 for null byte
  printf("sending size %zu message: %s\n", msg_size, msg);
  send(socket_num, &msg_size, sizeof(size_t), 0);
  send(socket_num, msg, msg_size, 0); 
  return 0;
}

int receiveMsg(int socket_num) {
  size_t msg_size;
  recv(socket_num, &msg_size, sizeof(size_t), 0); 
  char* buf = malloc(msg_size);
  size_t read_bytes = recv(socket_num, buf, msg_size, 0);
  if (read_bytes != msg_size) {
    printf("packets dropped");
    return -1;
  }
  buf[msg_size-1] = '\0';
  char* fail_msg = "unknown command";
  if (strcmp(buf, fail_msg) == 0) {
    printf("%s\n", fail_msg);
    return -1;
  }  
  printf("%s\n", buf);
  free(buf);
  return 0;
}

int main (int argc, char const* argv[]) {
  struct sockaddr_in serv_address;
  int test_socket = socket(AF_INET, SOCK_STREAM, 0);
  serv_address.sin_family = AF_INET;
  serv_address.sin_addr.s_addr = htonl(INADDR_ANY); 
  serv_address.sin_port = htons(SERV_PORT_NUM);
  
  int status = connect(test_socket, (struct sockaddr*) &serv_address, sizeof(serv_address));
  if (status < 0) {
    printf("Connection failure\n");
    return -1;
  }

  char client_cmd[MAX_INPUT_SIZE];
  printf("Enter commands here. Press Ctrl^D to exit.\n");
  while (fgets(client_cmd, MAX_INPUT_SIZE, stdin)) {
    sendMsg(client_cmd, test_socket);
    receiveMsg(test_socket); 
    if (strcmp(client_cmd, exit_cmd) == 0) {
      printf("closing server socket\n");
      close(test_socket);
      return 0;
    }
  }

  char* msg = "exit";
  sendMsg(msg, test_socket);
  receiveMsg(test_socket);

  printf("closing server socket\n");  
  close(test_socket);
  return 0;
}

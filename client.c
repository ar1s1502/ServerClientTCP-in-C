#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

int sendMsg(char* msg, int socket_num) {
  size_t msg_size = strlen(msg) + 1; //+ 1 for null byte
  printf("sending size %zu message: %s\n", msg_size, msg);
  write(socket_num, &msg_size, sizeof(size_t));
  write(socket_num, msg, msg_size);
  return 0;
}

int receiveMsg(int socket_num) {
  size_t msg_size;
  read(socket_num, &msg_size, sizeof(msg_size));
  char* buf = malloc(msg_size + 1);
  read(socket_num, buf, msg_size+1);
  buf[msg_size] = '\0';
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
  serv_address.sin_port = htons(8080);
  
  int status = connect(test_socket, (struct sockaddr*) &serv_address, sizeof(serv_address));
  if (status < 0) {
    printf("Connection failure\n");
    return -1;
  }
  
  char* msg = "echo Hello server, this is client";
  sendMsg(msg, test_socket);
  receiveMsg(test_socket);

  msg = "random unrecognized command to server";
  sendMsg(msg, test_socket);
  receiveMsg(test_socket);
  
  close(test_socket);
  return 0;
}

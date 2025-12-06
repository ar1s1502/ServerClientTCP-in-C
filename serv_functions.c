#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>         //for read/write/close
#include "serv_functions.h"

char* command_list[3] = {"ping", "echo", "temp"};
size_t num_commands = sizeof(command_list) / sizeof(command_list[0]);
//Command command_map[num_commands];

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
  printf("whitespace: %zu\n", whitespace);
  printf("client cmd: %s\n", client_cmd); 
  return client_cmd;
}

char* get_args(char* cmd, char* msg) {
    size_t space_til_arg = read_whitespace(msg);
    space_til_arg += strlen(cmd) + read_whitespace(msg + space_til_arg + strlen(cmd));
    //printf("space_til_arg: %zu\n", space_til_arg);
    //printf("first char of args: %c\n", msg[space_til_arg]);
    size_t args_size = strlen(msg + space_til_arg) + 1; //+1 for nul term
    char* args = (char*) malloc(args_size);
    //printf("args_size: %zu\n", args_size);
    strlcpy(args, msg + space_til_arg, args_size);
    printf("args: %s\n", args);  
    return args;
}


int executeCommand(char* cmd, char* msg, int socket_num) {
  int success = 0;
  char* args = get_args(cmd, msg);
  if (strcmp(cmd, "echo") == 0) {
    echo_exec(args, socket_num);
  }
  else {
    char* fail_msg = "unknown command";
    printf("%s\n", fail_msg);
    sendMsg(fail_msg, socket_num);
    success = -1;
  }
  free(msg);
  free(cmd);
  free(args);
  return success;
}


//command execution functions

//echo: prints args
int echo_exec(char* args, int client_socket) {
  return sendMsg(args, client_socket);
}


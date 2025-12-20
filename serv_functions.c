#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>         //for read/write/close
#include "serv_functions.h"


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
  printf("received size %zu message from client on socket_num %d: %s\n", msg_size, socket_num, buf);
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
    //printf("args: %s\n", args);  
    return args;
}


//command execution functions
//echo: prints args
void echo_exec(char* args, int client_socket) {
  sendMsg(args, client_socket);
}

size_t NUM_CMD = 1; 

Command* init_cmdmap() {
  Command echo = {.cmd = "echo", .func_ptr = &echo_exec};
  Command command_map[] = {
    echo
  };
  Command* map_ptr = malloc(sizeof(command_map));
  memcpy(map_ptr, command_map, sizeof(command_map));
  return map_ptr;
}

//temp: converts F to C or C to F


int executeCommand(char* cmd, char* msg, Command* cmd_map, int socket_num) {
  int success = 0;
  char* args = get_args(cmd, msg);
  //Command* cmds = get_all_cmd();
  size_t i = 0;
  Command cmd_struct = cmd_map[i];
  while ((strcmp(cmd, cmd_struct.cmd) != 0) && (i < NUM_CMD)) {
    cmd_struct = cmd_map[i++];
  } 
  if (strcmp(cmd, cmd_struct.cmd) == 0) {
    cmd_struct.func_ptr(args, socket_num);  
  }
  else {
    char* fail_msg = "unknown command";
    printf("%s\n", fail_msg);
    sendMsg(fail_msg, socket_num);
    success = -1;
  }
  free(args);
  return success;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>         //for read/write/close
#include <string.h>         //for strstr 
#include <regex.h>          //POSIX regex library
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
    char* cmdstrt = strstr(msg, cmd);
    char* arg_start = cmdstrt + strlen(cmd);
    if (arg_start[0] == '\0') { return NULL; }
    size_t arg_len = sizeof(arg_start);
    char* args = malloc(arg_len);
    strlcpy(args, arg_start, arg_len);
    printf("args: %s\n", args);
    return args;
}


//command execution functions
//echo: prints args
void echo_exec(char* args, int client_socket) {
  if (args != NULL) {
    char* resp = args;
    if (args[0] == ' ') {
      resp += 1;
    }
    sendMsg(resp, client_socket);
  } else {
    sendMsg("no args given", client_socket);
  }  
}


//#TODO: implement for negative valued temperatures
//helper func to regex match the integer argument for temp command
char* match_int(char* input) {
  regex_t regex;
  regmatch_t matches[2]; //capture groups are stored in regmatch_t array. 0th element is entire capture, 1th element is first capture, 2nd element is second capture, etc. 
  const char* pattern = "([0-9]+(\\.[0-9]*)?|\\.[0-9]+)";
  if (regcomp(&regex, pattern, REG_EXTENDED) != 0) { //regex must be compiled
    fprintf(stderr, "Couldn't compile regex\n");
    return NULL;
  }
  if (regexec(&regex, input, 2, matches, 0) == 0) {
  //A regmatch_t struct has a .rm_so = index of start of match and a .rm_eo = index of end of match in input string
    size_t length = (size_t) (matches[1].rm_eo - matches[1].rm_so); 
    char* match = malloc(length + 1);
    strlcpy(match, input + matches[1].rm_so, length + 1); 
    return match;
  }
  return NULL;
}
//temp: convert celsius to fahrenheit or vice versa
void temp_exec(char* args, int client_socket) {  
  if (args == NULL) {
    char* msg = "No arguments given\n";
    sendMsg(msg, client_socket);
    return;
  }
  const size_t max_digits = 6;
  char* match = match_int(args);
  //printf("match: %s\n", match);
  long temp = strtol(match, NULL, 10);
  free(match);
  if (strstr(args, "-c") != NULL) {
    float cels_temp = ((float) temp - 32) * (((float) 5)/9);
    char* new_temp = malloc(max_digits);
    snprintf(new_temp, max_digits, "%f", cels_temp);
    char* text = " degrees Celsius";
    size_t resp_size = strlen(text) + strlen(new_temp) + 1;
    new_temp  = (char*) realloc(new_temp, resp_size); 
    strlcat(new_temp, text, resp_size);
    sendMsg(new_temp, client_socket);
    free(new_temp);
    return;
  } else if (strstr(args, "-f") != NULL) { //celsius to fahrenheit
    float fahr_temp = (((float) 9)/5) * temp + 32;
    char* new_temp = malloc(max_digits);
    snprintf(new_temp, max_digits, "%f", fahr_temp);
    char* text = " degrees Fahrenheit";
    size_t resp_size = strlen(text) + strlen(new_temp) + 1;
    new_temp  = (char*) realloc(new_temp, resp_size); 
    strlcat(new_temp, text, resp_size);
    sendMsg(new_temp, client_socket);
    free(new_temp);
    return;
  } else {
    char* msg = "Enter -f or -c to specify fahrenheit or celsius conversion, respectively\n";
    sendMsg(msg, client_socket);
  }
  return;
}

size_t NUM_CMD = 2; 

Command* init_cmdmap() {
  Command echo = {.cmd = "echo", .func_ptr = &echo_exec};
  Command temp = {.cmd = "temp", .func_ptr = &temp_exec};
  Command command_map[] = {
    echo,
    temp
  };
  Command* map_ptr = malloc(sizeof(command_map));
  memcpy(map_ptr, command_map, sizeof(command_map));
  return map_ptr;
}

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

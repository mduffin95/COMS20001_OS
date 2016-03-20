#include "cli.h"
#include "libc.h"

#include <string.h>
#include <stdlib.h>

const commandStruct commands[] = {
  {"test", test_func, 0, "A test function."},
  {"run", run, 0, "Run a user program."},
  {"",0,0,""}
};

void process_command( char *buffer ) {
  uint16_t args[MAX_ARGS] = {0};
  uint8_t num_args = 0;
  char *inst, *token;

  inst = strtok(buffer, " ");
  token = strtok(NULL, " ");

  int i;
  for(i=0; token != NULL && i < MAX_ARGS; i++) {
    args[i] = atoi(token);
    num_args++;
    token = strtok(NULL, " ");
  }

  uint16_t cmdCount = 0;
  while(strcmp(commands[cmdCount].name,"")) {
    if(!strcmp(commands[cmdCount].name,inst)) {
      commands[cmdCount].execute(num_args, args);
      break;
    }
    cmdCount++;
  }
}

void test_func(uint8_t argc, uint16_t *argv) {
  write( 0, "E", 1 );
}

void run( uint8_t argc, uint16_t *argv ) {
  pid_t pid = fork();
  if(!pid) {
    //Child, so run new program.
    execv("Blah", NULL);
  }
}

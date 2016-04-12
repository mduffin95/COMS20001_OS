#include "shell.h"
#include "libc.h"
#include "cli.h"

void shell() {
  char x;
  while (1) {
    if ( read( -1, &x, 1 ) ) {
      process_input(x);
    }
  }
}

void (*entry_shell)() = &shell;

char inst_buffer[SZ_INST_BUFFER];
uint8_t inst_end = 0;

/*
 * Takes character and stores in instruction buffer (inst_buffer) which is 256 chars long.
 */
int process_input(uint8_t data) //Returns 1 if buffer is full. Otherwise 0.
{
    switch (data) {
    case '\r':
        inst_buffer[inst_end] = '\0';
        process_command(inst_buffer); //Determines which cmd function to run.
        inst_clear();
        write( -1, "\r\n", 2 );
        break;
    case '\n':
        //ignore
        break;
    case 127:
        inst_del();
        write( -1, &data, 1 );
        // PL011_putc( UART0,  data  );
        break;
    default:
        // PL011_putc( UART0,  data  );
        write( -1, &data, 1 );
        return inst_add(data);
    }
    return 0;
}


//Adds a character to the buffer.
int inst_add(uint8_t data)
{
    inst_buffer[inst_end] = data; //Not sure this is updated properly.
    if(inst_end == SZ_INST_BUFFER - 1)
        return 1;
    else
    {
        inst_end++;
        return 0;
    }
}

//Deletes a character from the buffer.
void inst_del()
{
    if (inst_end) inst_end--;
}


void inst_clear()
{
    inst_end = 0;
}

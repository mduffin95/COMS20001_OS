#include "PL011.h"
#include "inst.h"
#include "cli.h"

char inst_buffer[SZ_INST_BUFFER];
uint8_t inst_end = 0;

/*
 * Takes character and stores in instruction buffer (inst_buffer) which is 256 chars long.
 */
int inst_process(uint8_t data) //Returns 1 if buffer is full. Otherwise 0.
{
    switch (data)
    {
    case '\r':
        inst_buffer[inst_end] = '\0';
        exec_command(inst_buffer); //Determines which cmd function to run.
        inst_clear();
        PL011_putc( UART0,  '\r'  );
        PL011_putc( UART0,  '\n'  );
        break;
    case '\n':
        //ignore
        break;
    case 127:
        inst_del();
        PL011_putc( UART0,  data  );
        break;
    default:
        PL011_putc( UART0,  data  );
        return inst_add(data);
    }
    return 0;
}


//Adds a character to the buffer.
int inst_add(uint8_t data)
{
    inst_buffer[inst_end] = data;
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

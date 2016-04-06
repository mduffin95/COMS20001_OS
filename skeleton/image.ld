SECTIONS {
  /* assign address (per  QEMU)  */
  .       =     0x70010000; 
  /* place text segment(s)       */
  .text : { kernel/interrupt.o(.text) *(.text .rodata) } 
  /* place data segment(s)       */        
  .data : {                           *(.data        ) } 
  /* place bss  segment(s)       */        
  .bss  : {                           *(.bss         ) }
}

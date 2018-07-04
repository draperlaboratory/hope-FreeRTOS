/*
 * FreeRTOS Kernel V10.0.0
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software. If you wish to use our Amazon
 * FreeRTOS name, please do so in a fair use way that does not cause confusion.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

GROUP( -lc -lgloss -lgcc )

OUTPUT_ARCH( "riscv" )

_STACK_SIZE = DEFINED(_STACK_SIZE) ? _STACK_SIZE : 0x1000;
_HEAP_SIZE = DEFINED(_HEAP_SIZE) ? _HEAP_SIZE : 0x1000;

/*****************************************************************************
 * Define memory layout
 ****************************************************************************/
MEMORY {
	imem : ORIGIN = 0x80000100, LENGTH = 0x00100000
	dmem : ORIGIN = 0x80100000, LENGTH = 0x00700000
}

/* Specify the default entry point to the program */

ENTRY(_mstart)

/*ENTRY(user_trap_entry) */
/*****************************************************************************
 * Define the sections, and where they are mapped in memory 
 ****************************************************************************/
SECTIONS {

  /*--------------------------------------------------------------------*/
  /* Code and read-only segment                                         */
  /*--------------------------------------------------------------------*/

  /* Begining of code and text segment */
	. = 0x80000100;
	  _ftext = .;
	  PROVIDE( eprol = . );
	.freertos_text : {
	    *boot.S.o(.text);
	    *croutine.c.o(.text);
	    *list.c.o(.text);
	    *queue.c.o(.text);
	    *tasks.c.o(.text);
	    *timers.c.o(.text);
	    *event_groups.c.o(.text);
	    *heap_2.c.o(.text);
	    *string.c.o(.text);
	    *port.c.o(.text);
	    *portasm.S.o(.text);
	    *miv_core_uart.c.o(.text);

	    *boot.S.o(.text.*);
	    *croutine.c.o(.text.*);
	    *list.c.o(.text.*);
	    *queue.c.o(.text.*);
	    *tasks.c.o(.text.*);
	    *timers.c.o(.text.*);
	    *event_groups.c.o(.text.*);
	    *heap_2.c.o(.text.*);
	    *string.c.o(.text.*);
	    *port.c.o(.text.*);
	    *portasm.S.o(.text.*);
	    *miv_core_uart.c.o(.text.*);

	    *boot.o(.text);
	    *croutine.o(.text);
	    *list.o(.text);
	    *queue.o(.text);
	    *tasks.o(.text);
	    *timers.o(.text);
	    *event_groups.o(.text);
	    *heap_2.o(.text);
	    *string.o(.text);
	    *port.o(.text);
	    *portasm.o(.text);
	    *ns16550.o(.text);
	} > imem

  /* text: Program code section */
  .text : 
  {
    *(.text)
    *(.text.*)
    *(.gnu.linkonce.t.*)
  } > imem

  /* init: Code to execute before main (called by crt0.S) */
  .init : 
  {
    KEEP( *(.init) )
  } > imem

  /* fini: Code to execute after main (called by crt0.S) */
  .fini : 
  {
    KEEP( *(.fini) )
  } > imem

    .dover_policy_names : {
        . = ALIGN(4);
	*(.dover_policy_names)
    } > imem

    .rodata : {
       __rodata_start = .;
       *(.rodata)
       *(.rodata.*)
       *(.gnu.linkonce.r.*)
       __rodata_end = .;
	} > dmem

  /* End of code and read-only segment */
  PROVIDE( etext = . );
  _etext = .;

  /*--------------------------------------------------------------------*/
  /* Global constructor/destructor segement                             */
  /*--------------------------------------------------------------------*/

  .preinit_array     :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  }

  .init_array     :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array ))
    PROVIDE_HIDDEN (__init_array_end = .);
  }

  .fini_array     :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array ))
    PROVIDE_HIDDEN (__fini_array_end = .);
  }

  /*--------------------------------------------------------------------*/
  /* Initialized data segment                                           */
  /*--------------------------------------------------------------------*/

  /* Start of initialized data segment */
  . = ALIGN(16);
   _fdata = .;

  /* data: Writable data */
	.data : {
	   . = ALIGN(4);
	   __data_start = .;
	   *(.data)
	   *(.data.*)
	   *(.gnu.linkonce.d.*)
	   __data_end = .;
	} > dmem

    /* Stack and Heap definitions */
    .heap : {
       . = ALIGN(4);
       _heap = .;
       _heap_start = .;
       . += _HEAP_SIZE;
       _heap_end = .;
    } > dmem

  /* FreeRTOS heap */
  .freertos_heap (NOLOAD):
  {
    . = ALIGN(8);
  } > dmem

    .stack : {
       _stack_end = .;
       . += _STACK_SIZE;
       . = ALIGN(4);
       _stack = .;
       __stack = _stack;
    } > dmem

  /* End of initialized data segment */
  PROVIDE( edata = . );
  _edata = .;

  /* Writable small data segment */
    .sdata : {
        _gp = . + 0x800;
        *(.srodata.cst16) *(.srodata.cst8) *(.srodata.cst4) *(.srodata.cst2) *(.srodata*)
        *(.sdata .sdata.* .gnu.linkonce.s.*)
     }	> dmem

  /*--------------------------------------------------------------------*/
  /* Uninitialized data segment                                         */
  /*--------------------------------------------------------------------*/

  /* Start of uninitialized data segment */
  . = ALIGN(8);
  _fbss = .;

  /* Writable uninitialized small data segment */
	.sbss : {
	   __sbss_start = .;
	   *(.sbss)
	   *(.sbss.*)
	   *(.gnu.linkonce.sb.*)
	   __sbss_end = .;
	} > dmem
	


	.sbss2 : {
	   __sbss2_start = .;
	   *(.sbss2)
	   *(.sbss2.*)
	   *(.gnu.linkonce.sb2.*)
	   __sbss2_end = .;
	} > dmem

    .bss : {
       . = ALIGN(4);
       __bss_start = .;
       *(.bss)
       *(.bss.*)
       *(.gnu.linkonce.b.*)
       *(COMMON)
       . = ALIGN(4);
       __bss_end = .;

    } > dmem  

    /* End of uninitialized data segment */
    PROVIDE( end = . );
    _end = ALIGN(8);

}


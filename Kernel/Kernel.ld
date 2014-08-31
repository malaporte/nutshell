/*******************************************************************************
* Copyright (C) Martin Laporte.
*******************************************************************************/

/*
	TODO: There is a bug with COMDAT support in DJGPP's binary output, I think,
	that causes call to out-of-line functions marked as inline to be mistaken
	when they occur on more than one translation unit.
*/  

SECTIONS
{
	.text 0xC0000000 :
	{
		__BEGINNING_OF_TEXT = .;

		/* The kernel entry point (must be first) */
		Kernel.o(.text)

		/* The remaining of the kernel code */
		*(.text*)
		*(.gnu.linkonce.t*)

		/* Read only data */
	    *(.const*)
	    *(.ro*)
	    *(.gnu.linkonce.r*)

		__END_OF_TEXT = .;
	}
	
	.data ALIGN(8) :
	{
		__BEGINNING_OF_DATA = .;

		/* The global constructors */
		__CTOR_LIST = .;
		*(.ctor*)
		LONG(0)

		/* The global destructors */
		__DTOR_LIST = .;
		*(.dtor*)
		LONG(0)

		/* Initialized data */
		*(.data*)
		*(.rdata*)
	    *(.gnu.linkonce.d*)

		/* Exception information */
	    *(.gcc_exc*)
	    ___EH_FRAME_BEGIN__ = .;
		*(.eh_fram*)
    	___EH_FRAME_END__ = .;
		LONG(0);

		__END_OF_DATA = .;
	}

	.bss ALIGN(8) :
	{
		__BEGINNING_OF_BSS = .;

		/* Uninitialized data */
		*(.bss)
		*(COMMON)

		. = ALIGN(0x1000);
		__END_OF_BSS = .;
	}
}

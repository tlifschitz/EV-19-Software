
ENTRY(_start)


MEMORY
{
	ROM (rx)     : ORIGIN = 0x00000000, LENGTH = 20k
	RAM (rw)    : ORIGIN = 0x02000000, LENGTH = 4k
	SDRAM (rw)   : ORIGIN = 0x04000000, LENGTH = 32M
	
}

SECTIONS
{
	.text :
	{
		*(.init)
		*(.text*)
		__text_end = ALIGN(4);
	} > ROM

	.data :
	{
		__data_begin = .;
		*(.rodata*)
		*(.data*)
		*(.sdata*)
		*(.eh_frame*)
		__data_end = ALIGN(4);
	} > RAM



	.bss :
	{
		__bss_begin = .;
		*(.bss*)
		*(.sbss)
		*(COMMON)
		__bss_end = ALIGN(4);
	} > RAM
	/* Use the top of RAM and downwards for the stack: */
		__stack_top = ORIGIN(RAM)+LENGTH(RAM);
	end = .;

	.extRAM :
	{
		__extRAM_begin = .;
		*(.extRAM*)
		__extRAM_end = ALIGN(4);
	} > SDRAM
	

	/DISCARD/ :
	{
	*(.comment)
	}
}

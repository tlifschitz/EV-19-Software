
.section .init

.global _start
_start:

# Copies the .data from ROM to RAM - this is only used by the bootloader, which runs from ROM:
#ifdef COPY_DATA_TO_RAM
.hidden copy_data
copy_data:
	la x1, __text_end	# Copy source address
	la x2, __data_begin	# Copy destination address
	la x3, __data_end	# Copy destination end address

	beq x2, x3, 2f		# Skip if there is no data to copy

1:
	lb x4, (x1)
	sb x4, (x2)
	addi x1, x1, 1
	addi x2, x2, 1

	bne x2, x3, 1b		# Repeat as long as there is more data to copy
2:
#endif

# Clears the .bss (zero initialized data) section:
.hidden clear_bss
clear_bss:
	la x1, __bss_begin
	la x2, __bss_end
	beq x1, x2, 2f		# Skip if there is no .bss section

1:
	sw x0, (x1)
	addi x1, x1, 4
	bne x1, x2, 1b
2:

# Sets up the stack pointer:
.hidden init_stack
init_stack:
	la sp, __stack_top

.hidden call_main
call_main:
	call main

stall:
	beq x0, x0, stall

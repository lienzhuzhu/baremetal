.global _start

.equ GPIO_BASE, 0x3F200000
.equ GPFSEL2, 0x8 

.equ GPIO_21_OUTPUT, 0x8 

.equ GPFSET0, 0x1c 
.equ GPFCLR0, 0x28 

.equ GPIOVAL, 0x200000 

_start:

	ldr r0, =GPIO_BASE

	ldr r1, =GPIO_21_OUTPUT
	str r1, [r0, #GPFSEL2]

	ldr r2, =0x800000

loop:
	ldr r1, =GPIOVAL 
	str r1, [r0, #GPFSET0] 

	eor r10, r10, r10
	delay1:
		add r10, r10, #1
		cmp r10, r2
		bne, delay1

	ldr r1, =GPIOVAL 
	str r1, [r0, #GPFCLR0] 

	eor r10, r10, r10
	delay2:
		add r10, r10, #1
		cmp r10, r2
		bne delay2

	b loop

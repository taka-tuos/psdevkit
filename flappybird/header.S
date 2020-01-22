	.text
	.code 32

	.globl _start
_start:
	.ascii	"SC"
	.byte	0x11
	.byte	1		// Number of memory card blocks (use "mcpad" tool for auto-adjust)

	// Title 32*2 Shift-JIS zero padded (use "ascii2sjis" tool to generate this file)
	.include "data/title.txt"
	.space	12

	.hword	1		// Number of File Viewer Mono Icon Frames
	.ascii	"MCX0"
	.byte	1		// Number of entries in Executable Mono Icon List
	.byte	0		// No Function Table
	.word	0
	.word	_progstart

IconPS:	.incbin	"data/psicon.bin"	// 16*word Palette + 16x16 4bits Icon
IconPK:	.incbin	"data/icon.bin"		// File Viewer Mono Icon 32x32

	.hword	1		// Number of Executable Mono Icon
	.hword	1		// Icon Anim Speed
	.word	IconPK		// Pointer to the same File Viewer Mono Icon

_progstart:
	b	main

//----------------------------------------
// Bios Calls

	.globl	SetCallbacks
SetCallbacks:
	swi	1
	mov	pc, lr

	.globl	SetCpuSpeed
SetCpuSpeed:
	swi	4
	mov	pc, lr

	.globl	PrepareExecute
PrepareExecute:
	swi	8
	mov	pc, lr

	.globl	DoExecute
DoExecute:
	swi	9

	.globl	SetComOnOff
SetComOnOff:
	swi	17
	mov	pc, lr

	.globl	GetDirIndex
GetDirIndex:
	swi	22
	mov	pc, lr

	.globl	IntIRQ
IntIRQ:
	stmdb	sp!,{r2-r3,lr}
	bl	IRQ_Handler
	ldmia	sp!,{r2-r3,pc}

	.globl	IntFIQ
IntFIQ:
	stmdb	sp!,{r2-r3,lr}
	bl	FIQ_Handler
	ldmia	sp!,{r2-r3,pc}


//----------------------------------------
// Divide Function

	.globl	__aeabi_idiv0
	.globl	__aeabi_idiv
__aeabi_idiv:
	cmp	r0, #0
	bge	L_num_pos
	rsbs	r0, r0, #0 @ num = -num
	cmp	r1, #0
	bge	L_neg_result
	rsbs	r1, r1, #0 @ den = -den
	b	__aeabi_uidivmod
L_num_pos:
	cmp	r1, #0
	bge	__aeabi_uidivmod
	rsbs	r1, r1, #0 @ den = -den
L_neg_result:
	push	{lr}
	bl	__aeabi_uidivmod
	rsbs	r0, r0, #0 @ quot = -quot
	pop	{pc}

	.globl	__aeabi_uidivmod
__aeabi_uidivmod:
	cmp	r1, #0
	bne	L_no_div0
	b	__aeabi_idiv0
L_no_div0:
	@ Shift left the denominator until it is greater than the numerator
	movs	r2, #1 @ counter
	movs	r3, #0 @ result
	cmp	r0, r1
	bls	L_sub_loop0
	adds	r1, #0 @ dont shift if denominator would overflow
	bmi	L_sub_loop0
L_denom_shift_loop:
	lsls	r2, #1
	lsls	r1, #1
	bmi	L_sub_loop0
	cmp	r0, r1
	bhi	L_denom_shift_loop	
L_sub_loop0:	
	cmp	r0, r1
	bcc	L_dont_sub0	@ if (num>denom)
	subs	r0, r1	@ numerator -= denom
	orrs	r3, r2	@ result(r3) |= bitmask(r2)
L_dont_sub0:
	lsrs	r1, #1 @ denom(r1) >>= 1
	lsrs	r2, #1 @ bitmask(r2) >>= 1
	bne	L_sub_loop0
	mov	r1, r0	@ remainder(r1) = numerator(r0)
	mov	r0, r3	@ quotient(r0) = result(r3)
__aeabi_idiv0:
	bx	lr


//----------------------------------------
// User Data

	.globl	TinyFont
TinyFont:	.incbin	"tinyfont.dat"

	.include	"data.s"

	.end

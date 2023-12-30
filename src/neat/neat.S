.syntax unified
.thumb_func
.global neat_init
.global neat_evaluate
.global neat_hash
.global neat_io



.section .text
.type neat_init,%function
neat_init:
	ldr   r1, [r0, #4]
	adds  r0, r0, #8
	lsrs  r2, r1, #24
	lsls  r1, r1, #8
	lsrs  r1, r1, #6
1:
	subs  r1, r1, #4
	ldr   r3, [r0, r1]
	eors  r2, r3
	cmp   r1, #0
	bne   1b
	lsrs  r3, r2, #16
	eors  r2, r3
	lsrs  r3, r2, #8
	eors  r2, r3
	lsls  r2, r2, #24
	lsrs  r2, r2, #24
	cmp   r2, #0
	bne   2f
	subs  r0, r0, #8
	ldr   r1, [r0, #0]
	ldr   r2, neat_init._data+16
	str   r1, [r2, #0]
	ldr   r1, [r0, #8]
	adds  r0, r0, #12
	ldr   r2, neat_init._data+12
	str   r0, [r2, #0]
	lsls  r0, r1, #1
	lsrs  r0, r0, #17
	lsls  r0, r0, #1
	ldr   r2, neat_init._data+8
	str   r0, [r2, #0]
	lsls  r0, r1, #24
	lsrs  r0, r0, #23
	ldr   r2, neat_init._data
	str   r0, [r2, #0]
	lsls  r0, r1, #16
	lsrs  r0, r0, #24
	lsls  r0, r0, #1
	lsrs  r1, r1, #31
	lsls  r1, r1, #31
	orrs  r0, r1
	ldr   r2, neat_init._data+4
	str   r0, [r2, #0]
	movs  r0, #1
	bx    lr
2:
	movs  r0, #0
	bx    lr
	.align 4
neat_init._data:
	.word _neat_data
	.word _neat_data+4
	.word _neat_data+8
	.word _neat_data+12
	.word neat_hash



.type neat_evaluate,%function
neat_evaluate:
	push  {r4, r5, r6, r7, lr}
	sub   sp, sp, #20
	ldr   r0, neat_evaluate._data+8
	ldr   r1, neat_evaluate._data+40
	ldr   r2, neat_evaluate._data
	ldr   r2, [r2, #0]
	ldr   r5, neat_evaluate._data+44
	ldr   r6, neat_evaluate._data+48
	ldr   r7, neat_evaluate._data+52
	mov   ip, r5
1:
	subs  r2, r2, #2
	lsls  r3, r2, #1
	ldr   r3, [r1, r3]
	lsrs  r4, r3, #11
	ands  r4, r6
	lsrs  r5, r3, #31
	add   r5, r5, ip
	lsls  r3, r3, #9
	adds  r4, r4, r5
	lsrs  r3, r3, #22
	lsls  r3, r3, #1
	ldrh  r3, [r3, r7]
	adds  r3, r4, r3
	mvns  r4, r3
	asrs  r4, r4, #31
	ands  r3, r4
	strh  r3, [r0, r2]
	cmp   r2, #0
	bne   1b
	ldr   r1, neat_evaluate._data
	ldr   r1, [r1, #0]
	movs  r2, #2
	movs  r3, #0
	movs  r4, r1
	ldr   r5, neat_evaluate._data+4
	ldr   r5, [r5, #0]
	tst   r1, r2
	beq   2f
	strh  r3, [r0, r1]
	adds  r1, r1, #2
	cmp   r1, r5
	beq   3f
2:
	str   r3, [r0, r1]
	adds  r1, r1, #4
	cmp   r1, r5
	bne   2b
3:
	ldr   r1, neat_evaluate._data+12
	ldr   r1, [r1, #0]
	ldr   r6, neat_evaluate._data+20
	mov   ip, r6
	ldr   r6, neat_evaluate._data+24
	ldr   r7, neat_evaluate._data+28
	str   r6, [sp, #0]
	str   r7, [sp, #4]
	str   r5, [sp, #12]
4:
	adds  r2, r4, #2
	lsrs  r2, r2, #2
	lsls  r2, r2, #2
	str   r2, [sp, #16]
	ldrh  r3, [r1, r2]
	subs  r2, r2, #4
	str   r4, [sp, #8]
	ldr   r4, [sp, #0]
	ldr   r5, [sp, #4]
5:
	ldr   r6, [r0, r2]
	ldr   r7, [r1, r2]
	subs  r2, r2, #4
	add   r6, r6, r7
	lsrs  r7, r6, #16
	uxth  r6, r6
	subs  r7, r6, r7
	add   r7, r7, ip
	lsls  r7, r7, #1
	ldrsh r7, [r5, r7]
	add   r6, r6, r7
	add   r6, r4, r6
	mvns  r7, r6
	asrs  r7, r7, #31
	ands  r6, r7
	subs  r6, r3, r6
	add   r6, r6, ip
	lsls  r6, r6, #1
	ldrsh r6, [r5, r6]
	add   r3, r3, r6
	add   r3, r3, r4
	mvns  r6, r3
	asrs  r6, r6, #31
	ands  r3, r6
	adds  r6, r2, #4
	bne   5b
	ldr   r4, [sp, #8]
	ldr   r5, [sp, #12]
	ldr   r6, [sp, #16]
	adds  r1, r1, r6
	ldr   r6, neat_evaluate._data+16
	ldrh  r7, [r1, #2]
	adds  r6, r6, r7
	adds  r1, r1, #4
	lsrs  r7, r3, #4
	lsls  r3, r3, #12
	orrs  r3, r7
	lsls  r3, r3, #19
	lsrs  r3, r3, #18
	ldrh  r3, [r6, r3]
	strh  r3, [r0, r4]
	adds  r4, r4, #2
	cmp   r4, r5
	bne   4b
	ldr   r1, neat_evaluate._data+32
	ldr   r1, [r1, #0]
	ldr   r4, neat_evaluate._data+36
	ldr   r6, neat_evaluate._data+40
	adds  r0, r0, r5
	lsrs  r2, r1, #31
	lsls  r2, r2, #1
	subs  r0, r0, r2
	lsls  r1, r1, #1
	lsrs  r1, r1, #1
	subs  r0, r0, r1
6:
	subs  r1, r1, #2
	ldrh  r2, [r0, r1]
	lsrs  r3, r2, #4
	lsls  r3, r3, #2
	ldr   r3, [r4, r3]
	lsls  r2, r2, #31
	orrs  r2, r3
	lsls  r3, r1, #1
	str   r2, [r6, r3]
	cmp   r1, #0
	bne   6b
	add   sp, sp, #20
	pop   {r4, r5, r6, r7, pc}
	.align 4
neat_evaluate._data:
	.word _neat_data
	.word _neat_data+8
	.word _neat_data+16
	.word _neat_data+12
	.word _neat_tables+292864
	.word 0x0000ffff
	.word 0x00003a7a
	.word _neat_tables
	.word _neat_data+4
	.word _neat_tables+262144
	.word neat_io
	.word 0xfff85000
	.word 0x000ff000
	.word _neat_tables+290816



.section .bss
	.align 4
_neat_data:
	.space 4096
neat_hash:
	.space 4
neat_io:
	.space 256

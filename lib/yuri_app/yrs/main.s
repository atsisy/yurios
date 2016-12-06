	.file	"main.c"
	.text
	.globl	strcmp
	.type	strcmp, @function
strcmp:
.LFB0:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$16, %esp
	movl	$0, -4(%ebp)
	jmp	.L2
.L5:
	movl	-4(%ebp), %edx
	movl	8(%ebp), %eax
	addl	%edx, %eax
	movzbl	(%eax), %edx
	movl	-4(%ebp), %ecx
	movl	12(%ebp), %eax
	addl	%ecx, %eax
	movzbl	(%eax), %eax
	cmpb	%al, %dl
	je	.L3
	movl	$0, %eax
	jmp	.L4
.L3:
	addl	$1, -4(%ebp)
.L2:
	movl	-4(%ebp), %edx
	movl	8(%ebp), %eax
	addl	%edx, %eax
	movzbl	(%eax), %eax
	testb	%al, %al
	jne	.L5
	movl	$0, -4(%ebp)
	jmp	.L6
.L8:
	movl	-4(%ebp), %edx
	movl	8(%ebp), %eax
	addl	%edx, %eax
	movzbl	(%eax), %edx
	movl	-4(%ebp), %ecx
	movl	12(%ebp), %eax
	addl	%ecx, %eax
	movzbl	(%eax), %eax
	cmpb	%al, %dl
	je	.L7
	movl	$0, %eax
	jmp	.L4
.L7:
	addl	$1, -4(%ebp)
.L6:
	movl	-4(%ebp), %edx
	movl	12(%ebp), %eax
	addl	%edx, %eax
	movzbl	(%eax), %eax
	testb	%al, %al
	jne	.L8
	movl	$1, %eax
.L4:
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE0:
	.size	strcmp, .-strcmp
	.section	.rodata
.LC0:
	.string	"quit"
	.text
	.globl	main
	.type	main, @function
main:
.LFB1:
	.cfi_startproc
	leal	4(%esp), %ecx
	.cfi_def_cfa 1, 0
	andl	$-16, %esp
	pushl	-4(%ecx)
	pushl	%ebp
	.cfi_escape 0x10,0x5,0x2,0x75,0
	movl	%esp, %ebp
	pushl	%ecx
	.cfi_escape 0xf,0x3,0x75,0x7c,0x6
	subl	$1028, %esp
.L12:
	subl	$4, %esp
	pushl	$1024
	leal	-1032(%ebp), %eax
	pushl	%eax
	pushl	$-1
	call	read
	addl	$16, %esp
	subl	$8, %esp
	pushl	$.LC0
	leal	-1032(%ebp), %eax
	pushl	%eax
	call	strcmp
	addl	$16, %esp
	testl	%eax, %eax
	jne	.L14
	jmp	.L12
.L14:
	nop
	call	exit
	nop
	movl	-4(%ebp), %ecx
	.cfi_def_cfa 1, 0
	leave
	.cfi_restore 5
	leal	-4(%ecx), %esp
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE1:
	.size	main, .-main
	.ident	"GCC: (GNU) 6.2.0"
	.section	.note.GNU-stack,"",@progbits

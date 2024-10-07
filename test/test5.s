.section .text
.global _start

	# ASM_funcSignature
_start:
	pushq %rbp
	movq %rsp, %rbp



	# ASM_Var
	subq $8, %rsp
	movq $3, %rax
	pushq %rax
	popq %rax
	movq %rax, -8(%rbp)



	# ASM_Var_CALL
	movq -8(%rbp), %rax
	movq %rax, %rdi
	call factorial


	# ASM_Var
	subq $8, %rsp
	movq %rax, -16(%rbp)



	# ASM_CALL
	movq -16(%rbp), %rax
	movq %rax, %rdi
	call int_to_string




	# End block
	movq $60, %rax
	movq $0, %rdi
	syscall


	# ASM_funcSignature
factorial:
	pushq %rbp
	movq %rsp, %rbp


	subq $8, %rsp
	movq %rdi, -8(%rbp)

	# ASM_If
	movq -8(%rbp), %rax
	pushq %rax
	movq $1, %rax
	pushq %rax
	popq %rdi
	popq %rax
	cmpq %rdi, %rax
	jne if_end_1


	# ASM_Return
	movq -8(%rbp), %rax
	movq %rbp, %rsp
	popq %rbp
	ret
	# ASM_END_if
if_end_1:


	# ASM_Var
	subq $8, %rsp
	movq -8(%rbp), %rax
	pushq %rax
	movq $1, %rax
	pushq %rax
	popq %rcx
	popq %rdi
	subq %rcx, %rdi
	pushq %rdi

	popq %rax
	movq %rax, -16(%rbp)



	# ASM_Var_CALL
	movq -16(%rbp), %rax
	movq %rax, %rdi
	call factorial


	# ASM_Var
	subq $8, %rsp
	movq %rax, -24(%rbp)



	# ASM_Assignment
	movq -24(%rbp), %rax
	pushq %rax
	movq -8(%rbp), %rax
	pushq %rax
	popq %rdi
	popq %rax
	imulq %rdi
	pushq %rax

	popq %rax
	movq %rax, -24(%rbp)



	# ASM_Return
	movq -24(%rbp), %rax

	movq %rbp, %rsp
	popq %rbp
	ret



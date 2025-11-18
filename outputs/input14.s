.data
print_fmt: .string "%ld \n"
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $16, %rsp
 movq $1, %rax
 pushq %rax
 movq $2, %rax
 movq %rax, %rcx
 popq %rax
 cmpq %rcx, %rax
 movl $0, %eax
 setl %al
 movzbq %al, %rax
 cmpq $0, %rax
 je else_0
 movq $3, %rax
 pushq %rax
 movq $1, %rax
 movq %rax, %rcx
 popq %rax
 cmpq %rcx, %rax
 movl $0, %eax
 setg %al
 movzbq %al, %rax
 cmpq $0, %rax
 je else_1
 movq $4, %rax
 pushq %rax
 movq $4, %rax
 movq %rax, %rcx
 popq %rax
 cmpq %rcx, %rax
 movl $0, %eax
 sete %al
 movzbq %al, %rax
 cmpq $0, %rax
 je else_2
 movq $999, %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 jmp endif_2
 else_2:
endif_2:
 jmp endif_1
 else_1:
endif_1:
 jmp endif_0
 else_0:
endif_0:
.end_main:
leave
ret
.section .note.GNU-stack,"",@progbits

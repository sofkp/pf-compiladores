.data
print_fmt: .string "%ld \n"
B: .quad 3
A: .quad 10
.text
.globl cuadrado
cuadrado:
 pushq %rbp
 movq %rsp, %rbp
 subq $16, %rsp
 movq %rdi, -8(%rbp)
 movq -8(%rbp), %rax
 pushq %rax
 movq -8(%rbp), %rax
 movq %rax, %rcx
 popq %rax
 imulq %rcx, %rax
 jmp .end_cuadrado
.end_cuadrado:
leave
ret
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $32, %rsp
 movq $0, %rax
 movq %rax, -8(%rbp)
 movq $1, %rax
 movq %rax, -16(%rbp)
for_0:
 movq $5, %rax
 movq %rax, %rcx
 movq -16(%rbp), %rax
 cmpq %rcx, %rax
 movl $0, %eax
 setle %al
 movzbq %al, %rax
 cmpq $0, %rax
 je endfor_0
 movq -8(%rbp), %rax
 pushq %rax
 movq -16(%rbp), %rax
 movq %rax, %rdi
call cuadrado
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
 movq %rax, -8(%rbp)
 movq -16(%rbp), %rax
 addq $1, %rax
 movq %rax, -16(%rbp)
 jmp for_0
endfor_0:
 movq -8(%rbp), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq -8(%rbp), %rax
 pushq %rax
 movq $50, %rax
 movq %rax, %rcx
 popq %rax
 cmpq %rcx, %rax
 movl $0, %eax
 setge %al
 movzbq %al, %rax
 cmpq $0, %rax
 je else_1
 movq A(%rip), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 jmp endif_1
 else_1:
 movq B(%rip), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
endif_1:
.end_main:
leave
ret
.section .note.GNU-stack,"",@progbits

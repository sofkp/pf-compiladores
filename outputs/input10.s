.data
print_fmt: .string "%ld \n"
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $16, %rsp
 movq $0, %rax
 movq %rax, -8(%rbp)
for_0:
 movq $5, %rax
 movq %rax, %rcx
 movq -8(%rbp), %rax
 cmpq %rcx, %rax
 movl $0, %eax
 setl %al
 movzbq %al, %rax
 cmpq $0, %rax
 je endfor_0
 movq -8(%rbp), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq -8(%rbp), %rax
 addq $1, %rax
 movq %rax, -8(%rbp)
 jmp for_0
endfor_0:
.end_main:
leave
ret
.section .note.GNU-stack,"",@progbits

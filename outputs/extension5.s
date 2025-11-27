.data
print_int_fmt: .string "%ld\n"
print_str_fmt: .string "%s\n"
.text
.globl combinar
combinar:
 pushq %rbp
 movq %rsp, %rbp
 subq $32, %rsp
 movq %rdi, -8(%rbp)
 movq %rsi, -16(%rbp)
 movq $8, %rdi
 call malloc@PLT
 movq %rax, %r11
 movq $48, %rdi
 call malloc@PLT
 movq %rax, %r10
 movq -8(%rbp), %rax
 movq %rax, %rdx
 movq 0(%rdx), %rax
 movq %rax, %rdx
 movq $0, %rax
 imulq $8, %rax
 addq %rax, %rdx
 movq (%rdx), %rax
 pushq %rax
 movq -16(%rbp), %rax
 movq %rax, %rdx
 movq 0(%rdx), %rax
 movq %rax, %rdx
 movq $0, %rax
 imulq $8, %rax
 addq %rax, %rdx
 movq (%rdx), %rax
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
 movq %rax, 0(%r10)
 movq -8(%rbp), %rax
 movq %rax, %rdx
 movq 0(%rdx), %rax
 movq %rax, %rdx
 movq $1, %rax
 imulq $8, %rax
 addq %rax, %rdx
 movq (%rdx), %rax
 pushq %rax
 movq -16(%rbp), %rax
 movq %rax, %rdx
 movq 0(%rdx), %rax
 movq %rax, %rdx
 movq $1, %rax
 imulq $8, %rax
 addq %rax, %rdx
 movq (%rdx), %rax
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
 movq %rax, 8(%r10)
 movq -8(%rbp), %rax
 movq %rax, %rdx
 movq 0(%rdx), %rax
 movq %rax, %rdx
 movq $2, %rax
 imulq $8, %rax
 addq %rax, %rdx
 movq (%rdx), %rax
 pushq %rax
 movq -16(%rbp), %rax
 movq %rax, %rdx
 movq 0(%rdx), %rax
 movq %rax, %rdx
 movq $2, %rax
 imulq $8, %rax
 addq %rax, %rdx
 movq (%rdx), %rax
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
 movq %rax, 16(%r10)
 movq -8(%rbp), %rax
 movq %rax, %rdx
 movq 0(%rdx), %rax
 movq %rax, %rdx
 movq $3, %rax
 imulq $8, %rax
 addq %rax, %rdx
 movq (%rdx), %rax
 pushq %rax
 movq -16(%rbp), %rax
 movq %rax, %rdx
 movq 0(%rdx), %rax
 movq %rax, %rdx
 movq $3, %rax
 imulq $8, %rax
 addq %rax, %rdx
 movq (%rdx), %rax
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
 movq %rax, 24(%r10)
 movq -8(%rbp), %rax
 movq %rax, %rdx
 movq 0(%rdx), %rax
 movq %rax, %rdx
 movq $4, %rax
 imulq $8, %rax
 addq %rax, %rdx
 movq (%rdx), %rax
 pushq %rax
 movq -16(%rbp), %rax
 movq %rax, %rdx
 movq 0(%rdx), %rax
 movq %rax, %rdx
 movq $4, %rax
 imulq $8, %rax
 addq %rax, %rdx
 movq (%rdx), %rax
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
 movq %rax, 32(%r10)
 movq -8(%rbp), %rax
 movq %rax, %rdx
 movq 0(%rdx), %rax
 movq %rax, %rdx
 movq $5, %rax
 imulq $8, %rax
 addq %rax, %rdx
 movq (%rdx), %rax
 pushq %rax
 movq -16(%rbp), %rax
 movq %rax, %rdx
 movq 0(%rdx), %rax
 movq %rax, %rdx
 movq $5, %rax
 imulq $8, %rax
 addq %rax, %rdx
 movq (%rdx), %rax
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
 movq %rax, 40(%r10)
 movq %r10, %rax
 movq %rax, 0(%r11)
 movq %r11, %rax
 movq %rax, -24(%rbp)
 movq -24(%rbp), %rax
 jmp .Lend_combinar
.Lend_combinar:
 leave
 ret
.globl analizar
analizar:
 pushq %rbp
 movq %rsp, %rbp
 subq $48, %rsp
 movq %rdi, -8(%rbp)
 movq $0, %rax
 movq %rax, -16(%rbp)
 movq $0, %rax
 movq %rax, -24(%rbp)
.Lfor_0:
 movq $6, %rax
 movq %rax, %rcx
 movq -24(%rbp), %rax
 cmpq %rcx, %rax
 jge .Lendfor_0
 movq -16(%rbp), %rax
 pushq %rax
 movq -8(%rbp), %rax
 movq %rax, %rdx
 movq 0(%rdx), %rax
 movq %rax, %rdx
 movq -24(%rbp), %rax
 imulq $8, %rax
 addq %rax, %rdx
 movq (%rdx), %rax
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
 pushq %rax
 leaq -16(%rbp), %rax
 movq %rax, %rdx
 popq %rax
 movq %rax, (%rdx)
 movq -24(%rbp), %rax
 addq $1, %rax
 movq %rax, -24(%rbp)
 jmp .Lfor_0
.Lendfor_0:
 movq -16(%rbp), %rax
 pushq %rax
 movq $6, %rax
 movq %rax, %rcx
 popq %rax
 cqto
 idivq %rcx
 movq %rax, -32(%rbp)
 movq $16, %rdi
 call malloc@PLT
 movq %rax, %r11
 movq -16(%rbp), %rax
 movq %rax, 0(%r11)
 movq -32(%rbp), %rax
 movq %rax, 8(%r11)
 movq %r11, %rax
 movq %rax, -40(%rbp)
 movq -40(%rbp), %rax
 jmp .Lend_analizar
.Lend_analizar:
 leave
 ret
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $32, %rsp
 movq $8, %rdi
 call malloc@PLT
 movq %rax, %r11
 movq $48, %rdi
 call malloc@PLT
 movq %rax, %r10
 movq $10, %rax
 movq %rax, 0(%r10)
 movq $20, %rax
 movq %rax, 8(%r10)
 movq $15, %rax
 movq %rax, 16(%r10)
 movq $30, %rax
 movq %rax, 24(%r10)
 movq $25, %rax
 movq %rax, 32(%r10)
 movq $18, %rax
 movq %rax, 40(%r10)
 movq %r10, %rax
 movq %rax, 0(%r11)
 movq %r11, %rax
 movq %rax, -8(%rbp)
 movq $8, %rdi
 call malloc@PLT
 movq %rax, %r11
 movq $48, %rdi
 call malloc@PLT
 movq %rax, %r10
 movq $5, %rax
 movq %rax, 0(%r10)
 movq $18, %rax
 movq %rax, 8(%r10)
 movq $12, %rax
 movq %rax, 16(%r10)
 movq $22, %rax
 movq %rax, 24(%r10)
 movq $35, %rax
 movq %rax, 32(%r10)
 movq $10, %rax
 movq %rax, 40(%r10)
 movq %r10, %rax
 movq %rax, 0(%r11)
 movq %r11, %rax
 movq %rax, -16(%rbp)
 movq -8(%rbp), %rax
 movq %rax, %rdi
 movq -16(%rbp), %rax
 movq %rax, %rsi
 call combinar
 movq %rax, -24(%rbp)
 movq -24(%rbp), %rax
 movq %rax, %rdi
 call analizar
 movq %rax, -32(%rbp)
 movq -32(%rbp), %rax
 movq %rax, %rdx
 movq 0(%rdx), %rax
 movq %rax, %rsi
 leaq print_int_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq -32(%rbp), %rax
 movq %rax, %rdx
 movq 8(%rdx), %rax
 movq %rax, %rsi
 leaq print_int_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.Lend_main:
 leave
 ret
.section .note.GNU-stack,"",@progbits

.data
print_int_fmt: .string "%ld\n"
.text
.globl buildarr
buildarr:
 pushq %rbp
 movq %rsp, %rbp
 subq $32, %rsp
 movq %rdi, -8(%rbp)
 movq %rsi, -16(%rbp)
 movq %rdx, -24(%rbp)
 movq $24, %rdi
 call malloc@PLT
 movq %rax, %r10
 movq -8(%rbp), %rax
 movq %rax, 0(%r10)
 movq -16(%rbp), %rax
 movq %rax, 8(%r10)
 movq -24(%rbp), %rax
 movq %rax, 16(%r10)
 movq %r10, %rax
 movq %rax, -32(%rbp)
 movq -32(%rbp), %rax
 jmp .Lend_buildarr
.Lend_buildarr:
 leave
 ret
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $16, %rsp
 movq $3, %rax
 movq %rax, %rdi
 movq $4, %rax
 movq %rax, %rsi
 movq $5, %rax
 movq %rax, %rdx
 call buildarr
 movq %rax, -8(%rbp)
 movq -8(%rbp), %rax
 movq %rax, %rdx
 movq $0, %rax
 imulq $8, %rax
 addq %rax, %rdx
 movq (%rdx), %rax
 movq %rax, %rsi
 leaq print_int_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq -8(%rbp), %rax
 movq %rax, %rdx
 movq $1, %rax
 imulq $8, %rax
 addq %rax, %rdx
 movq (%rdx), %rax
 movq %rax, %rsi
 leaq print_int_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq -8(%rbp), %rax
 movq %rax, %rdx
 movq $2, %rax
 imulq $8, %rax
 addq %rax, %rdx
 movq (%rdx), %rax
 movq %rax, %rsi
 leaq print_int_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.Lend_main:
 leave
 ret
.section .note.GNU-stack,"",@progbits

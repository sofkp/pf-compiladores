.data
print_int_fmt: .string "%ld\n"
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $16, %rsp
 pushq %r10
 subq $32, %rsp
 movq %rsp, %rax
 movq %rax, %r10
 movq $1, %rax
 movq %rax, 0(%r10)
 movq $2, %rax
 movq %rax, 8(%r10)
 movq $3, %rax
 movq %rax, 16(%r10)
 movq $4, %rax
 movq %rax, 24(%r10)
 movq %r10, %rax
 popq %r10
 movq %rax, -8(%rbp)
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
 movq $99, %rax
 pushq %rax
 movq -8(%rbp), %rax
 movq %rax, %rdx
 movq $2, %rax
 imulq $8, %rax
 addq %rax, %rdx
 movq %rdx, %rax
 movq %rax, %rdx
 popq %rax
 movq %rax, (%rdx)
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

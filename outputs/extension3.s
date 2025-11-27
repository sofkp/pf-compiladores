.data
print_int_fmt: .string "%ld\n"
print_str_fmt: .string "%s\n"
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $16, %rsp
 movq $8, %rdi
 call malloc@PLT
 movq %rax, %r11
 movq $24, %rdi
 call malloc@PLT
 movq %rax, %r10
 movq $15, %rax
 movq %rax, 0(%r10)
 movq $18, %rax
 movq %rax, 8(%r10)
 movq $20, %rax
 movq %rax, 16(%r10)
 movq %r10, %rax
 movq %rax, 0(%r11)
 movq %r11, %rax
 movq %rax, -8(%rbp)
 movq -8(%rbp), %rax
 movq %rax, %rdx
 movq 0(%rdx), %rax
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
 movq 0(%rdx), %rax
 movq %rax, %rdx
 movq $1, %rax
 imulq $8, %rax
 addq %rax, %rdx
 movq (%rdx), %rax
 pushq %rax
 movq -8(%rbp), %rax
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
 pushq %rax
 movq -8(%rbp), %rax
 movq %rax, %rdx
 movq 0(%rdx), %rax
 movq %rax, %rdx
 movq $0, %rax
 imulq $8, %rax
 addq %rax, %rdx
 movq %rdx, %rax
 movq %rax, %rdx
 popq %rax
 movq %rax, (%rdx)
 movq -8(%rbp), %rax
 movq %rax, %rdx
 movq 0(%rdx), %rax
 movq %rax, %rdx
 movq $0, %rax
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

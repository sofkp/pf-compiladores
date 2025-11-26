.data
print_int_fmt: .string "%ld\n"
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $16, %rsp
 pushq %r10
 subq $16, %rsp
 movq %rsp, %rax
 movq %rax, %r10
 movq $20, %rax
 movq %rax, 0(%r10)
 movq $12345, %rax
 movq %rax, 8(%r10)
 movq %r10, %rax
 popq %r10
 movq %rax, -8(%rbp)
 movq -8(%rbp), %rax
 movq %rax, %rdx
 movq 0(%rdx), %rax
 movq %rax, %rsi
 leaq print_int_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq $21, %rax
 pushq %rax
 movq -8(%rbp), %rax
 movq %rax, %rdx
 leaq 0(%rdx), %rax
 movq %rax, %rdx
 popq %rax
 movq %rax, (%rdx)
 movq -8(%rbp), %rax
 movq %rax, %rdx
 movq 0(%rdx), %rax
 movq %rax, %rsi
 leaq print_int_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.Lend_main:
 leave
 ret
.section .note.GNU-stack,"",@progbits

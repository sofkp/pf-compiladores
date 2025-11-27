.data
print_int_fmt: .string "%ld\n"
print_str_fmt: .string "%s\n"
.text
.globl mkprod
mkprod:
 pushq %rbp
 movq %rsp, %rbp
 subq $32, %rsp
 movq %rdi, -8(%rbp)
 movq %rsi, -16(%rbp)
 movq %rdx, -24(%rbp)
 movq $24, %rdi
 call malloc@PLT
 movq %rax, %r11
 movq -8(%rbp), %rax
 movq %rax, 0(%r11)
 movq -16(%rbp), %rax
 movq %rax, 8(%r11)
 movq -24(%rbp), %rax
 movq %rax, 16(%r11)
 movq %r11, %rax
 movq %rax, -32(%rbp)
 movq -32(%rbp), %rax
 jmp .Lend_mkprod
.Lend_mkprod:
 leave
 ret
.globl total
total:
 pushq %rbp
 movq %rsp, %rbp
 subq $16, %rsp
 movq %rdi, -8(%rbp)
 movq -8(%rbp), %rax
 movq %rax, %rdx
 movq 8(%rdx), %rax
 pushq %rax
 movq -8(%rbp), %rax
 movq %rax, %rdx
 movq 16(%rdx), %rax
 movq %rax, %rcx
 popq %rax
 imulq %rcx, %rax
 jmp .Lend_total
.Lend_total:
 leave
 ret
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $32, %rsp
 movq $1001, %rax
 movq %rax, %rdi
 movq $25, %rax
 movq %rax, %rsi
 movq $10, %rax
 movq %rax, %rdx
 call mkprod
 movq %rax, -8(%rbp)
 movq $2001, %rax
 movq %rax, %rdi
 movq $40, %rax
 movq %rax, %rsi
 movq $3, %rax
 movq %rax, %rdx
 call mkprod
 movq %rax, -16(%rbp)
 movq -8(%rbp), %rax
 movq %rax, %rdi
 call total
 movq %rax, %rsi
 leaq print_int_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq -16(%rbp), %rax
 movq %rax, %rdi
 call total
 movq %rax, %rsi
 leaq print_int_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq $24, %rdi
 call malloc@PLT
 movq %rax, %r10
 movq -8(%rbp), %rax
 movq %rax, %rdi
 call total
 movq %rax, 0(%r10)
 movq -16(%rbp), %rax
 movq %rax, %rdi
 call total
 movq %rax, 8(%r10)
 movq $5, %rax
 movq %rax, 16(%r10)
 movq %r10, %rax
 movq %rax, -24(%rbp)
 movq -24(%rbp), %rax
 movq %rax, %rdx
 movq $0, %rax
 imulq $8, %rax
 addq %rax, %rdx
 movq (%rdx), %rax
 movq %rax, %rsi
 leaq print_int_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq -24(%rbp), %rax
 movq %rax, %rdx
 movq $1, %rax
 imulq $8, %rax
 addq %rax, %rdx
 movq (%rdx), %rax
 movq %rax, %rsi
 leaq print_int_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq -24(%rbp), %rax
 movq %rax, %rdx
 movq $0, %rax
 imulq $8, %rax
 addq %rax, %rdx
 movq (%rdx), %rax
 pushq %rax
 movq -24(%rbp), %rax
 movq %rax, %rdx
 movq $1, %rax
 imulq $8, %rax
 addq %rax, %rdx
 movq (%rdx), %rax
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
 pushq %rax
 movq -24(%rbp), %rax
 movq %rax, %rdx
 movq $2, %rax
 imulq $8, %rax
 addq %rax, %rdx
 movq %rdx, %rax
 movq %rax, %rdx
 popq %rax
 movq %rax, (%rdx)
 movq -24(%rbp), %rax
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

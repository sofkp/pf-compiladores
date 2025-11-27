.data
print_int_fmt: .string "%ld\n"
print_str_fmt: .string "%s\n"
.text
.globl makepair
makepair:
 pushq %rbp
 movq %rsp, %rbp
 subq $32, %rsp
 movq %rdi, -8(%rbp)
 movq %rsi, -16(%rbp)
 movq $16, %rdi
 call malloc@PLT
 movq %rax, %r11
 movq -8(%rbp), %rax
 movq %rax, 0(%r11)
 movq -16(%rbp), %rax
 movq %rax, 8(%r11)
 movq %r11, %rax
 movq %rax, -24(%rbp)
 movq -24(%rbp), %rax
 jmp .Lend_makepair
.Lend_makepair:
 leave
 ret
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $16, %rsp
 movq $10, %rax
 movq %rax, %rdi
 movq $20, %rax
 movq %rax, %rsi
 call makepair
 movq %rax, -8(%rbp)
 movq -8(%rbp), %rax
 movq %rax, %rdx
 movq 0(%rdx), %rax
 movq %rax, %rsi
 leaq print_int_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq -8(%rbp), %rax
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

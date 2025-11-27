.data
print_int_fmt: .string "%ld\n"
print_str_fmt: .string "%s\n"
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $16, %rsp
 movq $16, %rdi
 call malloc@PLT
 movq %rax, %r11
.section .rodata
.LC0: .string "gato"
.text
 leaq .LC0(%rip), %rax
 movq %rax, 0(%r11)
 movq $2, %rax
 movq %rax, 8(%r11)
 movq %r11, %rax
 movq %rax, -8(%rbp)
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

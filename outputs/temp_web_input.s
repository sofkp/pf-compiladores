.data
print_int_fmt: .string "%ld\n"
print_str_fmt: .string "%s\n"
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $16, %rsp
 movq $50, %rax
 movq %rax, -8(%rbp)
 movq $2, %rax
 movq %rax, %rsi
 leaq print_int_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.Lend_main:
 leave
 ret
.section .note.GNU-stack,"",@progbits

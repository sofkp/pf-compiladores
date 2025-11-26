.data
print_int_fmt: .string "%ld\n"
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $16, %rsp
 movq $0, %rax
 movq %rax, -8(%rbp)
.Lfor_0:
 movq $3, %rax
 movq %rax, %rcx
 movq -8(%rbp), %rax
 cmpq %rcx, %rax
 jg .Lendfor_0
 movq -8(%rbp), %rax
 movq %rax, %rsi
 leaq print_int_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq -8(%rbp), %rax
 addq $1, %rax
 movq %rax, -8(%rbp)
 jmp .Lfor_0
.Lendfor_0:
.Lend_main:
 leave
 ret
.section .note.GNU-stack,"",@progbits

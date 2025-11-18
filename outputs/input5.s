.data
print_fmt: .string "%ld \n"
.text
.globl hola
hola:
 pushq %rbp
 movq %rsp, %rbp
 subq $16, %rsp
 movq $123, %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.end_hola:
leave
ret
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $16, %rsp
call hola
.end_main:
leave
ret
.section .note.GNU-stack,"",@progbits

.data
print_fmt: .string "%ld \n"
.text
.globl int
int:
 pushq %rbp
 movq %rsp, %rbp
 subq $16, %rsp
 movq 0(%rbp), %rax
 mov %rax, %rdi
call suma
.end_int:
leave
ret
.section .note.GNU-stack,"",@progbits

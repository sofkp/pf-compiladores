.data
print_int_fmt: .string "%ld\n"
print_str_fmt: .string "%s\n"
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $16, %rsp
 movq $24, %rdi
 call malloc@PLT
 movq %rax, %r10
.section .rodata
.LC0: .string "hola"
.text
 leaq .LC0(%rip), %rax
 movq %rax, 0(%r10)
.section .rodata
.LC1: .string "mundo"
.text
 leaq .LC1(%rip), %rax
 movq %rax, 8(%r10)
.section .rodata
.LC2: .string "utec"
.text
 leaq .LC2(%rip), %rax
 movq %rax, 16(%r10)
 movq %r10, %rax
 movq %rax, -8(%rbp)
.section .rodata
.LC3: .string "palabra 1"
.text
 leaq .LC3(%rip), %rax
 movq %rax, %rsi
 leaq print_str_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq -8(%rbp), %rax
 movq %rax, %rdx
 movq $0, %rax
 imulq $8, %rax
 addq %rax, %rdx
 movq (%rdx), %rax
 movq %rax, %rsi
 leaq print_str_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.section .rodata
.LC4: .string "palabra 2"
.text
 leaq .LC4(%rip), %rax
 movq %rax, %rsi
 leaq print_str_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq -8(%rbp), %rax
 movq %rax, %rdx
 movq $1, %rax
 imulq $8, %rax
 addq %rax, %rdx
 movq (%rdx), %rax
 movq %rax, %rsi
 leaq print_str_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.section .rodata
.LC5: .string "palabra 3"
.text
 leaq .LC5(%rip), %rax
 movq %rax, %rsi
 leaq print_str_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq -8(%rbp), %rax
 movq %rax, %rdx
 movq $2, %rax
 imulq $8, %rax
 addq %rax, %rdx
 movq (%rdx), %rax
 movq %rax, %rsi
 leaq print_str_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.section .rodata
.LC6: .string "compiladores"
.text
 leaq .LC6(%rip), %rax
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
.section .rodata
.LC7: .string "nueva palabra 3"
.text
 leaq .LC7(%rip), %rax
 movq %rax, %rsi
 leaq print_str_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq -8(%rbp), %rax
 movq %rax, %rdx
 movq $2, %rax
 imulq $8, %rax
 addq %rax, %rdx
 movq (%rdx), %rax
 movq %rax, %rsi
 leaq print_str_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.Lend_main:
 leave
 ret
.section .note.GNU-stack,"",@progbits

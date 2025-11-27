.data
print_int_fmt: .string "%ld\n"
print_str_fmt: .string "%s\n"
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $32, %rsp
 movq $16, %rdi
 call malloc@PLT
 movq %rax, %r11
.section .rodata
.LC0: .string "EDA"
.text
 leaq .LC0(%rip), %rax
 movq %rax, 0(%r11)
.section .rodata
.LC1: .string "Victor Flores"
.text
 leaq .LC1(%rip), %rax
 movq %rax, 8(%r11)
 movq %r11, %rax
 movq %rax, -8(%rbp)
 movq $16, %rdi
 call malloc@PLT
 movq %rax, %r11
.section .rodata
.LC2: .string "Compiladores"
.text
 leaq .LC2(%rip), %rax
 movq %rax, 0(%r11)
.section .rodata
.LC3: .string "Julio Yarasca"
.text
 leaq .LC3(%rip), %rax
 movq %rax, 8(%r11)
 movq %r11, %rax
 movq %rax, -16(%rbp)
 movq $16, %rdi
 call malloc@PLT
 movq %rax, %r11
.section .rodata
.LC4: .string "Arquitectura"
.text
 leaq .LC4(%rip), %rax
 movq %rax, 0(%r11)
.section .rodata
.LC5: .string "Jorge Reaño"
.text
 leaq .LC5(%rip), %rax
 movq %rax, 8(%r11)
 movq %r11, %rax
 movq %rax, -24(%rbp)
 movq $24, %rdi
 call malloc@PLT
 movq %rax, %r10
 movq -8(%rbp), %rax
 movq %rax, 0(%r10)
 movq -16(%rbp), %rax
 movq %rax, 8(%r10)
 movq -24(%rbp), %rax
 movq %rax, 16(%r10)
 movq %r10, %rax
 movq %rax, -32(%rbp)
 movq -32(%rbp), %rax
 movq %rax, %rdx
 movq $0, %rax
 imulq $8, %rax
 addq %rax, %rdx
 movq (%rdx), %rax
 movq %rax, %rdx
 movq 8(%rdx), %rax
 movq %rax, %rsi
 leaq print_str_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq -32(%rbp), %rax
 movq %rax, %rdx
 movq $1, %rax
 imulq $8, %rax
 addq %rax, %rdx
 movq (%rdx), %rax
 movq %rax, %rdx
 movq 8(%rdx), %rax
 movq %rax, %rsi
 leaq print_str_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq -32(%rbp), %rax
 movq %rax, %rdx
 movq $2, %rax
 imulq $8, %rax
 addq %rax, %rdx
 movq (%rdx), %rax
 movq %rax, %rdx
 movq 8(%rdx), %rax
 movq %rax, %rsi
 leaq print_str_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.section .rodata
.LC6: .string "Carlos Williams"
.text
 leaq .LC6(%rip), %rax
 pushq %rax
 movq -32(%rbp), %rax
 movq %rax, %rdx
 movq $2, %rax
 imulq $8, %rax
 addq %rax, %rdx
 movq (%rdx), %rax
 movq %rax, %rdx
 leaq 8(%rdx), %rax
 movq %rax, %rdx
 popq %rax
 movq %rax, (%rdx)
 movq -32(%rbp), %rax
 movq %rax, %rdx
 movq $2, %rax
 imulq $8, %rax
 addq %rax, %rdx
 movq (%rdx), %rax
 movq %rax, %rdx
 movq 8(%rdx), %rax
 movq %rax, %rsi
 leaq print_str_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.Lend_main:
 leave
 ret
.section .note.GNU-stack,"",@progbits

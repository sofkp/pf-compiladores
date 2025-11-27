.data
print_int_fmt: .string "%ld\n"
print_str_fmt: .string "%s\n"
.text
.globl procesar
procesar:
 pushq %rbp
 movq %rsp, %rbp
 subq $32, %rsp
 movq %rdi, -8(%rbp)
 movq $0, %rax
 movq %rax, -16(%rbp)
 movq $0, %rax
 movq %rax, -24(%rbp)
.Lfor_0:
 movq $3, %rax
 movq %rax, %rcx
 movq -24(%rbp), %rax
 cmpq %rcx, %rax
 jge .Lendfor_0
 movq -16(%rbp), %rax
 pushq %rax
 movq -8(%rbp), %rax
 movq %rax, %rdx
 movq 0(%rdx), %rax
 movq %rax, %rdx
 movq -24(%rbp), %rax
 imulq $8, %rax
 addq %rax, %rdx
 movq (%rdx), %rax
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
 pushq %rax
 leaq -16(%rbp), %rax
 movq %rax, %rdx
 popq %rax
 movq %rax, (%rdx)
 movq -24(%rbp), %rax
 addq $1, %rax
 movq %rax, -24(%rbp)
 jmp .Lfor_0
.Lendfor_0:
 movq -16(%rbp), %rax
 jmp .Lend_procesar
.Lend_procesar:
 leave
 ret
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $48, %rsp
.section .rodata
.LC0: .string "Sensor"
.text
 leaq .LC0(%rip), %rax
 movq %rax, -8(%rbp)
 movq -8(%rbp), %rax
 movq %rax, %rsi
 leaq print_str_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq $8, %rdi
 call malloc@PLT
 movq %rax, %r11
 movq $24, %rdi
 call malloc@PLT
 movq %rax, %r10
 movq $3, %rax
 movq %rax, 0(%r10)
 movq $4, %rax
 movq %rax, 8(%r10)
 movq $5, %rax
 movq %rax, 16(%r10)
 movq %r10, %rax
 movq %rax, 0(%r11)
 movq %r11, %rax
 movq %rax, -16(%rbp)
 movq -16(%rbp), %rax
 movq %rax, %rdi
 call procesar
 movq %rax, -24(%rbp)
 movq $10, %rax
 movq %rax, -32(%rbp)
.Lfor_1:
 movq $15, %rax
 movq %rax, %rcx
 movq -32(%rbp), %rax
 cmpq %rcx, %rax
 jg .Lendfor_1
 movq -32(%rbp), %rax
 pushq %rax
 movq -24(%rbp), %rax
 movq %rax, %rcx
 popq %rax
 cmpq %rcx, %rax
 setl %al
 movzbq %al, %rax
 cmpq $0, %rax
 je .Lelse_2
 movq -32(%rbp), %rax
 movq %rax, %rsi
 leaq print_int_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 jmp .Lendif_2
.Lelse_2:
.Lendif_2:
 movq -32(%rbp), %rax
 addq $1, %rax
 movq %rax, -32(%rbp)
 jmp .Lfor_1
.Lendfor_1:
 movq $15, %rax
 movq %rax, -40(%rbp)
 movq $2, %rax
 movq %rax, %rsi
 leaq print_int_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq $555, %rax
 movq %rax, %rsi
 leaq print_int_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.Lend_main:
 leave
 ret
.section .note.GNU-stack,"",@progbits

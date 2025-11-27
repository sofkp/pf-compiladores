.data
print_int_fmt: .string "%ld\n"
print_str_fmt: .string "%s\n"
.text
.globl crear
crear:
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
 jmp .Lend_crear
.Lend_crear:
 leave
 ret
.globl valortotal
valortotal:
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
 jmp .Lend_valortotal
.Lend_valortotal:
 leave
 ret
.globl venderunidades
venderunidades:
 pushq %rbp
 movq %rsp, %rbp
 subq $32, %rsp
 movq %rdi, -8(%rbp)
 movq %rsi, -16(%rbp)
 movq $24, %rdi
 call malloc@PLT
 movq %rax, %r11
 movq -8(%rbp), %rax
 movq %rax, %rdx
 movq 0(%rdx), %rax
 movq %rax, 0(%r11)
 movq -8(%rbp), %rax
 movq %rax, %rdx
 movq 8(%rdx), %rax
 movq %rax, 8(%r11)
 movq -8(%rbp), %rax
 movq %rax, %rdx
 movq 16(%rdx), %rax
 pushq %rax
 movq -16(%rbp), %rax
 movq %rax, %rcx
 popq %rax
 subq %rcx, %rax
 movq %rax, 16(%r11)
 movq %r11, %rax
 movq %rax, -24(%rbp)
 movq -24(%rbp), %rax
 jmp .Lend_venderunidades
.Lend_venderunidades:
 leave
 ret
.globl sumararray
sumararray:
 pushq %rbp
 movq %rsp, %rbp
 subq $16, %rsp
 movq %rdi, -8(%rbp)
 movq $0, %rax
 movq %rax, -16(%rbp)
 movq -16(%rbp), %rax
 pushq %rax
 movq -8(%rbp), %rax
 movq %rax, %rdx
 movq $0, %rax
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
 movq -16(%rbp), %rax
 pushq %rax
 movq -8(%rbp), %rax
 movq %rax, %rdx
 movq $1, %rax
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
 movq -16(%rbp), %rax
 pushq %rax
 movq -8(%rbp), %rax
 movq %rax, %rdx
 movq $2, %rax
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
 movq -16(%rbp), %rax
 jmp .Lend_sumararray
.Lend_sumararray:
 leave
 ret
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $96, %rsp
 movq $1001, %rax
 movq %rax, %rdi
 movq $25, %rax
 movq %rax, %rsi
 movq $10, %rax
 movq %rax, %rdx
 call crear
 movq %rax, -8(%rbp)
 movq $1002, %rax
 movq %rax, %rdi
 movq $40, %rax
 movq %rax, %rsi
 movq $5, %rax
 movq %rax, %rdx
 call crear
 movq %rax, -16(%rbp)
 movq $1003, %rax
 movq %rax, %rdi
 movq $12, %rax
 movq %rax, %rsi
 movq $20, %rax
 movq %rax, %rdx
 call crear
 movq %rax, -24(%rbp)
 movq -8(%rbp), %rax
 movq %rax, %rdx
 movq 0(%rdx), %rax
 movq %rax, %rsi
 leaq print_int_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq -16(%rbp), %rax
 movq %rax, %rdx
 movq 0(%rdx), %rax
 movq %rax, %rsi
 leaq print_int_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq -24(%rbp), %rax
 movq %rax, %rdx
 movq 0(%rdx), %rax
 movq %rax, %rsi
 leaq print_int_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq -8(%rbp), %rax
 movq %rax, %rdi
 call valortotal
 movq %rax, -32(%rbp)
 movq -16(%rbp), %rax
 movq %rax, %rdi
 call valortotal
 movq %rax, -40(%rbp)
 movq -24(%rbp), %rax
 movq %rax, %rdi
 call valortotal
 movq %rax, -48(%rbp)
 movq -32(%rbp), %rax
 movq %rax, %rsi
 leaq print_int_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq -40(%rbp), %rax
 movq %rax, %rsi
 leaq print_int_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq -48(%rbp), %rax
 movq %rax, %rsi
 leaq print_int_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq $24, %rdi
 call malloc@PLT
 movq %rax, %r10
 movq -32(%rbp), %rax
 movq %rax, 0(%r10)
 movq -40(%rbp), %rax
 movq %rax, 8(%r10)
 movq -48(%rbp), %rax
 movq %rax, 16(%r10)
 movq %r10, %rax
 movq %rax, -56(%rbp)
 movq -56(%rbp), %rax
 movq %rax, %rdi
 call sumararray
 movq %rax, %rsi
 leaq print_int_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq -8(%rbp), %rax
 movq %rax, %rdi
 movq $3, %rax
 movq %rax, %rsi
 call venderunidades
 movq %rax, -64(%rbp)
 movq -16(%rbp), %rax
 movq %rax, %rdi
 movq $1, %rax
 movq %rax, %rsi
 call venderunidades
 movq %rax, -72(%rbp)
 movq -64(%rbp), %rax
 movq %rax, %rdx
 movq 16(%rdx), %rax
 movq %rax, %rsi
 leaq print_int_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq -72(%rbp), %rax
 movq %rax, %rdx
 movq 16(%rdx), %rax
 movq %rax, %rsi
 leaq print_int_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq -24(%rbp), %rax
 movq %rax, %rdx
 movq 16(%rdx), %rax
 movq %rax, -80(%rbp)
.Lwhile_0:
 movq -80(%rbp), %rax
 pushq %rax
 movq $15, %rax
 movq %rax, %rcx
 popq %rax
 cmpq %rcx, %rax
 setg %al
 movzbq %al, %rax
 cmpq $0, %rax
 je .Lendwhile_0
 movq -80(%rbp), %rax
 pushq %rax
 movq $2, %rax
 movq %rax, %rcx
 popq %rax
 subq %rcx, %rax
 pushq %rax
 leaq -80(%rbp), %rax
 movq %rax, %rdx
 popq %rax
 movq %rax, (%rdx)
 jmp .Lwhile_0
.Lendwhile_0:
 movq -80(%rbp), %rax
 movq %rax, %rsi
 leaq print_int_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq $0, %rax
 movq %rax, -88(%rbp)
 movq $0, %rax
 movq %rax, -96(%rbp)
.Lfor_1:
 movq $3, %rax
 movq %rax, %rcx
 movq -96(%rbp), %rax
 cmpq %rcx, %rax
 jge .Lendfor_1
 movq -88(%rbp), %rax
 pushq %rax
 movq -56(%rbp), %rax
 movq %rax, %rdx
 movq -96(%rbp), %rax
 imulq $8, %rax
 addq %rax, %rdx
 movq (%rdx), %rax
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
 pushq %rax
 leaq -88(%rbp), %rax
 movq %rax, %rdx
 popq %rax
 movq %rax, (%rdx)
 movq -96(%rbp), %rax
 addq $1, %rax
 movq %rax, -96(%rbp)
 jmp .Lfor_1
.Lendfor_1:
 movq -88(%rbp), %rax
 movq %rax, %rsi
 leaq print_int_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq -88(%rbp), %rax
 pushq %rax
 movq $200, %rax
 movq %rax, %rcx
 popq %rax
 cmpq %rcx, %rax
 setge %al
 movzbq %al, %rax
 cmpq $0, %rax
 je .Lelse_2
 movq -8(%rbp), %rax
 movq %rax, %rdx
 movq 0(%rdx), %rax
 movq %rax, %rsi
 leaq print_int_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 jmp .Lendif_2
.Lelse_2:
 movq -16(%rbp), %rax
 movq %rax, %rdx
 movq 0(%rdx), %rax
 movq %rax, %rsi
 leaq print_int_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.Lendif_2:
.Lend_main:
 leave
 ret
.section .note.GNU-stack,"",@progbits

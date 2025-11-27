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
 movq $10, %rax
 movq %rax, 0(%r11)
 movq $4, %rax
 movq %rax, 8(%r11)
 movq %r11, %rax
 movq %rax, -8(%rbp)
 movq -8(%rbp), %rax
 movq %rax, %rdx
 movq 0(%rdx), %rax
 pushq %rax
 movq -8(%rbp), %rax
 movq %rax, %rdx
 movq 8(%rdx), %rax
 movq %rax, %rcx
 popq %rax
 subq %rcx, %rax
 pushq %rax
 movq $6, %rax
 movq %rax, %rcx
 popq %rax
 cmpq %rcx, %rax
 sete %al
 movzbq %al, %rax
 cmpq $0, %rax
 je .Lelse_0
 movq $77, %rax
 movq %rax, %rsi
 leaq print_int_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 jmp .Lendif_0
.Lelse_0:
 movq $88, %rax
 movq %rax, %rsi
 leaq print_int_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.Lendif_0:
.Lend_main:
 leave
 ret
.section .note.GNU-stack,"",@progbits

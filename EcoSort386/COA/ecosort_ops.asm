BITS 64
default rel

global asm_add
global asm_sub
global asm_mul
global asm_div
global asm_cmp_priority

section .text

; ============================================================================
; EcoSort 386 - COA 64-Bit Assembly Arithmetic & Priority Engine
; Implements Native x86-64 Instructions: ADD, SUB, MUL, DIV, CMP, JGE
; Follows Microsoft x64 Calling Convention:
;   Args:   RCX, RDX, R8, R9
;   Return: RAX
; ============================================================================

; uint64_t asm_add(uint64_t a, uint64_t b)
asm_add:
    mov rax, rcx
    add rax, rdx
    ret

; uint64_t asm_sub(uint64_t a, uint64_t b)
asm_sub:
    mov rax, rcx
    sub rax, rdx
    ret

; uint64_t asm_mul(uint64_t a, uint64_t b, uint64_t* out_high)
asm_mul:
    mov rax, rcx
    mul rdx            ; RDX:RAX = RAX * RDX
    test r8, r8
    jz .skip_high
    mov [r8], rdx
.skip_high:
    ret

; uint64_t asm_div(uint64_t a, uint64_t b, uint64_t* out_rem)
asm_div:
    mov rax, rcx
    mov r9, rdx        ; Divisor in R9
    xor rdx, rdx       ; Clear RDX before unsigned division
    div r9             ; RAX = quotient, RDX = remainder
    test r8, r8
    jz .skip_rem
    mov [r8], rdx
.skip_rem:
    ret

; int64_t asm_cmp_priority(int64_t weight, int64_t threshold)
asm_cmp_priority:
    cmp rcx, rdx
    jge .high_prio
    xor eax, eax       ; 0 = Normal priority
    ret
.high_prio:
    mov eax, 1         ; 1 = High priority (Quarantine)
    ret

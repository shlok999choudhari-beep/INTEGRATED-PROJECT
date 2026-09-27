BITS 64

default rel

global main
extern printf

section .data

    total_msg db "Total E-Waste Weight : %lld kg", 10, 0
    remaining_msg db "Remaining Weight     : %lld kg", 10, 0
    value_msg db "Estimated Value      : Rs %lld", 10, 0
    average_msg db "Average Weight       : %lld kg", 10, 0

    high_msg db "Priority             : HIGH PRIORITY", 10, 0
    normal_msg db "Priority             : NORMAL PRIORITY", 10, 0

section .text

main:

    ; ==========================================
    ; ECO SORT CO2 - 64 BIT ASSEMBLY
    ; ==========================================


    ; ------------------------------------------
    ; 1. ADDITION
    ; Laptop = 10 kg
    ; Mobile = 5 kg
    ; Total = 15 kg
    ; ------------------------------------------

    mov rax, 10
    mov rcx, 5

    add rax, rcx

    ; RAX = 15

    mov rdx, rax
    lea rcx, [rel total_msg]

    sub rsp, 40
    call printf
    add rsp, 40


    ; ------------------------------------------
    ; 2. SUBTRACTION
    ; Total Weight = 15 kg
    ; Laptop Weight = 10 kg
    ; Remaining = 5 kg
    ; ------------------------------------------

    mov rax, 15
    mov rcx, 10

    sub rax, rcx

    ; RAX = 5

    mov rdx, rax
    lea rcx, [rel remaining_msg]

    sub rsp, 40
    call printf
    add rsp, 40


    ; ------------------------------------------
    ; 3. MULTIPLICATION
    ; Total Weight = 15 kg
    ; Recycling Rate = Rs 50/kg
    ; Value = 750
    ; ------------------------------------------

    mov rax, 15
    mov rcx, 50

    mul rcx

    ; RDX:RAX contains result
    ; RAX = 750

    mov rdx, rax
    lea rcx, [rel value_msg]

    sub rsp, 40
    call printf
    add rsp, 40


    ; ------------------------------------------
    ; 4. DIVISION
    ; Total Weight = 15 kg
    ; Number of waste types = 2
    ; Average = 7 kg
    ; ------------------------------------------

    mov rax, 15
    mov rcx, 2

    xor rdx, rdx

    div rcx

    ; RAX = quotient = 7
    ; RDX = remainder = 1

    mov rdx, rax
    lea rcx, [rel average_msg]

    sub rsp, 40
    call printf
    add rsp, 40


    ; ------------------------------------------
    ; 5. COMPARISON
    ;
    ; If total weight >= 10 kg
    ;       HIGH PRIORITY
    ; Else
    ;       NORMAL PRIORITY
    ; ------------------------------------------

    mov rax, 15
    cmp rax, 10

    jge high_priority

    ; If weight < 10
    lea rcx, [rel normal_msg]

    sub rsp, 40
    call printf
    add rsp, 40

    jmp program_end


high_priority:

    ; Weight >= 10
    lea rcx, [rel high_msg]

    sub rsp, 40
    call printf
    add rsp, 40


program_end:

    xor eax, eax
    ret
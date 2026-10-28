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

    decision_banner db 10, "===== DECISION TEST CASES (COA x86-64 EVALUATION) =====", 10, 0
    dec_reusable_msg db "Reusable, 3 kg: Check if it can be reused or repaired.", 10, 0
    dec_recyclable_msg db "Recyclable, 12 kg: Send to priority recycling batch.", 10, 0
    dec_hazardous_msg db "Hazardous, 2 kg: Send for special hazardous-waste handling.", 10, 0

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

    jmp test_decisions


high_priority:

    ; Weight >= 10
    lea rcx, [rel high_msg]

    sub rsp, 40
    call printf
    add rsp, 40


test_decisions:

    ; ------------------------------------------
    ; 6. CATEGORY-BASED DECISION LOGIC
    ; Mapped from PSOOP CO4 / EcoSort test cases:
    ; Case 1: Reusable, 3 kg -> Repair / Reuse
    ; Case 2: Recyclable, 12 kg (>= 10 kg) -> Priority
    ; Case 3: Hazardous, 2 kg -> Special Handling
    ; ------------------------------------------

    lea rcx, [rel decision_banner]
    sub rsp, 40
    call printf
    add rsp, 40

    ; Case 1: Category = 1 (Reusable), Weight = 3 kg
    mov rcx, 1
    cmp rcx, 1
    jne .check_case2
    lea rcx, [rel dec_reusable_msg]
    sub rsp, 40
    call printf
    add rsp, 40

.check_case2:
    ; Case 2: Category = 2 (Recyclable), Weight = 12 kg, Threshold = 10 kg
    mov rcx, 2
    mov rax, 12
    cmp rax, 10
    jl .case2_normal
    lea rcx, [rel dec_recyclable_msg]
    sub rsp, 40
    call printf
    add rsp, 40
    jmp .check_case3

.case2_normal:
    ; (Not reached in test case 2)

.check_case3:
    ; Case 3: Category = 3 (Hazardous), Weight = 2 kg
    mov rcx, 3
    cmp rcx, 3
    jne program_end
    lea rcx, [rel dec_hazardous_msg]
    sub rsp, 40
    call printf
    add rsp, 40


program_end:

    xor eax, eax
    ret

BITS 64

default rel

; EcoSort 386 COA integration functions for the Windows x64 calling convention.
; Integer arguments arrive in RCX, RDX, R8, R9; integer result returns in RAX.

global ecosort_add
global ecosort_sub
global ecosort_mul
global ecosort_div
global ecosort_is_high_priority
global ecosort_classify_waste

section .text

ecosort_add:
    ; RAX = first + second
    mov rax, rcx
    add rax, rdx
    ret

ecosort_sub:
    ; RAX = first - second
    mov rax, rcx
    sub rax, rdx
    ret

ecosort_mul:
    ; Unsigned multiply: RDX:RAX = RAX * RDX; return low 64 bits in RAX.
    mov rax, rcx
    mul rdx
    ret

ecosort_div:
    ; Return numerator / denominator. Return 0 for denominator 0.
    test rdx, rdx
    jz .divide_by_zero
    mov r8, rdx
    mov rax, rcx
    xor edx, edx
    div r8
    ret
.divide_by_zero:
    xor eax, eax
    ret

ecosort_is_high_priority:
    ; Return 1 if weight >= threshold (signed comparison), otherwise 0.
    cmp rcx, rdx
    setge al
    movzx eax, al
    ret

ecosort_classify_waste:
    ; Waste Decision Classifier mapping directly to EcoSort Core & PSOOP CO4:
    ;   RCX = Category Code:
    ;         EcoSortCore::CAT_RECYCLABLE = 1
    ;         EcoSortCore::CAT_HAZARDOUS  = 2
    ;         EcoSortCore::CAT_REUSABLE   = 3
    ;   RDX = Weight (kg)
    ;   R8  = Priority Threshold Limit (10 kg)
    ; Returns action status code in RAX:
    ;   1 = Reusable (Check if it can be reused or repaired)
    ;   2 = Normal Recycling (Recyclable, weight < threshold limit)
    ;   3 = Priority Recycling (Recyclable, weight >= threshold limit)
    ;   4 = Hazardous Handling (Send for special hazardous-waste handling)
    cmp rcx, 3
    je .is_reusable
    cmp rcx, 2
    je .is_hazardous

    ; Recyclable (category 1): evaluate weight against threshold limit
    cmp rdx, r8
    jge .is_priority_recycling
    mov eax, 2              ; Code 2: Normal recycling batch
    ret

.is_priority_recycling:
    mov eax, 3              ; Code 3: Priority recycling batch
    ret

.is_reusable:
    mov eax, 1              ; Code 1: Check if it can be reused or repaired
    ret

.is_hazardous:
    mov eax, 4              ; Code 4: Send for special hazardous-waste handling
    ret

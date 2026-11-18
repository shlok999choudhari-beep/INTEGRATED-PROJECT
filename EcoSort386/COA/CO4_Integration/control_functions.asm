BITS 64
default rel

; ============================================================================
; EcoSort 386 - COA CO4: Processor Control, ALU Pipeline & System Integration
; Demonstrates Hardware-Level Control Operations for EcoSort Transaction Engine:
;   - Register-Based Parameter Passing (RCX, RDX, R8, R9)
;   - Arithmetic Instructions & ALU Operations (MUL, DIV, ADD, SUB)
;   - Flag Telemetry & Comparison Instructions (CMP, TEST, PUSHFQ, POP)
;   - Conditional Branching Logic (JE, JNE, JL, JGE, SETGE)
;   - Conforms strictly to Microsoft x64 Calling Convention
; ============================================================================

global asm_co4_validate_inputs
global asm_co4_compare_and_branch
global asm_co4_decision_control
global asm_co4_mul_overflow_check
global asm_co4_div_with_remainder
global asm_co4_read_rflags
global asm_co4_classify_category_str

extern asm_strcmp
extern asm_strlen

section .data
    str_rec db "RECYCLABLE", 0
    str_haz db "HAZARDOUS",  0
    str_reu db "REUSABLE",   0

section .text

; ----------------------------------------------------------------------------
; 1. asm_co4_validate_inputs
; Validates transaction inputs against hardware boundary conditions:
;   RCX = Category ID (Must be 1, 2, or 3)
;   RDX = Weight in kg (Must be > 0)
;   R8  = Rate per kg (Must be >= 0)
;   R9  = Priority Threshold in kg (Must be > 0)
; Returns RAX = 1 if valid, 0 if invalid
; ----------------------------------------------------------------------------
asm_co4_validate_inputs:
    ; Check Category: 1 <= RCX <= 3
    cmp rcx, 1
    jl .invalid
    cmp rcx, 3
    jg .invalid

    ; Check Weight: RDX > 0
    cmp rdx, 0
    jle .invalid

    ; Check Rate: R8 >= 0
    cmp r8, 0
    jl .invalid

    ; Check Threshold: R9 > 0
    cmp r9, 0
    jle .invalid

    mov eax, 1              ; Valid
    ret

.invalid:
    xor eax, eax            ; Invalid (0)
    ret

; ----------------------------------------------------------------------------
; 2. asm_co4_compare_and_branch
; Compares weight against threshold, extracts live CPU RFLAGS, and branches:
;   RCX = Weight (int64_t)
;   RDX = Threshold (int64_t)
;   R8  = Pointer to uint64_t to receive raw RFLAGS register (optional)
; Returns RAX = 1 (HIGH PRIORITY) or 0 (NORMAL PRIORITY)
; ----------------------------------------------------------------------------
asm_co4_compare_and_branch:
    cmp rcx, rdx            ; Compare Weight (RCX) with Threshold (RDX)
    pushfq                  ; Push 64-bit RFLAGS to stack
    pop rax                 ; Pop RFLAGS into RAX
    test r8, r8             ; Check if caller provided out_flags pointer
    jz .evaluate_branch
    mov [r8], rax           ; Store captured CPU flags for student inspection

.evaluate_branch:
    ; Re-run compare to set flags cleanly for jump instruction
    cmp rcx, rdx
    jge .branch_high        ; Conditional Jump: if Weight >= Threshold
    xor eax, eax            ; RAX = 0 (NORMAL PRIORITY)
    ret

.branch_high:
    mov eax, 1              ; RAX = 1 (HIGH PRIORITY)
    ret

; ----------------------------------------------------------------------------
; 3. asm_co4_decision_control
; Multi-way processor decision classifier matching EcoSort Unified Rules:
;   RCX = Category (1 = Recyclable, 2 = Hazardous, 3 = Reusable)
;   RDX = Weight (kg)
;   R8  = Priority Threshold (kg)
; Returns Action Code in RAX:
;   1 = Reusable (Check if it can be reused or repaired)
;   2 = Normal Recycling (Recyclable, weight < threshold limit)
;   3 = Priority Recycling (Recyclable, weight >= threshold limit)
;   4 = Hazardous Handling (Send for special hazardous-waste handling)
;   0 = Invalid / Unrecognized
; ----------------------------------------------------------------------------
asm_co4_decision_control:
    cmp rcx, 3
    je .route_reusable      ; Category 3 -> Reusable

    cmp rcx, 2
    je .route_hazardous     ; Category 2 -> Hazardous

    cmp rcx, 1
    je .route_recyclable    ; Category 1 -> Recyclable

    xor eax, eax            ; Unrecognized category
    ret

.route_recyclable:
    cmp rdx, r8             ; Compare weight with priority threshold
    jge .priority_batch
    mov eax, 2              ; Code 2: Normal recycling batch
    ret

.priority_batch:
    mov eax, 3              ; Code 3: Priority recycling batch
    ret

.route_reusable:
    mov eax, 1              ; Code 1: Check if it can be reused or repaired
    ret

.route_hazardous:
    mov eax, 4              ; Code 4: Send for special hazardous-waste handling
    ret

; ----------------------------------------------------------------------------
; 4. asm_co4_mul_overflow_check
; Performs 64-bit unsigned multiplication and checks upper 64-bit overflow:
;   RCX = Multiplicand (Weight)
;   RDX = Multiplier (Rate)
;   R8  = Pointer to uint64_t to receive upper 64-bit product (RDX)
; Returns RAX = lower 64-bit product
; ----------------------------------------------------------------------------
asm_co4_mul_overflow_check:
    mov rax, rcx
    mul rdx                 ; RDX:RAX = RAX * RDX
    test r8, r8
    jz .done_mul
    mov [r8], rdx           ; Save upper 64 bits to inspect overflow
.done_mul:
    ret

; ----------------------------------------------------------------------------
; 5. asm_co4_div_with_remainder
; Performs 64-bit unsigned division with zero-divisor hardware safeguard:
;   RCX = Dividend (Total Value)
;   RDX = Divisor (Item Count or Weight)
;   R8  = Pointer to uint64_t to receive remainder (optional)
; Returns RAX = quotient (or 0 if division by zero attempted)
; ----------------------------------------------------------------------------
asm_co4_div_with_remainder:
    test rdx, rdx           ; Check for divisor == 0
    jz .divide_by_zero_safe

    mov r9, rdx             ; Move divisor to R9
    mov rax, rcx            ; Move dividend to RAX
    xor edx, edx            ; Zero out RDX before 64-bit division
    div r9                  ; RAX = quotient, RDX = remainder

    test r8, r8
    jz .done_div
    mov [r8], rdx           ; Store remainder
.done_div:
    ret

.divide_by_zero_safe:
    xor eax, eax            ; Return quotient = 0
    test r8, r8
    jz .done_div
    mov qword [r8], 0       ; Remainder = 0
    ret

; ----------------------------------------------------------------------------
; 6. asm_co4_read_rflags
; Returns current 64-bit CPU RFLAGS register in RAX
; uint64_t asm_co4_read_rflags(void);
; ----------------------------------------------------------------------------
asm_co4_read_rflags:
    pushfq
    pop rax
    ret

; ----------------------------------------------------------------------------
; 7. asm_co4_classify_category_str
; Matches uppercase string against canonical EcoSort categories:
;   RCX = Pointer to null-terminated uppercase string
; Returns Category ID in RAX:
;   1 = Recyclable
;   2 = Hazardous
;   3 = Reusable
;   0 = Unrecognized
; ----------------------------------------------------------------------------
asm_co4_classify_category_str:
    push rbx
    push rsi
    push rdi
    sub rsp, 40             ; Shadow space + 16-byte stack alignment

    mov rbx, rcx            ; Save string pointer in non-volatile RBX

    ; Check "RECYCLABLE"
    mov rcx, rbx
    lea rdx, [rel str_rec]
    call asm_strcmp
    test rax, rax
    jz .found_recyclable

    ; Check "HAZARDOUS"
    mov rcx, rbx
    lea rdx, [rel str_haz]
    call asm_strcmp
    test rax, rax
    jz .found_hazardous

    ; Check "REUSABLE"
    mov rcx, rbx
    lea rdx, [rel str_reu]
    call asm_strcmp
    test rax, rax
    jz .found_reusable

    xor eax, eax            ; 0 = Unrecognized
    jmp .exit_class

.found_recyclable:
    mov eax, 1
    jmp .exit_class

.found_hazardous:
    mov eax, 2
    jmp .exit_class

.found_reusable:
    mov eax, 3

.exit_class:
    add rsp, 40
    pop rdi
    pop rsi
    pop rbx
    ret

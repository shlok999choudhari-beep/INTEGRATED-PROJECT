BITS 64
default rel

; ============================================================================
; EcoSort 386 - COA CO3: Algorithmic Arithmetic & 64-Bit String Processing
; Conforms to Microsoft x64 Calling Convention:
;   Input Arguments: RCX, RDX, R8, R9
;   Return Value:    RAX
;   Preserved Regs:  RBX, RBP, RDI, RSI, R12-R15
; ============================================================================

global asm_mul_shift_add
global asm_mul_successive_add
global asm_strlen
global asm_strcmp
global asm_classify_category_string
global asm_str_to_upper

section .data
    str_reusable   db "Reusable", 0
    str_recyclable db "Recyclable", 0
    str_hazardous  db "Hazardous", 0

section .text

; ----------------------------------------------------------------------------
; 1. Add-and-Shift Binary Multiplication Algorithm (COA CO3)
; Multiplies multiplicand (RCX) by multiplier (RDX) using bitwise shifts
; and conditional additions (emulating hardware ALU micro-operations).
; uint64_t asm_mul_shift_add(uint64_t multiplicand, uint64_t multiplier);
; ----------------------------------------------------------------------------
asm_mul_shift_add:
    xor rax, rax            ; Clear accumulator RAX = 0
    test rdx, rdx           ; Check if multiplier is 0
    jz .done

.loop:
    test rdx, 1             ; Test Least Significant Bit (LSB) of multiplier
    jz .skip_add            ; If LSB == 0, skip addition
    add rax, rcx            ; Accumulator += Multiplicand

.skip_add:
    shl rcx, 1              ; Shift Multiplicand Left by 1 bit (Multiply by 2)
    shr rdx, 1              ; Shift Multiplier Right by 1 bit (Divide by 2)
    jnz .loop               ; Continue while multiplier has remaining set bits

.done:
    ret

; ----------------------------------------------------------------------------
; 2. Successive Addition Multiplication Algorithm (COA CO3)
; Multiplies value (RCX) by count (RDX) via iterative accumulator additions.
; uint64_t asm_mul_successive_add(uint64_t value, uint64_t count);
; ----------------------------------------------------------------------------
asm_mul_successive_add:
    xor rax, rax            ; RAX = 0
    test rdx, rdx           ; If count == 0, return 0
    jz .done

.add_loop:
    add rax, rcx            ; Accumulator += Value
    dec rdx                 ; Decrement count
    jnz .add_loop           ; Loop until count reaches 0

.done:
    ret

; ----------------------------------------------------------------------------
; 3. 64-Bit Assembly String Length Calculation (COA CO3)
; Uses 64-bit REPNE SCASB to search for null-terminator in string buffer.
; uint64_t asm_strlen(const char* str);
; ----------------------------------------------------------------------------
asm_strlen:
    push rdi                ; Save non-volatile RDI
    mov rdi, rcx            ; RDI points to source string
    xor eax, eax            ; Search target AL = 0 (Null terminator)
    mov rcx, -1             ; Set counter to maximum uint64
    cld                     ; Clear direction flag (Forward scan)
    repne scasb             ; Scan string until byte in [RDI] == AL
    not rcx                 ; Bitwise invert: RCX = 1 + String Length + 1
    dec rcx                 ; Subtract 1 for true length
    mov rax, rcx            ; Return length in RAX
    pop rdi                 ; Restore RDI
    ret

; ----------------------------------------------------------------------------
; 4. 64-Bit Assembly String Comparison (COA CO3)
; Compares two null-terminated strings byte by byte.
; int64_t asm_strcmp(const char* s1, const char* s2);
; Returns: 0 if equal, <0 if s1 < s2, >0 if s1 > s2
; ----------------------------------------------------------------------------
asm_strcmp:
    push rsi                ; Save non-volatile registers
    push rdi
    mov rsi, rcx            ; RSI = s1
    mov rdi, rdx            ; RDI = s2
    xor eax, eax

.cmp_loop:
    mov al, byte [rsi]
    mov dl, byte [rdi]
    cmp al, dl
    jne .diff               ; Characters differ
    test al, al
    jz .match               ; Both reached null-terminator: strings are identical
    inc rsi
    inc rdi
    jmp .cmp_loop

.diff:
    movzx eax, al
    movzx edx, dl
    sub eax, edx            ; Return difference (s1[i] - s2[i])
    movsxd rax, eax
    pop rdi
    pop rsi
    ret

.match:
    xor rax, rax            ; Return 0 for identical match
    pop rdi
    pop rsi
    ret

; ----------------------------------------------------------------------------
; 5. 64-Bit Assembly Waste Category Classifier (COA CO3 / PSOOP Mapping)
; Validates input string against standard category strings in assembly.
; int64_t asm_classify_category_string(const char* catStr);
; Returns: 1 = Reusable, 2 = Recyclable, 3 = Hazardous, 0 = Unrecognized
; ----------------------------------------------------------------------------
asm_classify_category_string:
    push rbx
    push rsi
    push rdi
    mov rbx, rcx            ; RBX = input string pointer

    ; Check "Reusable"
    mov rcx, rbx
    lea rdx, [rel str_reusable]
    call asm_strcmp
    test rax, rax
    jz .ret_reusable

    ; Check "Recyclable"
    mov rcx, rbx
    lea rdx, [rel str_recyclable]
    call asm_strcmp
    test rax, rax
    jz .ret_recyclable

    ; Check "Hazardous"
    mov rcx, rbx
    lea rdx, [rel str_hazardous]
    call asm_strcmp
    test rax, rax
    jz .ret_hazardous

    ; Unrecognized category
    xor eax, eax
    jmp .exit

.ret_reusable:
    mov eax, 1
    jmp .exit

.ret_recyclable:
    mov eax, 2
    jmp .exit

.ret_hazardous:
    mov eax, 3

.exit:
    pop rdi
    pop rsi
    pop rbx
    ret

; ----------------------------------------------------------------------------
; 6. 64-Bit Assembly String Upper-Case Normalization (COA CO3)
; Converts ASCII lowercase characters to uppercase.
; void asm_str_to_upper(char* dest, const char* src);
; ----------------------------------------------------------------------------
asm_str_to_upper:
    push rsi
    push rdi
    mov rdi, rcx            ; RDI = destination buffer
    mov rsi, rdx            ; RSI = source string

.upper_loop:
    mov al, byte [rsi]
    test al, al
    jz .upper_done
    cmp al, 'a'
    jb .store_char
    cmp al, 'z'
    ja .store_char
    sub al, 32              ; Convert lowercase to uppercase ('a' - 32 = 'A')

.store_char:
    mov byte [rdi], al
    inc rsi
    inc rdi
    jmp .upper_loop

.upper_done:
    mov byte [rdi], 0       ; Null terminate destination
    pop rdi
    pop rsi
    ret

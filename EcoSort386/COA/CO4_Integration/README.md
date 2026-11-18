# EcoSort 386 – COA CO4: System & Processor Pipeline Integration

## 1. Objective of CO4
The objective of **COA CO4 (Computer Organization & Architecture Course Outcome 4)** is to demonstrate how a complete, real-world application workflow in the **EcoSort 386 E-Waste Management System** is translated and executed directly on the **x86-64 CPU hardware pipeline**.

Instead of performing calculations in high-level C++, the transaction is driven by native 64-bit Assembly functions:
- Translating application operations into CPU registers.
- Performing arithmetic and valuations via the ALU (`MUL`, `DIV`, `ADD`, `SUB`).
- Executing comparisons (`CMP`, `TEST`) and evaluating processor condition flags (`RFLAGS`).
- Making routing decisions via hardware conditional branching (`JGE`, `JE`, `JNE`, `JL`).
- Processing and normalizing strings using assembly string operations (`REPNE SCASB`).
- Passing arguments and returning results via the **Microsoft Windows x64 Calling Convention**.

---

## 2. Integration of CO2 and CO3 Assembly Functions

CO4 unifies the foundational assembly routines built in CO2 and CO3:

### Reused CO2 Functions (`EcoSort386/COA/ecosort_functions.asm`)
- `ecosort_add`: Accumulates batch weights.
- `ecosort_sub`: Calculates remaining capacity.
- `ecosort_mul`: 64-bit hardware single-cycle unsigned multiplication for lot valuation.
- `ecosort_div`: 64-bit hardware integer division with zero-divisor hardware safeguard.
- `ecosort_is_high_priority`: Uses `CMP` and `SETGE` to evaluate priority thresholds.
- `ecosort_classify_waste`: Evaluates categories and priority limits.

### Reused CO3 Functions (`EcoSort386/COA/CO3_Algorithm_String/algorithm_string_functions.asm`)
- `asm_mul_shift_add`: Implements hardware ALU micro-operation Add-and-Shift binary multiplication algorithm.
- `asm_mul_successive_add`: Validates multiplication using successive accumulator additions.
- `asm_strlen`: Computes string length using 64-bit `REPNE SCASB`.
- `asm_strcmp`: Byte-by-byte string comparison.
- `asm_str_to_upper`: Converts lowercase ASCII to uppercase (`SUB AL, 32`).
- `asm_classify_category_string`: Maps string tokens directly to category identifiers.

### Dedicated CO4 Control & Telemetry Functions (`control_functions.asm`)
- `asm_co4_validate_inputs`: Verifies bounds on category, weight, rate, and threshold.
- `asm_co4_compare_and_branch`: Compares weight with threshold and captures the raw 64-bit `RFLAGS` register via `PUSHFQ` / `POP`.
- `asm_co4_decision_control`: Hardware branching table directing items to proper handling bins.
- `asm_co4_mul_overflow_check`: Detects upper 64-bit product overflow in `RDX`.
- `asm_co4_div_with_remainder`: Unsigned division with hardware zero divisor protection.
- `asm_co4_classify_category_str`: Normalizes uppercase inputs to standard category IDs.

---

## 3. Microsoft Windows x64 Calling Convention & Register Mapping

All assembly routines adhere strictly to the **Microsoft x64 ABI**:
- **First 4 integer/pointer arguments**: Passed in `RCX`, `RDX`, `R8`, `R9`.
- **Return value**: Returned in `RAX` (64-bit integer / pointer).
- **Upper 64-bit results**: Stored in `RDX` for `MUL` and `DIV`.
- **Preserved registers (Non-volatile)**: `RBX`, `RBP`, `RDI`, `RSI`, `R12`–`R15`.
- **Shadow Space**: 32 bytes allocated by the caller across call boundaries.

### Application-to-Processor Hardware Mapping Table

| EcoSort Operation | Processor-Level Operation | Registers & Instructions |
|---|---|---|
| Read waste category | Input buffer pointer | `RCX = &str` |
| Compute string length | Search null terminator | `REPNE SCASB` with `AL=0, RCX=-1` |
| Uppercase conversion | ASCII adjustment | `SUB AL, 32` |
| Classify category | String comparison | `CMP AL, DL` / `JE` |
| Load waste weight | 1st argument register | `RCX = weight` |
| Load recycling rate | 2nd argument register | `RDX = rate` |
| Calculate valuation | 64-bit hardware multiplication | `MUL RDX` -> `RAX = weight * rate` |
| Compare weight & threshold | Arithmetic comparison | `CMP RCX, RDX` (`CMP weight, threshold`) |
| Check priority status | Condition code evaluation | `RFLAGS` (`ZF`, `SF`, `OF`, `CF`) |
| Select priority branch | Conditional branching | `JGE` (Jump if Greater or Equal) |
| Multi-way routing decision | Control unit jump table | `CMP`, `JE`, `JGE` -> `RAX = action_code` |
| Return calculated value | Function return | `RAX = valuation` |

---

## 4. Standard Exemplary Transaction (Syllabus Demonstration)

- **Waste Category**: Recyclable
- **Waste Weight**: 12 kg
- **Recycling Rate**: ₹50 / kg
- **Priority Threshold**: 10 kg

### Expected & Verified Output:
```text
Waste Category: Recyclable
Waste Weight: 12 kg
Estimated Value: Rs. 600
Priority: HIGH
Handling: Priority recycling batch
```

---

## 5. How to Build and Run

### Prerequisites
- GCC / G++ (64-bit, e.g., MSYS2 UCRT64)
- NASM (Netwide Assembler, 64-bit)

### Single-Command Build:
```cmd
build_co4_integration.bat
```

### Run Module:
```cmd
run_co4.bat
```

### Run Automated Academic Test Suite (17 Comprehensive Tests):
```cmd
run_co4.bat --test
```

---

## 6. Viva Voce Questions & Answers

**Q1: What is the purpose of COA CO4 in EcoSort?**  
*Ans:* CO4 demonstrates complete system integration where high-level e-waste operations (categorization, pricing, and quarantine routing) execute through native 64-bit assembly instructions on real processor registers rather than high-level C++ wrappers.

**Q2: Which registers are used to pass arguments in Windows x64 assembly?**  
*Ans:* `RCX` for the 1st argument, `RDX` for the 2nd, `R8` for the 3rd, and `R9` for the 4th. The result is returned in `RAX`.

**Q3: How does the priority check work at the hardware level?**  
*Ans:* The CPU runs `CMP RCX, RDX` (subtracting threshold from weight internally without saving the result). It sets condition flags (`SF`, `OF`, `ZF`). The `JGE` instruction tests whether `SF == OF`. If true, the processor branches to the high-priority routine.

**Q4: How is string processing done in assembly without C library functions?**  
*Ans:* String length is calculated using `REPNE SCASB` scanning forward for the null terminator byte (`0x00`). String comparison runs a byte-by-byte loop with `CMP AL, DL` and `JNE`.

**Q5: How does the module prevent division by zero or overflow?**  
*Ans:* Before executing `DIV`, the divisor in `RDX` is tested using `TEST RDX, RDX` / `JZ`. If zero, the quotient and remainder are safely zeroed without triggering an unhandled CPU exception.

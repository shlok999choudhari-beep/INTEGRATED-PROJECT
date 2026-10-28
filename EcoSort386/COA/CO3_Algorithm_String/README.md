# EcoSort 386 – COA CO3
## Algorithmic Arithmetic and 64-Bit String Processing

This module satisfies **Computer Organization & Architecture Laboratory Task 3 / Review 3 (Course Outcome CO3)** for the EcoSort 386 integrated project.

---

## 📌 Academic Mapping & Syllabus Rubrics

| Syllabus Criteria | Rubric Level | Implementation in EcoSort 386 CO3 |
| :--- | :--- | :--- |
| **Arithmetic Algorithm (CO3)** | **Level 4 - Excellent (4 Marks)** | Native 64-bit Assembly implementation of the **Binary Add-and-Shift Multiplication Algorithm** (`asm_mul_shift_add`) and **Successive Addition Algorithm** (`asm_mul_successive_add`) with ALU cycle-by-cycle trace table comparing directly against single-cycle hardware `MUL`. |
| **64-Bit String Processing (CO3)** | **Level 4 - Excellent (4 Marks)** | Native 64-bit Assembly string scanning using `REPNE SCASB` for string length (`asm_strlen`), `REPE CMPSB` for category string matching (`asm_strcmp`), category classification (`asm_classify_category_string`), and ASCII case normalization (`asm_str_to_upper`). |
| **Cross-Subject Integration** | **Full Integration** | Synchronized with `PL and PSOOP` domain strings and `CGL` graphics through `ecosort_bridge.h` for real-time hardware interrupt telemetry. |

---

## ⚙️ File Breakdown

```text
EcoSort386/COA/CO3_Algorithm_String/
├── algorithm_string_functions.asm   # 64-bit x86 NASM assembly implementations
├── main.cpp                         # C++ verification driver & IPC listener
├── build_co3_algorithm_string.bat   # Assembler/Compiler build script
├── run_co3.bat                      # Interactive launcher
├── coa_co3_algorithm_string.exe     # Compiled 64-bit executable
└── README.md                        # Module documentation
```

---

## 🔬 Algorithm Micro-Operations

### 1. Add-and-Shift Binary Multiplication (`asm_mul_shift_add`)
* **Multiplicand** placed in `RCX`, **Multiplier** in `RDX`, **Accumulator** cleared in `RAX`.
* In each cycle:
  1. Test Least Significant Bit (LSB) of `RDX` (`test rdx, 1`).
  2. If LSB == 1, add `RCX` into accumulator `RAX` (`add rax, rcx`).
  3. Shift multiplicand left by 1 bit: `shl rcx, 1` (multiply by 2).
  4. Shift multiplier right by 1 bit: `shr rdx, 1` (divide by 2).
  5. Repeat until multiplier is zero (`jnz .loop`).

### 2. Assembly String Length (`asm_strlen`)
* Pointer in `RCX` moved to `RDI`.
* `AL` cleared to 0 (Null-terminator).
* Counter `RCX` set to `-1` (`0xFFFFFFFFFFFFFFFF`).
* Direction flag cleared (`cld`).
* `REPNE SCASB` repeatedly decrements `RCX` and advances `RDI` until a matching null byte is found.
* String length is obtained as `not rcx - 1`.

---

## ⚡ How to Build & Run

### To Compile and Run Automated Tests:
```cmd
build_co3_algorithm_string.bat --test
```

### To Launch the Interactive Menu:
```cmd
run_co3.bat
```

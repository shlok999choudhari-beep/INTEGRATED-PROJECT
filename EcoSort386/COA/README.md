# EcoSort 386 – COA

## Computer Organization and Architecture

The **COA module** of EcoSort 386 demonstrates processor-level processing using **64-bit x86-64 NASM Assembly Language**.

The COA implementation provides comprehensive demonstrations covering the syllabus Course Outcomes:
- **CO1 & CO2 – Processor Registers & Assembly Arithmetic:** 64-bit ALU operations (ADD, SUB, MUL, DIV), status flags (ZF, SF, OF), comparisons (`CMP`), and conditional branching (`JGE`, `JE`).
- **CO2 & PSOOP CO4 Integration – Category Decision Classification:** Pure 64-bit Assembly decision classifier (`ecosort_classify_waste`) evaluating waste categories and weight thresholds to route items to reuse, normal recycling, priority recycling, or hazardous quarantine.
- **CO3 – Algorithmic & String Processing:** Successive-addition multiplication algorithm, null-terminated string length, and byte-by-byte category string comparison (`CO3_Algorithm_String/`).
- **CO4 – Micro-Architecture Integration & Demonstration:** Control Unit instruction fetch/decode simulation, ALU execution, flag inspection, data path mapping, and live sensor telemetry (`CO4_Integration/` & `ecosort_coa_driver.cpp`).

---

### 📋 COA Course Outcomes (CO) Matrix

| CO | Topic | Assembly Routine / Function | Processor Concept Demonstrated |
| :--- | :--- | :--- | :--- |
| **CO1** | Register Sets & Calling Conventions | Windows x64 ABI (`RCX`, `RDX`, `R8`, `RAX`) | Parameter passing in 64-bit registers, return values via `RAX` |
| **CO2** | 64-Bit Arithmetic & Flag Branching | `ecosort_add`, `ecosort_sub`, `ecosort_mul`, `ecosort_div`, `ecosort_is_high_priority` | `ADD`, `SUB`, `MUL`, `DIV`, `CMP`, `JGE`, `SETGE`, ALU flags `SF`, `OF`, `ZF` |
| **CO2/PSOOP CO4** | Rule Decision Evaluation | `ecosort_classify_waste` | Decision branching based on category codes and weight limit (10 kg) |
| **CO3** | Algorithmic & String Operations | `ecosort_multiply_successive`, `ecosort_string_length`, `ecosort_string_equals` | Successive addition loop with `DEC`/`JNZ`, byte pointer increment and zero-byte detection |
| **CO4** | Integration & Control Unit Flow | `control_functions.asm`, `ecosort_coa_driver.cpp` | Instruction cycle stages (Fetch, Decode, Execute, Store), live shared-memory bus listener |

---

## 📂 Directory Structure

```text
COA/
│
├── README.md                          # COA module master documentation
├── READMECO2.md                       # Detailed CO2 viva explanation
├── build_coa.bat                      # Builds driver (ecosort_coa.exe) and standalone (ecosort.exe)
├── run_coa.bat                        # Launches real-time COA telemetry console
├── ecosort.asm                        # Standalone 64-bit pure Assembly source with test cases
├── ecosort.obj                        # NASM object file for standalone program
├── ecosort.exe                        # Compiled standalone Assembly binary
├── ecosort_functions.asm              # Callable 64-bit NASM routines for C++ linking
├── ecosort_functions.obj              # Object file linked with driver
├── ecosort_coa_driver.cpp             # C++ integration driver and live IPC listener
├── ecosort_coa.exe                    # Statically linked COA telemetry executable
│
├── CO3_Algorithm_String/              # Dedicated CO3 algorithmic & string sub-module
│   ├── README.md
│   ├── build_co3_algorithm_string.bat
│   ├── run_co3_algorithm_string.bat
│   ├── algorithm_string_functions.asm
│   ├── main.cpp
│   └── coa_co3_algorithm_string.exe
│
└── CO4_Integration/                   # Dedicated CO4 processor integration sub-module
    ├── README.md
    ├── build_co4_integration.bat
    ├── run_co4_integration.bat
    ├── control_functions.asm
    ├── main.cpp
    └── coa_co4_integration.exe
```

---

## 🧮 Numerical & Decision Processing

### 1. Arithmetic Processing
```text
Laptop Weight      = 10 kg
Mobile Weight      = 5 kg
Recycling Rate     = ₹50/kg
Priority Threshold = 10 kg
```

| Operation | Calculation | NASM Instruction | Result | Register |
| :--- | :--- | :--- | :--- | :--- |
| **Addition** | 10 + 5 | `add rax, rcx` | 15 kg | `RAX` = 15 |
| **Subtraction** | 15 - 10 | `sub rax, rcx` | 5 kg remaining | `RAX` = 5 |
| **Multiplication** | 15 × 50 | `mul rcx` | ₹750 value | `RAX` = 750 (`RDX:RAX`) |
| **Division** | 15 / 2 | `xor rdx, rdx; div rcx` | 7 kg average | `RAX` = 7 (Quotient), `RDX` = 1 (Remainder) |
| **Comparison** | 15 ≥ 10 | `cmp rax, 10; jge ...` | High Priority | Flags: `SF == OF` |

### 2. Category Decision Evaluation (`ecosort_classify_waste`)
Directly mapped from the EcoSort PSOOP CO4 decision-making model:

| Test Case | Category Code (`RCX`) | Weight (`RDX`) | Threshold (`R8`) | Assembly Result (`RAX`) | Action Protocol |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Case 1** | 1 (Reusable) | 3 kg | 10 kg | `1` | Check if it can be reused or repaired |
| **Case 2** | 2 (Recyclable) | 12 kg | 10 kg | `3` | Send to priority recycling batch (Weight ≥ 10 kg) |
| **Case 3** | 3 (Hazardous) | 2 kg | 10 kg | `4` | Send for special hazardous-waste handling (Quarantine) |

---

## ⚡ Build and Run Instructions

### 1. Build All COA Binaries
From the `EcoSort386/COA/` folder:
```cmd
build_coa.bat
```
This builds:
- `ecosort_coa.exe`: Statically linked C++ driver with real-time IPC listener and NASM functions.
- `ecosort.exe`: Pure 64-bit standalone Assembly executable with printf output.

### 2. Run the Real-Time Telemetry Listener
```cmd
run_coa.bat
```
*(Or launch `ecosort_coa.exe` directly)*.

### 3. Run Dedicated CO3 & CO4 Demonstrations
```cmd
cd CO3_Algorithm_String && run_co3_algorithm_string.bat
cd ..\CO4_Integration && run_co4_integration.bat
```

---

## 🧠 Viva Voce Questions & Answers

### Q1: What calling convention is used by the Assembly routines?
**Answer**: The Windows x64 calling convention (Microsoft x64 ABI). The first four integer arguments are passed in `RCX`, `RDX`, `R8`, and `R9`. Return values are placed in the `RAX` register. 32 bytes of shadow store are allocated by the caller when calling subroutines.

### Q2: How does `ecosort_classify_waste` evaluate decisions in hardware registers?
**Answer**: `RCX` holds the category code (1=Reusable, 2=Recyclable, 3=Hazardous), `RDX` holds weight, and `R8` holds threshold (10 kg). `CMP RCX, 1` jumps to the reusable handler on zero flag (`ZF=1`). `CMP RCX, 3` jumps to the hazardous handler. For recyclable items, `CMP RDX, R8` followed by `JGE` checks if weight ≥ 10 kg, branching to priority recycling (`RAX=3`) or normal recycling (`RAX=2`).

### Q3: Why is static linking (`-static`) preferred for the executables?
**Answer**: Static linking packages the runtime dependencies directly inside the executable, ensuring it runs reliably on any Windows machine without requiring external MSYS2/MinGW DLL paths.

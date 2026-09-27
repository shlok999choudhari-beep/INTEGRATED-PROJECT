# EcoSort 386 – COA / CO2 Assembly Implementation

**Good morning sir/mam.**

I will explain the **COA implementation of our EcoSort 386 project**, specifically the **CO2 Assembly Language implementation**.

The main purpose of the COA module is to demonstrate how numerical computations of our EcoSort application can be performed at the **processor level using 64-bit Assembly language**.

### 1. 64-bit Assembly

Our program starts with:

```asm
BITS 64
```

This tells NASM to generate **64-bit code**.

We also use:

```asm
default rel
```

for relative addressing of data.

The program is connected with the C runtime using:

```asm
global main
extern printf
```

`main` is the entry point of our program, and `printf` is used to display the results.

---

### 2. Data Section

We use:

```asm
section .data
```

to store the output messages.

For example:

```asm
total_msg db "Total E-Waste Weight : %lld kg", 10, 0
```

Here, `db` defines the message as a sequence of bytes.

`10` represents a new line and `0` represents the null terminator.

We use `%lld` because our calculations use 64-bit integer values.

---

### 3. Use of 64-bit Registers

The main registers used in our program are:

* **RAX** – mainly used for operands and arithmetic results.
* **RCX** – used for another operand and function arguments.
* **RDX** – used for multiplication/division and passing the result for display.

For example:

```asm
mov rax, 10
mov rcx, 5
```

Here, 10 is loaded into RAX and 5 into RCX.

These are **64-bit registers**, which satisfies the 64-bit data processing requirement.

---

### 4. Addition

For calculating the total e-waste weight, we use:

```asm
mov rax, 10
mov rcx, 5
add rax, rcx
```

The operation performed is:

```text
10 + 5 = 15 kg
```

The `ADD` instruction performs the arithmetic operation.

The result is stored in **RAX**.

So this represents the calculation of total e-waste weight.

---

### 5. Subtraction

Next, we demonstrate subtraction:

```asm
mov rax, 15
mov rcx, 10
sub rax, rcx
```

This performs:

```text
15 - 10 = 5 kg
```

The result is again stored in RAX.

This can represent the remaining weight after removing a particular waste category.

---

### 6. Multiplication

For calculating the estimated recycling value, we use:

```asm
mov rax, 15
mov rcx, 50
mul rcx
```

This performs:

```text
15 × 50 = 750
```

So the estimated recycling value is:

```text
₹750
```

The `MUL` instruction performs multiplication.

For multiplication, the processor uses the **RDX:RAX register pair** for the result, and in our case the result is small enough to be present in RAX.

---

### 7. Division

We then perform division:

```asm
mov rax, 15
mov rcx, 2
xor rdx, rdx
div rcx
```

This calculates:

```text
15 / 2 = 7
```

So the average weight is displayed as:

```text
7 kg
```

Before division, we use:

```asm
xor rdx, rdx
```

because the division instruction uses the combined **RDX:RAX** value as the dividend.

After division:

* RAX contains the quotient.
* RDX contains the remainder.

In our example, the quotient is **7** and the remainder is **1**.

---

### 8. Comparison

The next important CO2 requirement is comparison.

We use:

```asm
mov rax, 15
cmp rax, 10
```

Here, we compare:

```text
15 with 10
```

The `CMP` instruction performs a comparison by internally setting processor flags based on the subtraction.

It does not directly store the subtraction result.

---

### 9. Conditional Branching

After comparison, we use:

```asm
jge high_priority
```

`JGE` means **Jump if Greater than or Equal**.

Our condition is:

```text
15 >= 10
```

Since the condition is true, execution jumps to:

```asm
high_priority:
```

and displays:

```text
Priority : HIGH PRIORITY
```

If the weight had been less than 10 kg, the program would execute the normal-priority section instead.

This demonstrates **conditional branching**, which is an important part of our CO2 implementation.

---

### 10. Processor-Level Mapping

The Assembly instructions can be mapped to processor components:

| Instruction | Purpose          | Processor concept |
| ----------- | ---------------- | ----------------- |
| `MOV`       | Load data        | Registers         |
| `ADD`       | Addition         | ALU               |
| `SUB`       | Subtraction      | ALU               |
| `MUL`       | Multiplication   | ALU               |
| `DIV`       | Division         | ALU               |
| `CMP`       | Comparison       | ALU + Flags       |
| `JGE`       | Conditional jump | Control Unit      |
| `CALL`      | Function call    | Control flow      |
| `RET`       | Return           | Control flow      |

So our program demonstrates the interaction between **registers, ALU, flags and control flow**.

---

### 11. EcoSort Output

When we execute the program, we get:

```text
Total E-Waste Weight : 15 kg
Remaining Weight     : 5 kg
Estimated Value      : Rs 750
Average Weight       : 7 kg
Priority             : HIGH PRIORITY
```

These calculations represent numerical processing relevant to our EcoSort application.

---

### 12. Compilation and Execution

We use **NASM** to assemble the Assembly source code:

```text
ecosort.asm
     ↓
NASM
     ↓
ecosort.obj
```

Then we use **GCC** to link the object file:

```text
ecosort.obj
     ↓
GCC
     ↓
ecosort.exe
```

Finally, we execute the program and verify the output in the VS Code terminal.

---

### 13. CO2 Requirement

Therefore, our implementation covers the required CO2 concepts:

* **64-bit Assembly**
* **64-bit registers**
* **Numerical computations**
* **Arithmetic instructions**
* **Comparison**
* **Conditional branching**
* **Application-related processing**

We have implemented more than the minimum two numerical operations by demonstrating **addition, subtraction, multiplication and division**.

### Conclusion

So, the COA module demonstrates how EcoSort's numerical processing can be performed at the **processor level using 64-bit Assembly language**.

The current Assembly program is independently tested and working. In the next stage, we can integrate this Assembly module with the **main C++ EcoSort application**, where the values will come from our actual OOP and Data Structure modules instead of being hard-coded.

**Thank you.**

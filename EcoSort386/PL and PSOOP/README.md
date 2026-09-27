# EcoSort 386 — Programming Laboratory (PL) & PSOOP
## Integrated Data Structures & Object-Oriented Segregation Pipeline

---

### 📋 Academic Practical Information
- **Course Outcomes Covered**:
  - **PSOOP CO1 & CO2**: Object-Oriented Class Design, Inheritance, and Runtime Polymorphism (`Waste` base class with virtual methods, derived `Reusable`, `Recyclable`, `Hazardous`).
  - **PL CO2**: Singly Linked List (`WasteList`) for dynamic registration of citizen collection requests.
  - **PL CO3**: FIFO Queue (`Queue`) for chronological dispatching and LIFO Stacks (`Stack`) for physical category bins.
- **Source File**: `PL&PSOOPCO1&2.cpp`
- **Application Context**: **EcoSort 386** — Smart Municipal E-Waste Segregation & Recovery System.

---

## 🎯 Architecture & Workflow

This single integrated program demonstrates the complete flow from citizen request to automated warehouse sorting:

```text
1. Citizen Pickup Request
   │
   ▼
2. Singly Linked List (WasteList)
   │ (Dynamic storage, linear search by ID)
   ▼
3. FIFO Queue (collectionQueue)
   │ (Chronological vehicle dispatch scheduling)
   ▼
4. Classification & Processing Engine
   │ (Polymorphic category evaluation)
   ▼
5. LIFO Stacks (reusableStack, recyclableStack, hazardousStack)
   │ (Physical container loading & recovery staging)
   ▼
6. Stored in EcoSort Central Recovery Depot
```

---

## 🔬 Key Components in `PL&PSOOPCO1&2.cpp`

1. **PSOOP: Abstract Hierarchy & Polymorphism**:
   - `Waste`: Base class with pure virtual functions `category()` and `priority()`.
   - `Reusable`: Derived class (Priority 1).
   - `Recyclable`: Derived class (Priority 2).
   - `Hazardous`: Derived class (Priority 3).

2. **PL: Custom Data Structures**:
   - `WasteList`: Singly linked list (`insert`, `display`, `search`).
   - `Queue`: Array-backed FIFO queue (`enqueue`, `dequeue`).
   - `Stack`: Array-backed LIFO stacks (`push`, `display`).

---

## ⚡ Compilation & Execution

From the `EcoSort386/PL and PSOOP/` directory:
```cmd
build.bat
run.bat
```
*(Or launch `ecosort_pl_psoop.exe` directly)*.

---

## 🧠 Viva Counter-Questions & Answers

### Q1: Why are PL and PSOOP integrated into a single program here?
**Answer**: Real-world software systems do not separate data structures from object-oriented domain models. The OOP hierarchy (`Waste`, `Recyclable`, `Reusable`, `Hazardous`) encapsulates the business attributes and priority logic, while the data structures (Linked List, FIFO Queue, LIFO Stacks) manage the runtime storage and operational pipeline.

### Q2: How does dynamic binding work when `dequeue()` items are sorted?
**Answer**: The Queue returns a `Waste*` pointer. Calling `w->getCategory()` dynamically resolves at runtime via the vtable to the appropriate derived class implementation (`Reusable`, `Recyclable`, or `Hazardous`), allowing clean polymorphic segregation without `typeid` checks.

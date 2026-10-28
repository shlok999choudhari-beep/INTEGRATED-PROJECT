# EcoSort 386 — Programming Laboratory (PL) & PSOOP
## Integrated Data Structures & Object-Oriented Segregation Pipeline

---

### 📋 Course Outcomes (CO) Coverage Matrix

| Subject | Outcome | Concept Demonstrated | Implementation Location |
| :--- | :--- | :--- | :--- |
| **PSOOP** | **CO1** | Base classes, inheritance & runtime polymorphism | `Waste` base class with pure virtual `category()` & `priority()`; derived `Reusable`, `Recyclable`, `Hazardous` |
| **PSOOP** | **CO2** | Modular object-oriented architecture | `EcoSort` system manager orchestrating domain classes, lists, queues, and stacks |
| **PSOOP** | **CO3** | Exception handling & input validation | `try...catch` blocks catching `std::invalid_argument` for non-numeric/empty input, non-positive weight/ID, and duplicate ID check via `hasID()` |
| **PSOOP** | **CO4** | Rule-based decision making | `recommend()` & `testDecisions()` routing items to repair, normal recycling, priority recycling batch (>= 10 kg), or hazardous quarantine |
| **PL** | **CO2** | Dynamic Singly Linked List | `WasteList` managing dynamic heap nodes (`insert`/`add`, `hasID`, `search`, `display`, `totalWeight`, `count`) |
| **PL** | **CO3** | FIFO Queue & LIFO Stacks | Circular `Queue` for chronological vehicle dispatch; array-backed `Stack`s for physical category container loading |
| **PL** | **CO4** | Searching & Sorting | Linear search by ID; Bubble sort by weight (`sortWaste`) using pointer array while preserving linked list integrity |
| **PL** | **CO5** | Hashing & Collision Resolution | `HashTable` with linear probing (`searchHash`), plus `compareSearch` analyzing search efficiency (available in `PL&PSOOPCO1&2.cpp`) |

---

## 📁 Source Implementations

1. **`PL&PSOOPCO1&2.cpp` (Full Integrated Pipeline)**:
   - Covers **PSOOP CO1, CO2, CO3, CO4** and **PL CO2, CO3, CO4, CO5**.
   - Includes real-time cross-process IPC telemetry via `ecosort_bridge.h` to synchronize with CGL Kiosks and COA 64-bit Assembly registers.
   - Executable: `ecosort_pl_psoop.exe`

2. **`EcoSort_without_PL_CO5.cpp` (Standalone Syllabus Version)**:
   - Covers **PSOOP CO1, CO2, CO3, CO4** and **PL CO2, CO3, CO4** (without hash tables).
   - Clean, self-contained educational code following university practical specifications.
   - Executable: `ecosort_without_pl_co5.exe`

---

## ⚡ Compilation & Execution

From the `EcoSort386/PL and PSOOP/` directory:

```cmd
:: Build both targets
build.bat

:: Run the integrated pipeline
run.bat

:: Or run the standalone syllabus pipeline (without PL CO5)
run_without_pl_co5.bat
```

---

## 🧠 Viva Counter-Questions & Answers

### Q1: Why are PL and PSOOP integrated into a single program here?
**Answer**: Real-world software systems do not separate data structures from object-oriented domain models. The OOP hierarchy (`Waste`, `Recyclable`, `Reusable`, `Hazardous`) encapsulates business attributes, validation rules, and priority logic, while data structures (Linked List, FIFO Queue, LIFO Stacks, Hash Table) manage runtime storage and scheduling.

### Q2: How does dynamic binding work when `dequeue()` items are sorted?
**Answer**: The Queue returns a `Waste*` base pointer. Calling `w->category()` dynamically resolves at runtime via the vtable to the derived class implementation (`Reusable`, `Recyclable`, or `Hazardous`), allowing clean polymorphic segregation without explicit `typeid` or `switch` statements.

### Q3: How is PSOOP CO3 (Exception Handling) demonstrated?
**Answer**: In `makeWaste()`, validations verify that ID > 0, name is not empty, weight > 0, category is 1–3, and the ID is unique via `list.hasID(id)`. If any rule is violated, `throw invalid_argument(...)` is triggered, and `addWaste()` catches it via `try ... catch (const exception &e)` to display error diagnostics without program crashes.

### Q4: How is PSOOP CO4 (Decision Making) evaluated?
**Answer**: `recommend()` evaluates the waste category and weight against `PRIORITY_LIMIT` (10.0 kg). Hazardous waste requires special containment, Reusable items prompt reuse/repair inspection, and Recyclable items exceeding 10 kg are routed to a high-priority processing stream.

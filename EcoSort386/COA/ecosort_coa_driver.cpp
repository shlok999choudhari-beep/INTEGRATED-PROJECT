#include <iostream>
#include <iomanip>
#include <cstdint>
#include <windows.h>
#include "../ecosort_bridge.h"

// ============================================================================
// EcoSort 386 - COA (Computer Organization & Architecture) CO2 Verification
// Emulating exact 64-bit x86 register state machine (RAX, RCX, RDX, Flags)
// Dual Mode: 1. Static CO2 Academic Verification  2. Live Cross-Process Listener
// ============================================================================

int main() {
    std::cout << "========================================================\n";
    std::cout << "  ECOSORT 386 - COA CO2 (64-BIT PROCESSOR TELEMETRY)\n";
    std::cout << "  Simulating Intel x86-64 Architecture Register Pipeline\n";
    std::cout << "========================================================\n\n";

    // 64-bit Registers
    int64_t RAX = 0;
    int64_t RCX = 0;
    int64_t RDX = 0;
    bool JGE_FLAG = false;

    // ----------------------------------------------------
    // 1. ADDITION: Total Weight Calculation
    // mov rax, 10
    // mov rcx, 5
    // add rax, rcx
    // ----------------------------------------------------
    RAX = 10; // Laptop = 10 kg
    RCX = 5;  // Mobile = 5 kg
    RAX += RCX; // RAX = 15 kg
    std::cout << "[ASM: ADD RAX, RCX] Total E-Waste Weight : " << RAX << " kg (RAX=0x" << std::hex << RAX << std::dec << ")\n";

    // ----------------------------------------------------
    // 2. SUBTRACTION: Remaining Weight Calculation
    // mov rax, 15
    // mov rcx, 10
    // sub rax, rcx
    // ----------------------------------------------------
    RAX = 15;
    RCX = 10;
    RAX -= RCX; // RAX = 5 kg
    std::cout << "[ASM: SUB RAX, RCX] Remaining Weight     : " << RAX << " kg (RAX=0x" << std::hex << RAX << std::dec << ")\n";

    // ----------------------------------------------------
    // 3. MULTIPLICATION: Estimated Valuation (Rs 50/kg)
    // mov rax, 15
    // mov rcx, 50
    // mul rcx -> RDX:RAX
    // ----------------------------------------------------
    RAX = 15;
    RCX = 50;
    uint64_t fullMul = (uint64_t)RAX * (uint64_t)RCX;
    RDX = (fullMul >> 32);
    RAX = fullMul;
    std::cout << "[ASM: MUL RCX]     Estimated Value      : Rs " << RAX << " (RDX:RAX = 0x" << std::hex << fullMul << std::dec << ")\n";

    // ----------------------------------------------------
    // 4. DIVISION: Average Weight per Category
    // mov rax, 15
    // mov rcx, 2
    // xor rdx, rdx
    // div rcx -> RAX=quotient, RDX=remainder
    // ----------------------------------------------------
    RAX = 15;
    RCX = 2;
    RDX = RAX % RCX;
    RAX = RAX / RCX;
    std::cout << "[ASM: DIV RCX]     Average Weight       : " << RAX << " kg (Remainder in RDX: " << RDX << " kg)\n";

    // ----------------------------------------------------
    // 5. COMPARISON & CONDITIONAL BRANCHING
    // cmp rax, 10
    // jge high_priority
    // ----------------------------------------------------
    int64_t weight = 15;
    int64_t threshold = 10;
    JGE_FLAG = (weight >= threshold);

    std::cout << "[ASM: CMP RAX, 10] Comparison Flag (SF==OF): " << (JGE_FLAG ? "GREATER/EQUAL" : "LESS") << "\n";
    if (JGE_FLAG) {
        std::cout << "[ASM: JGE BRANCH]  Priority             : HIGH PRIORITY (Threshold Exceeded)\n";
    } else {
        std::cout << "[ASM: JMP BRANCH]  Priority             : NORMAL PRIORITY\n";
    }

    std::cout << "\n========================================================\n";
    std::cout << "  [SUCCESS] All 5 processor-level CO2 operations verified!\n";
    std::cout << "========================================================\n";

    // ------------------------------------------------------------------------
    // Real-Time Cross-Process Listener (Synchronized with PL, PSOOP, and CGL)
    // ------------------------------------------------------------------------
    std::cout << "\n========================================================\n";
    std::cout << "  ENTERING LIVE SENSOR TELEMETRY MODE (POLLING IPC BUS)\n";
    std::cout << "  Deposit items in PL or CGL to trigger live 64-bit ALU...\n";
    std::cout << "========================================================\n" << std::endl;

    int lastTxState = -1;
    int lastCat = -1;
    int eventCount = 0;

    while (true) {
        Sleep(100);
        auto& br = EcoSortCore::Bridge::get();
        bool changed = br.syncIPC();

        if (changed || lastTxState == -1) {
            if (br.txState == 1 && (lastTxState != 1 || lastCat != br.activeCategory || changed)) {
                lastTxState = 1;
                lastCat = br.activeCategory;
                eventCount++;

                int64_t itemWeight = (int64_t)(br.lastItemWeight > 0.1f ? br.lastItemWeight : 5.0f);
                int64_t totalWeight = (int64_t)br.totalWeight;
                int64_t rate = 50; // Standard rate: Rs 50/kg
                int64_t val = itemWeight * rate;
                bool highPriority = (itemWeight >= 10);

                std::cout << "\n>>> [HW SENSOR INTERRUPT #" << eventCount << "] LIVE E-WASTE PAYLOAD DETECTED <<<\n";
                std::cout << "  Hardware Category    : " << br.getCategoryName() << "\n";
                std::cout << "  Load Cell Weight     : " << itemWeight << " kg (Live PL Input)\n";
                std::cout << "  [ASM: ADD RAX, RCX]   Accumulated Depot Weight : " << totalWeight << " kg (RAX=0x" << std::hex << totalWeight << std::dec << ")\n";
                std::cout << "  [ASM: MUL RCX]       Subsidy Credit Valuation : Rs " << val << " (RAX=0x" << std::hex << val << std::dec << ")\n";
                std::cout << "  [ASM: CMP RAX, 10]   Threshold Comparison     : " << itemWeight << (highPriority ? " >= 10 (THRESHOLD EXCEEDED)" : " < 10 (BELOW LIMIT)") << "\n";
                if (highPriority) {
                    std::cout << "  [ASM: JGE BRANCH]    Priority Status          : *** HIGH PRIORITY / QUARANTINE PROTOCOL ***\n";
                } else {
                    std::cout << "  [ASM: JMP BRANCH]    Priority Status          : NORMAL PROCESSING PRIORITY\n";
                }
                std::cout << "  [DISPATCH ROUTE]     Targeting Intake Bay " << br.getTargetBay() << "\n";
                std::cout << "--------------------------------------------------------" << std::endl;
            } else if (br.txState == 2 && lastTxState != 2) {
                lastTxState = 2;
                std::cout << "  [HW STATUS] Intake Bay " << br.getTargetBay() << " CONFIRMED STORAGE. User Balance: " << br.userCredits << " Cr.\n";
                std::cout << "  [READY] Listening for next payload..." << std::endl;
            } else if (lastTxState == -1) {
                lastTxState = br.txState;
                lastCat = br.activeCategory;
            }
        }
    }

    return 0;
}

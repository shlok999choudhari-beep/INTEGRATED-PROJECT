#include <iostream>
#include <iomanip>
#include <cstdint>
#include <windows.h>
#include "../ecosort_bridge.h"

extern "C" long long ecosort_add(long long first, long long second);
extern "C" long long ecosort_sub(long long first, long long second);
extern "C" long long ecosort_mul(long long first, long long second);
extern "C" long long ecosort_div(long long numerator, long long denominator);
extern "C" int ecosort_is_high_priority(long long weight, long long threshold);
extern "C" int ecosort_classify_waste(long long category, long long weight, long long threshold);

// ============================================================================
// EcoSort 386 - COA (Computer Organization & Architecture) CO2 Verification
// Emulating exact 64-bit x86 register state machine (RAX, RCX, RDX, Flags)
// Dual Mode: 1. Static CO2 Academic Verification  2. Live Cross-Process Listener
// ============================================================================

int main() {
    std::cout << "========================================================\n";
    std::cout << "  ECOSORT 386 - COA CO2 (64-BIT PROCESSOR TELEMETRY)\n";
    std::cout << "  Executing 64-bit NASM functions through the C++ integration driver\n";
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
    RAX = ecosort_add(RAX, RCX); // Actual 64-bit Assembly function: RAX = 15 kg
    std::cout << "[ASM: ADD RAX, RCX] Total E-Waste Weight : " << RAX << " kg (RAX=0x" << std::hex << RAX << std::dec << ")\n";

    // ----------------------------------------------------
    // 2. SUBTRACTION: Remaining Weight Calculation
    // mov rax, 15
    // mov rcx, 10
    // sub rax, rcx
    // ----------------------------------------------------
    RAX = 15;
    RCX = 10;
    RAX = ecosort_sub(RAX, RCX); // Actual 64-bit Assembly function: RAX = 5 kg
    std::cout << "[ASM: SUB RAX, RCX] Remaining Weight     : " << RAX << " kg (RAX=0x" << std::hex << RAX << std::dec << ")\n";

    // ----------------------------------------------------
    // 3. MULTIPLICATION: Estimated Valuation (Rs 50/kg)
    // mov rax, 15
    // mov rcx, 50
    // mul rcx -> RDX:RAX
    // ----------------------------------------------------
    RAX = 15;
    RCX = 50;
    RAX = ecosort_mul(RAX, RCX); // Actual 64-bit Assembly function
    uint64_t fullMul = (uint64_t)RAX; // Low 64 bits are enough for this sample value
    RDX = 0;
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
    RDX = RAX % RCX; // Remainder shown for teaching
    RAX = ecosort_div(RAX, RCX); // Quotient calculated by Assembly
    std::cout << "[ASM: DIV RCX]     Average Weight       : " << RAX << " kg (Remainder in RDX: " << RDX << " kg)\n";

    // ----------------------------------------------------
    // 5. COMPARISON & CONDITIONAL BRANCHING
    // cmp rax, 10
    // jge high_priority
    // ----------------------------------------------------
    int64_t weight = 15;
    int64_t threshold = 10;
    JGE_FLAG = (ecosort_is_high_priority(weight, threshold) != 0);

    std::cout << "[ASM: CMP RAX, 10] Comparison Flag (SF==OF): " << (JGE_FLAG ? "GREATER/EQUAL" : "LESS") << "\n";
    if (JGE_FLAG) {
        std::cout << "[ASM: JGE BRANCH]  Priority             : HIGH PRIORITY (Threshold Exceeded)\n";
    } else {
        std::cout << "[ASM: JMP BRANCH]  Priority             : NORMAL PRIORITY\n";
    }

    // ----------------------------------------------------
    // 6. CATEGORY DECISION CLASSIFIER (PSOOP CO4 Integration)
    // ecosort_classify_waste(RCX=cat, RDX=weight, R8=threshold) -> RAX
    // ----------------------------------------------------
    std::cout << "\n========================================================\n";
    std::cout << "  COA + PSOOP CO4 DECISION EVALUATION IN ASSEMBLY\n";
    std::cout << "========================================================\n";

    struct TestCase {
        const char* name;
        int64_t cat;
        int64_t weight;
    } tests[] = {
        {"Reusable", EcoSortCore::CAT_REUSABLE, 3},
        {"Recyclable", EcoSortCore::CAT_RECYCLABLE, 12},
        {"Hazardous", EcoSortCore::CAT_HAZARDOUS, 2}
    };

    for (const auto& tc : tests) {
        int code = ecosort_classify_waste(tc.cat, tc.weight, threshold);
        std::cout << "  Input: " << tc.name << ", " << tc.weight << " kg -> [ASM RAX=" << code << "] ";
        if (code == 1) {
            std::cout << "Check if it can be reused or repaired.\n";
        } else if (code == 2) {
            std::cout << "Add to normal recycling batch.\n";
        } else if (code == 3) {
            std::cout << "Send to priority recycling batch.\n";
        } else if (code == 4) {
            std::cout << "Send for special hazardous-waste handling.\n";
        }
    }

    std::cout << "\n========================================================\n";
    std::cout << "  [SUCCESS] Arithmetic, priority & decision checks executed by NASM!\n";
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
                int64_t val = ecosort_mul(itemWeight, rate);
                int decisionCode = ecosort_classify_waste(br.activeCategory, itemWeight, 10);
                bool highPriority = (decisionCode == 3);

                std::cout << "\n>>> [HW SENSOR INTERRUPT #" << eventCount << "] LIVE E-WASTE PAYLOAD DETECTED <<<\n";
                std::cout << "  Hardware Category    : " << br.getCategoryName() << "\n";
                std::cout << "  Load Cell Weight     : " << itemWeight << " kg (Live PL Input)\n";
                std::cout << "  [ASM: ADD RAX, RCX]   Accumulated Depot Weight : " << totalWeight << " kg (RAX=0x" << std::hex << totalWeight << std::dec << ")\n";
                std::cout << "  [ASM: MUL RCX]       Subsidy Credit Valuation : Rs " << val << " (RAX=0x" << std::hex << val << std::dec << ")\n";
                std::cout << "  [ASM: CLASSIFY]      Decision Code in RAX     : " << decisionCode << "\n";
                
                if (decisionCode == 4) {
                    std::cout << "  [ASM DECISION ROUTE] Protocol: *** SPECIAL HAZARDOUS-WASTE HANDLING (QUARANTINE) ***\n";
                } else if (decisionCode == 1) {
                    std::cout << "  [ASM DECISION ROUTE] Protocol: REUSABLE ITEM - INSPECT FOR REPAIR OR REUSE\n";
                } else if (decisionCode == 3) {
                    std::cout << "  [ASM DECISION ROUTE] Protocol: *** HIGH PRIORITY RECYCLING BATCH (WEIGHT >= 10 KG) ***\n";
                } else {
                    std::cout << "  [ASM DECISION ROUTE] Protocol: NORMAL RECYCLING BATCH\n";
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

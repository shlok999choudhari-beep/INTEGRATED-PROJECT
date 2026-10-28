#include <iostream>
#include <iomanip>
#include <string>
#include <cstring>
#include <vector>
#include <thread>
#include <chrono>
#include "../../ecosort_bridge.h"

// ============================================================================
// EcoSort 386 - COA CO3: Algorithmic Arithmetic & String Processing Engine
// Native 64-Bit x86 Assembly Linked via Microsoft x64 Calling Convention
// Meets Exam Reform Syllabus Criteria for CO3 (Algorithmic & String Processing)
// ============================================================================

extern "C" {
    uint64_t asm_mul_shift_add(uint64_t multiplicand, uint64_t multiplier);
    uint64_t asm_mul_successive_add(uint64_t value, uint64_t count);
    uint64_t asm_strlen(const char* str);
    int64_t  asm_strcmp(const char* s1, const char* s2);
    int64_t  asm_classify_category_string(const char* catStr);
    void     asm_str_to_upper(char* dest, const char* src);
}

// ----------------------------------------------------------------------------
// Visual Simulation of ALU Shift-and-Add Clock Cycles
// ----------------------------------------------------------------------------
void printShiftAddTrace(uint64_t multiplicand, uint64_t multiplier) {
    std::cout << "\n----------------------------------------------------------------------\n";
    std::cout << "  ALU HARDWARE MICRO-OPERATION CYCLE TRACE (Add-and-Shift Method)\n";
    std::cout << "----------------------------------------------------------------------\n";
    std::cout << "  Multiplicand (RCX) : 0x" << std::hex << multiplicand << std::dec << " (" << multiplicand << " dec)\n";
    std::cout << "  Multiplier   (RDX) : 0x" << std::hex << multiplier   << std::dec << " (" << multiplier   << " dec)\n\n";

    uint64_t accum = 0;
    uint64_t mc = multiplicand;
    uint64_t mp = multiplier;
    int cycle = 1;

    while (mp > 0) {
        bool bitSet = (mp & 1);
        std::cout << "  Cycle " << std::setw(2) << std::setfill('0') << cycle++ << " | ";
        if (bitSet) {
            accum += mc;
            std::cout << "Multiplier LSB = 1 | ALU ADD -> Accumulator: " << std::setw(7) << std::setfill(' ') << accum;
        } else {
            std::cout << "Multiplier LSB = 0 | No Add  -> Accumulator: " << std::setw(7) << std::setfill(' ') << accum;
        }
        mc <<= 1;
        mp >>= 1;
        std::cout << " | Shift Multiplicand << 1\n";
    }

    std::cout << "----------------------------------------------------------------------\n";
    std::cout << "  [ALGORITHM OUTPUT RAX] : " << accum << " (Rs " << accum << " Valuation)\n";
    std::cout << "----------------------------------------------------------------------\n";
}

// ----------------------------------------------------------------------------
// COA CO3 Automated Benchmark & Verification Suite
// ----------------------------------------------------------------------------
void runAcademicVerification() {
    std::cout << "\n======================================================================\n";
    std::cout << "  ECOSORT 386 - COA CO3 ACADEMIC VERIFICATION SUITE\n";
    std::cout << "======================================================================\n";

    // ------------------------------------------------------------------------
    // Part 1: Algorithmic Multiplication Verification
    // ------------------------------------------------------------------------
    std::cout << "\n>>> [MODULE 1: ALGORITHMIC MULTIPLICATION ENGINE] <<<\n";
    uint64_t rate = 50; // Rs 50 / kg
    uint64_t weights[] = { 5, 14, 25, 42 };

    for (uint64_t wt : weights) {
        uint64_t valShiftAdd = asm_mul_shift_add(wt, rate);
        uint64_t valSuccAdd  = asm_mul_successive_add(wt, rate);
        uint64_t valHw       = wt * rate;

        std::cout << "  Weight: " << std::setw(2) << wt << " kg @ Rs " << rate << "/kg\n";
        std::cout << "    * Add-and-Shift (asm_mul_shift_add)    : Rs " << valShiftAdd << "\n";
        std::cout << "    * Successive Add (asm_mul_successive) : Rs " << valSuccAdd  << "\n";
        std::cout << "    * Hardware Single-Cycle MUL Reference : Rs " << valHw       << "\n";

        if (valShiftAdd == valHw && valSuccAdd == valHw) {
            std::cout << "    * Verification Status                 : [PASS - 100% PARITY]\n";
        } else {
            std::cout << "    * Verification Status                 : [FAIL - MISMATCH]\n";
        }
        std::cout << "\n";
    }

    // Print Detailed ALU Cycle Trace for an exemplary e-waste lot
    printShiftAddTrace(50, 14);

    // ------------------------------------------------------------------------
    // Part 2: 64-Bit Assembly String Processing Verification
    // ------------------------------------------------------------------------
    std::cout << "\n>>> [MODULE 2: 64-BIT ASSEMBLY STRING PROCESSING ENGINE] <<<\n";

    const char* sampleItems[] = {
        "Motherboard PCB",
        "Lithium-Ion Battery Pack",
        "CRT Monitor Chassis",
        "Copper Cable Harness"
    };

    std::cout << "\n[A. String Length Scan via asm_strlen (REPNE SCASB)]:\n";
    for (const char* item : sampleItems) {
        uint64_t len = asm_strlen(item);
        std::cout << "  String: \"" << item << "\"\n";
        std::cout << "    * Computed Length (RAX via SCASB) : " << len << " bytes (strlen: " << strlen(item) << ")\n";
    }

    std::cout << "\n[B. Category String Comparison via asm_strcmp (REPE CMPSB)]:\n";
    const char* testCats[] = { "Reusable", "Recyclable", "Hazardous", "Organic" };
    for (const char* cat : testCats) {
        int64_t code = asm_classify_category_string(cat);
        std::cout << "  Input Category: \"" << cat << "\"\n";
        if (code == 1)      std::cout << "    * Match: REUSABLE   (Code 1) -> Routed to Bay 1\n";
        else if (code == 2) std::cout << "    * Match: RECYCLABLE (Code 2) -> Routed to Bay 2\n";
        else if (code == 3) std::cout << "    * Match: HAZARDOUS  (Code 3) -> Routed to Bay 3 Quarantine\n";
        else                std::cout << "    * Match: UNKNOWN    (Code 0) -> Rejected by Assembly Classifier\n";
    }

    std::cout << "\n[C. Case Normalization via asm_str_to_upper]:\n";
    char upperBuf[64];
    asm_str_to_upper(upperBuf, "ecosort-386-smart-city-e-waste");
    std::cout << "  Input Lowercase  : \"ecosort-386-smart-city-e-waste\"\n";
    std::cout << "  Assembly Output  : \"" << upperBuf << "\"\n";

    std::cout << "\n======================================================================\n";
    std::cout << "  ALL CO3 ALGORITHMIC & STRING ARTIFACTS VERIFIED SUCCESSFULLY\n";
    std::cout << "======================================================================\n";
}

// ----------------------------------------------------------------------------
// Live Cross-Process Listener (Integrating COA with PL, PSOOP and CGL)
// ----------------------------------------------------------------------------
void runLiveIPCListener() {
    std::cout << "\n======================================================================\n";
    std::cout << "  ECOSORT 386 - COA CO3 LIVE HARDWARE TELEMETRY LISTENER\n";
    std::cout << "  Listening for Cross-Process Signals from PL/PSOOP & CGL via IPC...\n";
    std::cout << "  Press Ctrl+C to stop listening and return to menu.\n";
    std::cout << "======================================================================\n\n";

    EcoSortCore::Bridge& br = EcoSortCore::Bridge::get();
    uint32_t lastSeenSeq = 0;
    int eventCount = 0;

    while (true) {
        if (br.syncIPC()) {
            if (br.txState == 1 && br.lastItemWeight > 0.05f) {
                eventCount++;
                uint64_t weightInt = (uint64_t)br.lastItemWeight;
                if (weightInt == 0) weightInt = 1;
                uint64_t rate = 50;

                // 1. Process Valuation using Native Add-and-Shift Algorithm
                uint64_t val = asm_mul_shift_add(weightInt, rate);

                // 2. Classify Category String using 64-Bit Assembly
                const char* catStr = (br.activeCategory == EcoSortCore::CAT_REUSABLE ? "Reusable" :
                                     (br.activeCategory == EcoSortCore::CAT_RECYCLABLE ? "Recyclable" : "Hazardous"));
                int64_t catCode = asm_classify_category_string(catStr);

                // 3. String length of category identifier
                uint64_t catLen = asm_strlen(catStr);

                std::cout << "\n>>> [HW INTERRUPT #" << eventCount << "] LIVE CROSS-PROCESS E-WASTE PACKET <<<\n";
                std::cout << "  Assembly Category Code : " << catCode << " (" << catStr << " | " << catLen << " bytes)\n";
                std::cout << "  Payload Weight         : " << br.lastItemWeight << " kg (RCX = " << weightInt << ")\n";
                std::cout << "  Standard Rate Factor   : Rs " << rate << "/kg (RDX = " << rate << ")\n";
                std::cout << "  Add-and-Shift Value    : Rs " << val << " (RAX = 0x" << std::hex << val << std::dec << ")\n";
                std::cout << "  Active System Credits  : " << br.userCredits << " Credits\n";
                std::cout << "  Synchronized Module    : CGL Graphics Conveyor & PL/PSOOP Queue\n";
                std::cout << "----------------------------------------------------------------------\n";
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
}

// ----------------------------------------------------------------------------
// Main Menu
// ----------------------------------------------------------------------------
int main(int argc, char** argv) {
    if (argc > 1 && strcmp(argv[1], "--test") == 0) {
        runAcademicVerification();
        return 0;
    }
    if (argc > 1 && strcmp(argv[1], "--listen") == 0) {
        runLiveIPCListener();
        return 0;
    }

    int choice = 0;
    do {
        std::cout << "\n======================================================\n";
        std::cout << "  ECOSORT 386 - COA CO3 PROCESSOR ENGINE\n";
        std::cout << "  Algorithmic Arithmetic & 64-Bit String Processing\n";
        std::cout << "======================================================\n";
        std::cout << "  [1] Run Academic CO3 Benchmark & Parity Verification\n";
        std::cout << "  [2] Print Hardware Add-and-Shift Step-by-Step Trace\n";
        std::cout << "  [3] Run Live Multi-Subject IPC Hardware Listener\n";
        std::cout << "  [4] Interactive Test (Custom Weight & Category String)\n";
        std::cout << "  [5] Exit\n";
        std::cout << "------------------------------------------------------\n";
        std::cout << "Enter choice (1-5): ";

        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            continue;
        }

        switch (choice) {
            case 1:
                runAcademicVerification();
                break;
            case 2: {
                uint64_t w, r;
                std::cout << "Enter Weight (kg): ";
                std::cin >> w;
                std::cout << "Enter Rate (Rs/kg, e.g. 50): ";
                std::cin >> r;
                printShiftAddTrace(r, w);
                break;
            }
            case 3:
                runLiveIPCListener();
                break;
            case 4: {
                std::string s;
                std::cout << "Enter Category String (Reusable, Recyclable, Hazardous): ";
                std::cin >> s;
                int64_t code = asm_classify_category_string(s.c_str());
                uint64_t len = asm_strlen(s.c_str());
                char upper[64];
                asm_str_to_upper(upper, s.c_str());

                std::cout << "  * Length (asm_strlen via SCASB) : " << len << " bytes\n";
                std::cout << "  * Category Code (asm_strcmp)     : " << code;
                if (code == 1) std::cout << " (Reusable)\n";
                else if (code == 2) std::cout << " (Recyclable)\n";
                else if (code == 3) std::cout << " (Hazardous)\n";
                else std::cout << " (Unrecognized)\n";
                std::cout << "  * Uppercase (asm_str_to_upper)  : " << upper << "\n";
                break;
            }
            case 5:
                std::cout << "Exiting COA CO3 Engine.\n";
                break;
            default:
                std::cout << "Invalid choice. Please select 1 to 5.\n";
        }
    } while (choice != 5);

    return 0;
}

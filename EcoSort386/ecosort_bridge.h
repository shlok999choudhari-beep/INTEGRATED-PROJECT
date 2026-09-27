#ifndef ECOSORT_BRIDGE_H
#define ECOSORT_BRIDGE_H

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// ============================================================================
// EcoSort 386 - Unified Cross-Subject Integration Bridge (Option B)
// Cross-Process Shared IPC: CGL (Graphics HMI) <--> PL & PSOOP (CLI) <--> COA
// ============================================================================

namespace EcoSortCore {

enum Category {
    CAT_NONE = 0,
    CAT_RECYCLABLE = 1, // Bay 1: PCB, copper, precious metals (+40 Credits)
    CAT_HAZARDOUS  = 2, // Bay 2: Swollen cells, batteries, toxic (+60 Credits)
    CAT_REUSABLE   = 3  // Bay 3: Working monitors, CPUs, peripherals (+80 Credits)
};

// ----------------------------------------------------------------------------
// COA Hardware Emulation: 64-Bit Register Arithmetic & Priority Thresholding
// ----------------------------------------------------------------------------
struct COA_Telemetry {
    int64_t RAX;       // Primary Accumulator (Total weight / Valuation)
    int64_t RCX;       // Operand (Rate multiplier / Division factor)
    int64_t RDX;       // Remainder / High 64-bit product
    bool JGE_Critical; // Flag: High Priority (Weight >= 10 kg)

    static COA_Telemetry compute(int64_t weightKg, int64_t ratePerKg = 50, int64_t thresholdKg = 10) {
        COA_Telemetry t;
        t.RAX = weightKg;
        t.RCX = ratePerKg;
        t.RAX *= t.RCX;         // MUL RCX -> Estimated Value in Rs
        t.RDX = weightKg % 2;   // DIV RCX -> Average category remainder
        t.JGE_Critical = (weightKg >= thresholdKg); // CMP RAX, 10 -> JGE
        return t;
    }
};

// ----------------------------------------------------------------------------
// Cross-Process IPC Payload
// ----------------------------------------------------------------------------
struct BridgePacket {
    int activeCategory;
    int txState; // 0: Idle/Select, 1: Dispatched, 2: Completed
    int userCredits;
    int queueLength;
    uint32_t seq;
};

// ----------------------------------------------------------------------------
// Central Synchronized State Bridge (Cross-Process File IPC)
// ----------------------------------------------------------------------------
class Bridge {
private:
    uint32_t localSeq;
    uint32_t lastReadSeq;

    Bridge() {
        activeCategory = CAT_RECYCLABLE;
        txState = 0;
        userCredits = 140;
        truckX = -0.05f;
        destX = -0.45f;
        totalRecycledKg = 84.5f;
        totalHazardKg = 32.0f;
        totalReusableKg = 48.0f;
        queueLength = 3;
        localSeq = 1;
        lastReadSeq = 0;
        saveIPC();
    }

    static const char* getIPCFilePath() {
        static char fpath[512] = "";
        if (fpath[0] == '\0') {
            const char* tmp = getenv("TEMP");
            if (!tmp) tmp = "C:\\Temp";
            snprintf(fpath, sizeof(fpath), "%s\\ecosort_bridge_state.bin", tmp);
        }
        return fpath;
    }

public:
    int activeCategory;
    int txState;
    int userCredits;
    float truckX;
    float destX;
    float totalRecycledKg;
    float totalHazardKg;
    float totalReusableKg;
    int queueLength;

    static Bridge& get() {
        static Bridge instance;
        return instance;
    }

    void saveIPC() {
        FILE* f = fopen(getIPCFilePath(), "wb");
        if (f) {
            BridgePacket p;
            p.activeCategory = activeCategory;
            p.txState = txState;
            p.userCredits = userCredits;
            p.queueLength = queueLength;
            p.seq = ++localSeq;
            fwrite(&p, sizeof(p), 1, f);
            fclose(f);
        }
    }

    bool syncIPC() {
        FILE* f = fopen(getIPCFilePath(), "rb");
        if (f) {
            BridgePacket p;
            if (fread(&p, sizeof(p), 1, f) == 1) {
                if (p.seq != lastReadSeq && p.seq != localSeq) {
                    lastReadSeq = p.seq;
                    activeCategory = p.activeCategory;
                    userCredits = p.userCredits;
                    queueLength = p.queueLength;
                    txState = p.txState;
                    if (p.txState == 1) {
                        truckX = getSpawnX();
                        destX = getBayX();
                    }
                    fclose(f);
                    return true;
                }
            }
            fclose(f);
        }
        return false;
    }

    void setCategory(int cat) {
        if (cat >= 1 && cat <= 3) {
            activeCategory = cat;
            txState = 0;
            saveIPC();
        }
    }

    const char* getCategoryName() const {
        if (activeCategory == CAT_RECYCLABLE) return "RECYCLABLE (PCBs & Copper)";
        if (activeCategory == CAT_HAZARDOUS)  return "HAZARDOUS (Li-Ion Batteries)";
        if (activeCategory == CAT_REUSABLE)   return "REUSABLE (Monitors & CPUs)";
        return "GENERAL E-WASTE";
    }

    int getCategoryReward() const {
        if (activeCategory == CAT_RECYCLABLE) return 40;
        if (activeCategory == CAT_HAZARDOUS)  return 60;
        if (activeCategory == CAT_REUSABLE)   return 80;
        return 20;
    }

    int getTargetBay() const {
        return activeCategory;
    }

    float getBayX() const {
        if (activeCategory == CAT_RECYCLABLE) return -0.45f;
        if (activeCategory == CAT_HAZARDOUS)  return -0.10f;
        return 0.30f;
    }

    float getSpawnX() const {
        if (activeCategory == CAT_RECYCLABLE) return -0.90f;
        if (activeCategory == CAT_HAZARDOUS)  return -0.28f;
        return 0.65f;
    }

    void triggerDeposit() {
        txState = 1;
        truckX = getSpawnX();
        destX = getBayX();
        queueLength++;
        saveIPC();
    }

    void completeDeposit() {
        txState = 2;
        truckX = destX;
        userCredits += getCategoryReward();
        if (queueLength > 0) queueLength--;
        if (activeCategory == CAT_RECYCLABLE) totalRecycledKg += 2.5f;
        else if (activeCategory == CAT_HAZARDOUS) totalHazardKg += 1.8f;
        else totalReusableKg += 4.2f;
        saveIPC();
    }
};

} // namespace EcoSortCore

#endif // ECOSORT_BRIDGE_H

#include <windows.h>
#include <GLFW/glfw3.h>
#include <GL/glu.h>
#include <cstdio>
#include <cstring>
#include <vector>
#include <string>
#include "../ecosort_bridge.h"

using namespace EcoSortCore;

// ============================================================================
// EcoSort 386 - Dedicated Master Graphical Control Interface
// Clean 2D Computer Graphics Control Panel (No Glow, No Hover Complexity)
// Synchronized bidirectionally with PL & PSOOP CLI, CGL Kiosk, and COA
// ============================================================================

const int WIN_W = 860;
const int WIN_H = 560;
GLuint fontBase = 0;

struct Button {
    int id;
    float x, y, w, h;
    const char* label;
    float r, g, b; // Button fill color
};

std::vector<Button> buttons;
std::vector<std::string> eventLogs;

void addLog(const std::string& msg) {
    if (eventLogs.size() >= 7) {
        eventLogs.erase(eventLogs.begin());
    }
    eventLogs.push_back(msg);
}

void initFont() {
    HDC hdc = wglGetCurrentDC();
    HFONT font = CreateFontA(15, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                             ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
                             ANTIALIASED_QUALITY, FF_DONTCARE, "Arial");
    SelectObject(hdc, font);
    fontBase = glGenLists(128);
    wglUseFontBitmaps(hdc, 0, 128, fontBase);
}

void drawText(float x, float y, const char* str, float r = 1.0f, float g = 1.0f, float b = 1.0f) {
    glColor3f(r, g, b);
    glRasterPos2f(x, y);
    glListBase(fontBase);
    glCallLists((GLsizei)strlen(str), GL_UNSIGNED_BYTE, str);
}

void drawSolidRect(float x, float y, float w, float h, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + w, y);
        glVertex2f(x + w, y + h);
        glVertex2f(x, y + h);
    glEnd();
}

void drawRectBorder(float x, float y, float w, float h, float r, float g, float b, float lineWidth = 1.5f) {
    glColor3f(r, g, b);
    glLineWidth(lineWidth);
    glBegin(GL_LINE_LOOP);
        glVertex2f(x, y);
        glVertex2f(x + w, y);
        glVertex2f(x + w, y + h);
        glVertex2f(x, y + h);
    glEnd();
}

void initButtons() {
    buttons.clear();
    // Simple, clean rectangular command buttons corresponding to PL & PSOOP functions
    // (x, y, w, h) in normalized coordinates [-1.0, 1.0]
    float bx = -0.92f;
    float bw = 0.88f;
    float bh = 0.11f;

    buttons.push_back({1, bx,  0.42f, bw, bh, "1. Add Recyclable Waste (PCBs / Copper)", 0.15f, 0.35f, 0.60f});
    buttons.push_back({2, bx,  0.27f, bw, bh, "2. Add Hazardous Waste (Li-Ion Battery)",  0.65f, 0.22f, 0.22f});
    buttons.push_back({3, bx,  0.12f, bw, bh, "3. Add Reusable Waste (Monitors / CPUs)",   0.18f, 0.50f, 0.25f});
    buttons.push_back({4, bx, -0.03f, bw, bh, "4. Create Collection Queue (FIFO)",         0.35f, 0.25f, 0.55f});
    buttons.push_back({5, bx, -0.18f, bw, bh, "5. Process & Segregate Waste",              0.75f, 0.45f, 0.15f});
    buttons.push_back({6, bx, -0.33f, bw, bh, "6. Display Sorted Waste Summary",           0.25f, 0.40f, 0.45f});
    buttons.push_back({7, bx, -0.48f, bw, bh, "7. Clear & Reset System State",             0.35f, 0.35f, 0.38f});
}

void handleButtonClick(int id) {
    auto& br = Bridge::get();
    char buf[128];

    switch (id) {
        case 1:
            br.setCategory(CAT_RECYCLABLE);
            br.triggerDeposit(4.5f);
            snprintf(buf, sizeof(buf), "[PL CLI SYNC] Request Added: Recyclable (4.5kg) -> Dispatched Bay 1");
            addLog(buf);
            break;
        case 2:
            br.setCategory(CAT_HAZARDOUS);
            br.triggerDeposit(12.0f);
            snprintf(buf, sizeof(buf), "[PL CLI SYNC] Request Added: Hazardous (12.0kg) -> Dispatched Bay 2");
            addLog(buf);
            break;
        case 3:
            br.setCategory(CAT_REUSABLE);
            br.triggerDeposit(8.0f);
            snprintf(buf, sizeof(buf), "[PL CLI SYNC] Request Added: Reusable (8.0kg) -> Dispatched Bay 3");
            addLog(buf);
            break;
        case 4:
            br.queueLength++;
            br.saveIPC();
            snprintf(buf, sizeof(buf), "[PL CO3 QUEUE] Requests converted to FIFO Queue (Count: %d)", br.queueLength);
            addLog(buf);
            break;
        case 5:
            br.completeDeposit();
            snprintf(buf, sizeof(buf), "[PSOOP CO2] Waste Segregated! User Balance: %d Cr", br.userCredits);
            addLog(buf);
            break;
        case 6:
            snprintf(buf, sizeof(buf), "[STACKS] Recycled: %.1fkg | Hazard: %.1fkg | Reusable: %.1fkg",
                     br.totalRecycledKg, br.totalHazardKg, br.totalReusableKg);
            addLog(buf);
            break;
        case 7:
            br.txState = 0;
            br.saveIPC();
            addLog("[SYSTEM] State reset to Standby. Ready for next transaction.");
            break;
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double mx, my;
        glfwGetCursorPos(window, &mx, &my);
        int w, h;
        glfwGetWindowSize(window, &w, &h);

        // Convert window pixel coordinates to OpenGL Ortho [-1.0, 1.0]
        float ox = (float)mx / (float)w * 2.0f - 1.0f;
        float oy = 1.0f - (float)my / (float)h * 2.0f;

        for (const auto& btn : buttons) {
            if (ox >= btn.x && ox <= btn.x + btn.w &&
                oy >= btn.y && oy <= btn.y + btn.h) {
                handleButtonClick(btn.id);
                break;
            }
        }
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    auto& br = Bridge::get();
    br.syncIPC();

    // 1. Top Header Banner
    drawSolidRect(-1.0f, 0.76f, 2.0f, 0.24f, 0.10f, 0.12f, 0.16f);
    drawRectBorder(-1.0f, 0.76f, 2.0f, 0.24f, 0.25f, 0.30f, 0.40f, 2.0f);
    drawText(-0.92f, 0.89f, "ECOSORT 386 - MASTER SYSTEM CONTROL INTERFACE", 0.95f, 0.95f, 0.95f);
    drawText(-0.92f, 0.81f, "Integrated 2D Command GUI | Synchronized with PL & PSOOP, CGL & COA", 0.65f, 0.75f, 0.85f);

    // Header Right-Hand Status Badges
    char creditBuf[64];
    snprintf(creditBuf, sizeof(creditBuf), "User Credits: %d Cr", br.userCredits);
    drawSolidRect(0.48f, 0.80f, 0.44f, 0.12f, 0.08f, 0.28f, 0.15f);
    drawRectBorder(0.48f, 0.80f, 0.44f, 0.12f, 0.20f, 0.70f, 0.35f, 1.5f);
    drawText(0.52f, 0.84f, creditBuf, 0.90f, 1.0f, 0.90f);

    // 2. Left Column Title (PL & PSOOP Actions)
    drawText(-0.92f, 0.66f, "PL & PSOOP COMMAND BUTTONS (Click to Execute):", 0.85f, 0.85f, 0.90f);

    // Render Buttons (Clean, solid rectangular aesthetic - no glow, no hover complexity)
    for (const auto& btn : buttons) {
        // Solid fill
        drawSolidRect(btn.x, btn.y, btn.w, btn.h, btn.r, btn.g, btn.b);
        // Border outline
        drawRectBorder(btn.x, btn.y, btn.w, btn.h, 0.9f, 0.9f, 0.95f, 1.2f);
        // Button Label
        drawText(btn.x + 0.04f, btn.y + btn.h * 0.36f, btn.label, 1.0f, 1.0f, 1.0f);
    }

    // 3. Right Column: System Telemetry & Live Monitor Board
    float rx = 0.08f;
    float rw = 0.84f;
    drawText(rx, 0.66f, "LIVE SYSTEM STATE & IPC TELEMETRY:", 0.85f, 0.85f, 0.90f);

    // Telemetry Card Box
    drawSolidRect(rx, 0.12f, rw, 0.49f, 0.13f, 0.15f, 0.20f);
    drawRectBorder(rx, 0.12f, rw, 0.49f, 0.30f, 0.35f, 0.45f, 1.5f);

    // State Fields
    char tbuf[128];
    const char* stateStr = (br.txState == 1 ? "DISPATCHED / MOVING" : (br.txState == 2 ? "COMPLETED / STORED" : "IDLE / STANDBY"));
    float sr = (br.txState == 1 ? 1.0f : (br.txState == 2 ? 0.3f : 0.7f));
    float sg = (br.txState == 1 ? 0.8f : (br.txState == 2 ? 0.9f : 0.7f));
    float sb = (br.txState == 1 ? 0.2f : (br.txState == 2 ? 0.4f : 0.7f));

    snprintf(tbuf, sizeof(tbuf), "System State      : %s", stateStr);
    drawText(rx + 0.04f, 0.53f, tbuf, sr, sg, sb);

    snprintf(tbuf, sizeof(tbuf), "Active Category   : %s", br.getCategoryName());
    drawText(rx + 0.04f, 0.46f, tbuf, 0.85f, 0.85f, 0.85f);

    snprintf(tbuf, sizeof(tbuf), "Designated Bay    : Intake Bay %d", br.getTargetBay());
    drawText(rx + 0.04f, 0.39f, tbuf, 0.85f, 0.85f, 0.85f);

    snprintf(tbuf, sizeof(tbuf), "FIFO Queue Count  : %d item(s) pending", br.queueLength);
    drawText(rx + 0.04f, 0.32f, tbuf, 0.95f, 0.75f, 0.30f);

    snprintf(tbuf, sizeof(tbuf), "Last Item Weight  : %.1f kg (COA Input)", br.lastItemWeight);
    drawText(rx + 0.04f, 0.25f, tbuf, 0.95f, 0.90f, 0.35f);

    snprintf(tbuf, sizeof(tbuf), "Total System Mass : %.1f kg", br.totalWeight);
    drawText(rx + 0.04f, 0.18f, tbuf, 0.70f, 0.85f, 0.95f);

    // 4. Right Column Bottom: Real-Time Event Activity Log Box
    drawText(rx, 0.04f, "REAL-TIME CROSS-SUBJECT ACTIVITY LOG:", 0.85f, 0.85f, 0.90f);

    drawSolidRect(rx, -0.56f, rw, 0.54f, 0.08f, 0.09f, 0.12f);
    drawRectBorder(rx, -0.56f, rw, 0.54f, 0.25f, 0.28f, 0.35f, 1.2f);

    float logY = 0.47f - 0.56f;
    if (eventLogs.empty()) {
        drawText(rx + 0.04f, logY, "> System ready. Click any button or enter data in PL CLI.", 0.55f, 0.55f, 0.55f);
    } else {
        for (int i = (int)eventLogs.size() - 1; i >= 0; i--) {
            drawText(rx + 0.03f, logY, ("> " + eventLogs[i]).c_str(), 0.75f, 0.85f, 0.95f);
            logY -= 0.07f;
            if (logY < -0.52f) break;
        }
    }

    // 5. Bottom Instructions Bar
    drawText(-0.92f, -0.66f, "Tip: Both this GUI window and the PL & PSOOP console stay open and fully active!", 0.60f, 0.60f, 0.65f);

    glFlush();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height > 0 ? height : 1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main() {
    if (!glfwInit()) {
        printf("[ERROR] Failed to initialize GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    GLFWwindow* window = glfwCreateWindow(WIN_W, WIN_H, "EcoSort 386 - System Interface & Control Panel", NULL, NULL);
    if (!window) {
        printf("[ERROR] Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    glClearColor(0.12f, 0.14f, 0.18f, 1.0f); // Clean, dark professional background
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    initFont();
    initButtons();
    addLog("EcoSort 386 Interface initialized and connected to IPC bridge.");

    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
        Sleep(16); // ~60 FPS
    }

    if (fontBase) glDeleteLists(fontBase, 128);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

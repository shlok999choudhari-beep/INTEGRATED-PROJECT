#include <windows.h>
#include <GLFW/glfw3.h>
#include <GL/glu.h>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <vector>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "../../ecosort_bridge.h"

using namespace EcoSortCore;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

const unsigned int SCR_WIDTH = 900, SCR_HEIGHT = 650;
const char* SCR_TITLE = "EcoSort 386 - CGL CO2: Bresenham & DDA Algorithms";
int stage = 5;
bool captureAll = false;
bool isPaused = false;
GLuint fontBase = 0;

// ----------------------------------------------------------------------------
// Font & Text Utilities
// ----------------------------------------------------------------------------
void initFont() {
    HDC hdc = wglGetCurrentDC();
    HFONT font = CreateFontA(14, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE, "Arial");
    SelectObject(hdc, font);
    fontBase = glGenLists(128);
    wglUseFontBitmaps(hdc, 0, 128, fontBase);
}

void drawText(float x, float y, const char* s, float r = 0.1f, float g = 0.1f, float b = 0.1f) {
    glColor3f(r, g, b);
    glRasterPos2f(x, y);
    glPushAttrib(GL_LIST_BIT);
    glListBase(fontBase);
    glCallLists((GLsizei)strlen(s), GL_UNSIGNED_BYTE, s);
    glPopAttrib();
}

// ============================================================================
// CO2 Core Algorithm 1: Bresenham's Integer Line Algorithm
// ============================================================================
void drawLineBresenham(int x1, int y1, int x2, int y2, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_POINTS);
    int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
    int err = dx + dy;
    while (true) {
        glVertex2i(x1, y1);
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x1 += sx; }
        if (e2 <= dx) { err += dx; y1 += sy; }
    }
    glEnd();
}

// ============================================================================
// CO2 Core Algorithm 2: Bresenham's Midpoint Circle Algorithm (8-Way Symmetry)
// ============================================================================
void drawCircleBresenham(int xc, int yc, int radius, float r, float g, float b, bool colorOctants = false) {
    int x = 0, y = radius, p = 1 - radius;
    glBegin(GL_POINTS);
    auto plot8 = [&](int x, int y) {
        if (!colorOctants) {
            glColor3f(r, g, b);
            glVertex2i(xc + x, yc + y); glVertex2i(xc - x, yc + y);
            glVertex2i(xc + x, yc - y); glVertex2i(xc - x, yc - y);
            glVertex2i(xc + y, yc + x); glVertex2i(xc - y, yc + x);
            glVertex2i(xc + y, yc - x); glVertex2i(xc - y, yc - x);
        } else {
            glColor3f(1, 0.1f, 0.1f); glVertex2i(xc + x, yc + y);
            glColor3f(0.1f, 0.9f, 0.2f); glVertex2i(xc + y, yc + x);
            glColor3f(0.2f, 0.5f, 1);    glVertex2i(xc - y, yc + x);
            glColor3f(1, 0.85f, 0);      glVertex2i(xc - x, yc + y);
            glColor3f(1, 0.2f, 0.8f);    glVertex2i(xc - x, yc - y);
            glColor3f(0, 0.8f, 0.8f);    glVertex2i(xc - y, yc - x);
            glColor3f(1, 0.5f, 0);       glVertex2i(xc + y, yc - x);
            glColor3f(0.6f, 0.2f, 0.8f); glVertex2i(xc + x, yc - y);
        }
    };
    plot8(x, y);
    while (x < y) {
        x++;
        if (p < 0) p += 2 * x + 1;
        else { y--; p += 2 * (x - y) + 1; }
        plot8(x, y);
    }
    glEnd();
}

// ============================================================================
// CO2 Core Algorithm 3: Digital Differential Analyzer (DDA) Line Algorithm
// ============================================================================
void drawLineDDA(int x1, int y1, int x2, int y2, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_POINTS);
    int dx = x2 - x1, dy = y2 - y1;
    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
    if (steps == 0) { glVertex2i(x1, y1); glEnd(); return; }
    float xInc = (float)dx / steps, yInc = (float)dy / steps;
    float x = (float)x1, y = (float)y1;
    for (int i = 0; i <= steps; i++) {
        glVertex2i((int)roundf(x), (int)roundf(y));
        x += xInc; y += yInc;
    }
    glEnd();
}

// ============================================================================
// CO2 Core Algorithm 4: Digital Differential Analyzer (DDA) Circle Algorithm
// ============================================================================
void drawCircleDDA(int xc, int yc, int radius, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_POINTS);
    float dtheta = 1.0f / (float)radius;
    float x = (float)radius, y = 0.0f;
    int totalSteps = (int)(2.0f * (float)M_PI * radius);
    for (int i = 0; i <= totalSteps; i++) {
        glVertex2i(xc + (int)roundf(x), yc + (int)roundf(y));
        x -= y * dtheta;
        y += x * dtheta;
    }
    glEnd();
}

// Helper: Solid or wireframe box using Bresenham lines
void drawBoxBresenham(int x1, int y1, int x2, int y2, float r, float g, float b) {
    drawLineBresenham(x1, y1, x2, y1, r, g, b);
    drawLineBresenham(x2, y1, x2, y2, r, g, b);
    drawLineBresenham(x2, y2, x1, y2, r, g, b);
    drawLineBresenham(x1, y2, x1, y1, r, g, b);
}

bool saveScreenshot(GLFWwindow* window, const char* filename) {
    int w, h; glfwGetFramebufferSize(window, &w, &h);
    if (w <= 0) w = SCR_WIDTH; if (h <= 0) h = SCR_HEIGHT;
    std::vector<unsigned char> pixels(w * h * 3);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
    stbi_flip_vertically_on_write(1);
    return stbi_write_png(filename, w, h, 3, pixels.data(), w * 3);
}

// ----------------------------------------------------------------------------
// Academic Verification Stages 1 - 4
// ----------------------------------------------------------------------------
void stage1() {
    glClearColor(0.96f, 0.97f, 0.98f, 1); glClear(GL_COLOR_BUFFER_BIT);
    drawText(50, 615, "EcoSort 386 - CGL CO2 | Stage 1: Bresenham's Integer Line Algorithm", 0.1f, 0.2f, 0.35f);
    drawText(50, 590, "Testing Octants & Critical Slopes (|m| < 1, |m| > 1, m = 0, m = 1, m = inf)", 0.3f, 0.35f, 0.4f);
    int cx = 450, cy = 320;
    drawCircleBresenham(cx, cy, 180, 0.85f, 0.88f, 0.90f);
    for (int deg = 0; deg < 360; deg += 30) {
        float a = deg * (float)M_PI / 180.0f;
        int x2 = cx + (int)(180.0f * cosf(a)), y2 = cy + (int)(180.0f * sinf(a));
        float r = 0.2f + 0.7f * (deg / 360.0f);
        float g = 0.8f - 0.5f * (deg / 360.0f);
        drawLineBresenham(cx, cy, x2, y2, r, g, 0.3f);
        char lbl[16]; snprintf(lbl, sizeof(lbl), "%d", deg);
        drawText(x2 + (int)(15.0f * cosf(a)) - 8, y2 + (int)(15.0f * sinf(a)) - 5, lbl, 0.3f, 0.3f, 0.3f);
    }
    drawBoxBresenham(cx - 200, cy - 200, cx + 200, cy + 200, 0.75f, 0.78f, 0.82f);
    drawText(50, 45, "[Decision Parameter]: Pk+1 = Pk + 2dy - 2dx(yk+1 - yk)", 0.2f, 0.4f, 0.6f);
    drawText(50, 25, "[Keys 1-5]: Switch Stages | [S]: Save Screenshot | [ESC]: Quit", 0.5f, 0.5f, 0.5f);
}

void stage2() {
    glClearColor(0.96f, 0.97f, 0.98f, 1); glClear(GL_COLOR_BUFFER_BIT);
    drawText(50, 615, "EcoSort 386 - CGL CO2 | Stage 2: Bresenham's Midpoint Circle Algorithm", 0.1f, 0.2f, 0.35f);
    drawText(50, 590, "Demonstrating 8-Way Octant Symmetry & Second-Order Finite Differences", 0.3f, 0.35f, 0.4f);
    int cx = 450, cy = 320, r = 160;
    drawCircleBresenham(cx, cy, r, 0, 0, 0, true);
    for (int rad = 40; rad <= 120; rad += 40) drawCircleBresenham(cx, cy, rad, 0.75f, 0.8f, 0.85f);
    drawLineBresenham(cx - 200, cy, cx + 200, cy, 0.6f, 0.65f, 0.7f);
    drawLineBresenham(cx, cy - 200, cx, cy + 200, 0.6f, 0.65f, 0.7f);
    drawLineBresenham(cx - 150, cy - 150, cx + 150, cy + 150, 0.65f, 0.7f, 0.75f);
    drawLineBresenham(cx - 150, cy + 150, cx + 150, cy - 150, 0.65f, 0.7f, 0.75f);
    drawText(cx + 80, cy + 130, "Octant 1 (x, y)", 1, 0.1f, 0.1f);
    drawText(cx + 130, cy + 70, "Octant 2 (y, x)", 0.1f, 0.8f, 0.2f);
    drawText(cx + 125, cy - 80, "Octant 3 (y, -x)", 0.2f, 0.5f, 1);
    drawText(cx + 70, cy - 140, "Octant 4 (x, -y)", 0.9f, 0.75f, 0);
    drawText(cx - 150, cy - 140, "Octant 5 (-x, -y)", 1, 0.2f, 0.8f);
    drawText(cx - 210, cy - 80, "Octant 6 (-y, -x)", 0, 0.7f, 0.7f);
    drawText(cx - 210, cy + 70, "Octant 7 (-y, x)", 1, 0.5f, 0);
    drawText(cx - 150, cy + 130, "Octant 8 (-x, y)", 0.6f, 0.2f, 0.8f);
    drawText(50, 45, "[Decision Variable]: Pk = 1 - r | If Pk < 0: Pk+1 = Pk + 2x + 1 | Else: Pk+1 = Pk + 2(x - y) + 1", 0.1f, 0.45f, 0.25f);
    drawText(50, 25, "[Keys 1-5]: Switch Stages | [S]: Save Screenshot | [ESC]: Quit", 0.5f, 0.5f, 0.5f);
}

void stage3() {
    glClearColor(0.96f, 0.97f, 0.98f, 1); glClear(GL_COLOR_BUFFER_BIT);
    drawText(50, 615, "EcoSort 386 - CGL CO2 | Stage 3: Digital Differential Analyzer (DDA)", 0.1f, 0.2f, 0.35f);
    drawText(50, 590, "Incremental Differential Equations & Stepping Geometry (Lines & Circles)", 0.3f, 0.35f, 0.4f);
    for (int i = 0; i < 8; i++) {
        int x1 = 120 + i * 90, y1 = 450, x2 = 180 + i * 90, y2 = 250;
        drawLineDDA(x1, y1, x2, y2, 0.2f + i * 0.08f, 0.4f, 0.9f - i * 0.08f);
    }
    drawCircleDDA(250, 300, 75, 0.9f, 0.35f, 0.1f);
    drawCircleDDA(450, 300, 100, 0.1f, 0.7f, 0.3f);
    drawCircleDDA(650, 300, 60, 0.8f, 0.1f, 0.75f);
    drawText(50, 45, "[Differential Formula]: xk+1 = xk + (dx/steps) | yk+1 = yk + (dy/steps) [Requires roundf()]", 0.6f, 0.25f, 0.1f);
    drawText(50, 25, "[Keys 1-5]: Switch Stages | [S]: Save Screenshot | [ESC]: Quit", 0.5f, 0.5f, 0.5f);
}

void stage4() {
    glClearColor(0.95f, 0.96f, 0.98f, 1); glClear(GL_COLOR_BUFFER_BIT);
    drawLineBresenham(450, 60, 450, 560, 0.7f, 0.75f, 0.8f);
    for (int deg = -60; deg <= 60; deg += 20) {
        float a = deg * (float)M_PI / 180.0f;
        drawLineBresenham(230, 420, 230 + (int)(110 * cosf(a)), 420 + (int)(110 * sinf(a)), 0, 0.5f, 0.85f);
        drawLineDDA(670, 420, 670 + (int)(110 * cosf(a)), 420 + (int)(110 * sinf(a)), 0.85f, 0.35f, 0);
    }
    drawCircleBresenham(230, 200, 85, 0, 0.5f, 0.85f);
    drawCircleBresenham(230, 200, 50, 0, 0.3f, 0.65f);
    drawCircleDDA(670, 200, 85, 0.85f, 0.35f, 0);
    drawCircleDDA(670, 200, 50, 0.65f, 0.25f, 0);
    drawText(50, 615, "EcoSort 386 - CGL CO2 | Stage 4: Algorithm Comparative Analysis", 0.1f, 0.2f, 0.4f);
    drawText(120, 560, "BRESENHAM ALGORITHMS (Primary)", 0, 0.4f, 0.75f);
    drawText(100, 300, "* Pure integer arithmetic (fastest)", 0.2f, 0.2f, 0.2f);
    drawText(100, 280, "* Zero roundoff error or drift", 0.2f, 0.2f, 0.2f);
    drawText(100, 260, "* 8-Way symmetry circle stepping", 0.2f, 0.2f, 0.2f);
    drawText(570, 560, "DDA ALGORITHMS (Further Practice)", 0.8f, 0.3f, 0);
    drawText(540, 300, "* Uses floating point division & roundf()", 0.2f, 0.2f, 0.2f);
    drawText(540, 280, "* Incremental differential recurrence", 0.2f, 0.2f, 0.2f);
    drawText(540, 260, "* Accumulates slight roundoff error", 0.2f, 0.2f, 0.2f);
    drawText(50, 25, "[Keys 1-5]: Switch Stages | [S]: Save Screenshot | [ESC]: Quit", 0.5f, 0.5f, 0.5f);
}

// ============================================================================
// STAGE 5: FULLY WORKING DYNAMIC OPTICAL E-WASTE SORTING ASSEMBLY LINE
// ============================================================================

struct ConveyorItem {
    float x;
    float y;
    int category; // 1 = Reusable, 2 = Recyclable, 3 = Hazardous
    float weight;
    char name[32];
    bool scanned;
    bool diverted;
    bool inBin;
    float targetChuteX;
    float targetBinY;
};

std::vector<ConveyorItem> activeItems;
float simTime = 0.0f;
float beltOffset = 0.0f;
float rollerAngle = 0.0f;
float autoSpawnTimer = 0.0f;
uint32_t lastSyncedSeq = 0;

// Dynamic Bin Telemetry
int countReusable = 5;
float weightReusable = 19.5f;
int countRecyclable = 8;
float weightRecyclable = 38.0f;
int countHazardous = 3;
float weightHazardous = 7.5f;

char lastScannedName[32] = "Laptop Pro";
float lastScannedWeight = 5.2f;
int lastScannedCat = 2; // Recyclable
float scanFlashTimer = 0.0f;

void spawnConveyorItem(int cat, float wt, const char* name) {
    ConveyorItem item;
    item.x = 70.0f;
    item.y = 352.0f; // Directly on the belt surface
    item.category = cat;
    item.weight = wt > 0.1f ? wt : 4.0f;
    strncpy(item.name, name, sizeof(item.name) - 1);
    item.name[sizeof(item.name) - 1] = '\0';
    item.scanned = false;
    item.diverted = false;
    item.inBin = false;

    // Target diversion chutes along the conveyor
    if (cat == CAT_REUSABLE) {
        item.targetChuteX = 385.0f; // Chute 1 -> Reusable Bin
        item.targetBinY = 120.0f + (countReusable % 4) * 12.0f;
    } else if (cat == CAT_RECYCLABLE) {
        item.targetChuteX = 565.0f; // Chute 2 -> Recyclable Bin
        item.targetBinY = 120.0f + (countRecyclable % 4) * 12.0f;
    } else {
        item.targetChuteX = 745.0f; // Chute 3 -> Hazardous Bin
        item.targetBinY = 120.0f + (countHazardous % 4) * 12.0f;
    }

    activeItems.push_back(item);
}

void updateSimulation(float dt) {
    if (isPaused) return;

    simTime += dt;
    beltOffset += 70.0f * dt;
    if (beltOffset >= 40.0f) beltOffset -= 40.0f;
    rollerAngle += 4.5f * dt;
    if (rollerAngle > 2.0f * (float)M_PI) rollerAngle -= 2.0f * (float)M_PI;

    if (scanFlashTimer > 0.0f) scanFlashTimer -= dt;

    // 1. Check IPC Bridge for live incoming requests from PL/PSOOP or Master Interface
    Bridge& br = Bridge::get();
    if (br.syncIPC()) {
        if (br.txState == 1 && br.lastItemWeight > 0.05f) {
            const char* defaultName = (br.activeCategory == CAT_HAZARDOUS ? "Li-Ion Battery" : (br.activeCategory == CAT_REUSABLE ? "LED Display" : "Motherboard PCB"));
            spawnConveyorItem(br.activeCategory, br.lastItemWeight, defaultName);
        }
    }

    // 2. Auto-feed demo parcels periodically if conveyor has room
    autoSpawnTimer += dt;
    if (autoSpawnTimer >= 3.8f && activeItems.size() < 4) {
        autoSpawnTimer = 0.0f;
        static int demoCycle = 0;
        demoCycle = (demoCycle + 1) % 5;
        if (demoCycle == 0) spawnConveyorItem(CAT_RECYCLABLE, 4.8f, "PCB Circuit");
        else if (demoCycle == 1) spawnConveyorItem(CAT_REUSABLE, 3.2f, "CRT Monitor");
        else if (demoCycle == 2) spawnConveyorItem(CAT_HAZARDOUS, 1.8f, "Lead Acid Cell");
        else if (demoCycle == 3) spawnConveyorItem(CAT_RECYCLABLE, 9.5f, "Server Rack");
        else spawnConveyorItem(CAT_REUSABLE, 2.4f, "SMPS Adapter");
    }

    // 3. Update active conveyor packages
    float beltSpeed = 85.0f;
    float dropSpeed = 110.0f;

    for (size_t i = 0; i < activeItems.size(); i++) {
        ConveyorItem& item = activeItems[i];

        if (!item.diverted) {
            item.x += beltSpeed * dt;

            // Trigger Optical Scanner Inspection when package reaches sx = 220
            if (!item.scanned && item.x >= 200.0f && item.x <= 245.0f) {
                item.scanned = true;
                scanFlashTimer = 0.65f;
                strncpy(lastScannedName, item.name, sizeof(lastScannedName));
                lastScannedWeight = item.weight;
                lastScannedCat = item.category;
            }

            // Divert down into the respective bin chute
            if (item.x >= item.targetChuteX) {
                item.diverted = true;
                item.x = item.targetChuteX; // Lock horizontal position
            }
        } else if (!item.inBin) {
            // Drop down into bin
            item.y -= dropSpeed * dt;
            if (item.y <= item.targetBinY) {
                item.y = item.targetBinY;
                item.inBin = true;

                // Increment live bin metrics
                if (item.category == CAT_REUSABLE) {
                    countReusable++;
                    weightReusable += item.weight;
                } else if (item.category == CAT_RECYCLABLE) {
                    countRecyclable++;
                    weightRecyclable += item.weight;
                } else {
                    countHazardous++;
                    weightHazardous += item.weight;
                }
            }
        }
    }

    // Clean up items that have rested in the bin for too long
    if (activeItems.size() > 7) {
        for (auto it = activeItems.begin(); it != activeItems.end(); ++it) {
            if (it->inBin) { activeItems.erase(it); break; }
        }
    }
}

void stage5() {
    glClearColor(0.93f, 0.95f, 0.97f, 1); glClear(GL_COLOR_BUFFER_BIT);

    // ------------------------------------------------------------------------
    // 1. Conveyor Frame & Rolling Rollers (Bresenham Lines & Circles)
    // ------------------------------------------------------------------------
    int beltLeft = 60, beltRight = 840, beltTop = 350, beltBottom = 290;

    // Outer guide rails (Bresenham lines)
    drawLineBresenham(beltLeft, beltTop, beltRight, beltTop, 0.18f, 0.22f, 0.28f);
    drawLineBresenham(beltLeft, beltBottom, beltRight, beltBottom, 0.18f, 0.22f, 0.28f);
    drawLineBresenham(beltLeft, beltBottom - 4, beltRight, beltBottom - 4, 0.35f, 0.40f, 0.45f);

    // Moving belt tread notches (Bresenham lines)
    for (int x = beltLeft + (int)beltOffset; x <= beltRight; x += 35) {
        drawLineBresenham(x, beltBottom + 2, x, beltTop - 2, 0.65f, 0.70f, 0.75f);
    }

    // Rotating rollers with spokes (Bresenham & DDA Circles + Bresenham radial lines)
    for (int x = 100; x <= 800; x += 115) {
        drawCircleBresenham(x, 320, 24, 0.15f, 0.55f, 0.40f);
        drawCircleDDA(x, 320, 11, 0.70f, 0.75f, 0.80f);
        // Spokes drawn with Bresenham lines at current rotational angle
        for (int s = 0; s < 4; s++) {
            float ang = rollerAngle + s * ((float)M_PI / 2.0f);
            int spX = x + (int)(22.0f * cosf(ang));
            int spY = 320 + (int)(22.0f * sinf(ang));
            drawLineBresenham(x, 320, spX, spY, 0.25f, 0.30f, 0.35f);
        }
    }

    // ------------------------------------------------------------------------
    // 2. Optical Sensor Scanner Station (Intake at sx = 220, sy = 490)
    // ------------------------------------------------------------------------
    int sx = 220, sy = 490;

    // Overhead gantry structural truss (Bresenham lines)
    drawLineBresenham(sx - 70, 565, sx + 70, 565, 0.10f, 0.20f, 0.35f);
    drawLineBresenham(sx - 70, 565, sx, sy + 38, 0.25f, 0.35f, 0.45f);
    drawLineBresenham(sx + 70, 565, sx, sy + 38, 0.25f, 0.35f, 0.45f);
    drawLineBresenham(sx, 565, sx, sy + 38, 0.15f, 0.25f, 0.35f);

    // Scanner emitter & focal rings (Bresenham & DDA Circles)
    drawCircleBresenham(sx, sy, 36, 0.10f, 0.45f, 0.85f);
    drawCircleDDA(sx, sy, 50, 0.90f, 0.65f, 0.10f);

    // Dynamic scanning laser sweep
    float sweepAngle = sinf(simTime * 5.0f) * 28.0f;
    for (int deg = -36; deg <= 36; deg += 9) {
        float a = (deg + sweepAngle - 90.0f) * (float)M_PI / 180.0f;
        if (scanFlashTimer > 0.0f) {
            // High-intensity optical pulse during detection
            drawLineBresenham(sx, sy, sx + (int)(150 * cosf(a)), sy + (int)(150 * sinf(a)), 0.0f, 0.95f, 0.90f);
        } else {
            // Idle ambient sweeping beam
            drawLineBresenham(sx, sy, sx + (int)(140 * cosf(a)), sy + (int)(140 * sinf(a)), 0.95f, 0.30f, 0.20f);
        }
    }
    drawText(sx - 55, sy + 58, "OPTICAL SCANNER", 0.1f, 0.2f, 0.4f);

    // ------------------------------------------------------------------------
    // 3. Three Automated Sorting Bins (Reusable, Recyclable, Hazardous)
    // ------------------------------------------------------------------------
    auto drawSortBin = [&](int bx, int by, int bw, int bh, float r, float g, float b,
                           const char* code, const char* label, int itemCount, float totalKg, bool chuteActive) {
        // Bin walls and reinforced rim (Bresenham lines)
        drawLineBresenham(bx, by, bx + bw, by, r, g, b);
        drawLineBresenham(bx, by, bx, by + bh, r, g, b);
        drawLineBresenham(bx + bw, by, bx + bw, by + bh, r, g, b);
        drawLineBresenham(bx - 10, by + bh, bx + bw + 10, by + bh, r * 0.85f, g * 0.85f, b * 0.85f);
        drawLineBresenham(bx - 10, by + bh + 4, bx + bw + 10, by + bh + 4, r * 0.70f, g * 0.70f, b * 0.70f);

        // Circular Emblem on Bin Face (Bresenham & DDA Circles)
        drawCircleBresenham(bx + bw / 2, by + bh / 2 - 10, 22, r, g, b);
        drawCircleDDA(bx + bw / 2, by + bh / 2 - 10, 28, r * 0.75f, g * 0.75f, b * 0.75f);
        drawText(bx + bw / 2 - 5, by + bh / 2 - 16, code, r, g, b);

        // Fill level indicator (Bresenham lines inside bin)
        int fillMax = bh - 35;
        int fillCur = 10 + itemCount * 7;
        int fillH = (fillCur < fillMax ? fillCur : fillMax);
        for (int y = by + 6; y <= by + fillH; y += 6) {
            drawLineBresenham(bx + 8, y, bx + bw - 8, y, r * 0.45f, g * 0.45f, b * 0.45f);
        }

        // Chute diverter gate on conveyor (Bresenham line)
        int cx = bx + bw / 2;
        if (chuteActive) {
            // Diverter gate extends downwards (open)
            drawLineBresenham(cx - 15, beltTop, cx + 15, beltTop - 25, 0.0f, 0.85f, 0.45f);
            drawLineBresenham(cx, beltTop - 25, cx, by + bh, 0.2f, 0.7f, 0.3f);
        } else {
            // Diverter gate closed flat
            drawLineBresenham(cx - 15, beltTop + 2, cx + 15, beltTop + 2, 0.5f, 0.5f, 0.5f);
        }

        // Labels & Live Counters
        drawText(bx, by - 18, label, 0.15f, 0.20f, 0.30f);
        char stats[64];
        snprintf(stats, sizeof(stats), "Count: %d | %.1f kg", itemCount, totalKg);
        drawText(bx, by - 34, stats, r * 0.8f, g * 0.8f, b * 0.8f);
    };

    // Determine which chute gates are active based on approaching parcels
    bool chute1Active = false, chute2Active = false, chute3Active = false;
    for (const auto& it : activeItems) {
        if (it.diverted && !it.inBin) {
            if (it.category == CAT_REUSABLE) chute1Active = true;
            else if (it.category == CAT_RECYCLABLE) chute2Active = true;
            else chute3Active = true;
        }
    }

    drawSortBin(310, 80, 150, 150, 0.00f, 0.50f, 0.90f, "U", "Bay 1: Reusable (PSOOP)", countReusable, weightReusable, chute1Active);
    drawSortBin(490, 80, 150, 150, 0.00f, 0.70f, 0.30f, "R", "Bay 2: Recyclable (PCB)", countRecyclable, weightRecyclable, chute2Active);
    drawSortBin(670, 80, 150, 150, 0.90f, 0.25f, 0.20f, "H", "Bay 3: Hazardous (Toxic)", countHazardous, weightHazardous, chute3Active);

    // ------------------------------------------------------------------------
    // 4. Draw Active Moving E-Waste Packages on Belt / Dropping into Bins
    // ------------------------------------------------------------------------
    for (const auto& item : activeItems) {
        float r = 0, g = 0, b = 0;
        if (item.category == CAT_REUSABLE) { r = 0.0f; g = 0.55f; b = 0.95f; }
        else if (item.category == CAT_RECYCLABLE) { r = 0.0f; g = 0.75f; b = 0.35f; }
        else { r = 0.95f; g = 0.25f; b = 0.20f; }

        int px = (int)item.x - 22, py = (int)item.y;
        int pw = 44, ph = 26;

        // Package chassis (Bresenham box)
        drawBoxBresenham(px, py, px + pw, py + ph, r, g, b);
        drawBoxBresenham(px + 3, py + 3, px + pw - 3, py + ph - 3, r * 0.7f, g * 0.7f, b * 0.7f);

        // Microchip center circle (Bresenham circle)
        drawCircleBresenham(px + pw / 2, py + ph / 2, 6, 0.85f, 0.75f, 0.15f);

        // Package short label
        char shortLbl[16];
        snprintf(shortLbl, sizeof(shortLbl), "%.1fkg", item.weight);
        drawText(px + 4, py + 8, shortLbl, 0.1f, 0.1f, 0.15f);
    }

    // ------------------------------------------------------------------------
    // 5. Header, Live Sensor Telemetry HUD & Control Keys
    // ------------------------------------------------------------------------
    drawText(50, 620, "ECOSORT 386 - DYNAMIC OPTICAL E-WASTE SORTING ASSEMBLY LINE", 0.08f, 0.18f, 0.35f);
    drawText(50, 598, "CGL CO2 Capstone: Live Conveyor, Sweeping Scanner & Auto-Segregation via Bresenham & DDA", 0.35f, 0.40f, 0.45f);

    // Optical Sensor HUD Box (Bresenham box)
    drawBoxBresenham(310, 570, 840, 635, 0.75f, 0.80f, 0.85f);
    char scanHud[128];
    const char* catName = (lastScannedCat == CAT_REUSABLE ? "REUSABLE" : (lastScannedCat == CAT_RECYCLABLE ? "RECYCLABLE" : "HAZARDOUS"));
    snprintf(scanHud, sizeof(scanHud), "OPTICAL SENSOR TELEMETRY: %s | %s (%.1f kg)", (scanFlashTimer > 0.0f ? "[SCANNING...]" : "[LOCKED]"), lastScannedName, lastScannedWeight);
    drawText(320, 612, scanHud, (scanFlashTimer > 0.0f ? 0.0f : 0.1f), (scanFlashTimer > 0.0f ? 0.6f : 0.4f), (scanFlashTimer > 0.0f ? 0.9f : 0.7f));
    char routeHud[128];
    snprintf(routeHud, sizeof(routeHud), "Target Routing: Bay %d (%s) | Conveyor Speed: 85 px/s | IPC: SYNCED", (lastScannedCat == CAT_REUSABLE ? 1 : (lastScannedCat == CAT_RECYCLABLE ? 2 : 3)), catName);
    drawText(320, 584, routeHud, 0.25f, 0.30f, 0.35f);

    // Footer Control Keybindings
    drawText(50, 25, "[Keys 1-5]: Switch Stages | [SPACE]: Pause/Resume | [T]: Inject Parcel | [S]: Save Screenshot | [ESC]: Quit", 0.4f, 0.45f, 0.5f);
}

void display() {
    if (stage == 1) stage1();
    else if (stage == 2) stage2();
    else if (stage == 3) stage3();
    else if (stage == 4) stage4();
    else stage5();
    glFlush();
}

void framebuffer_size_callback(GLFWwindow* w, int width, int height) {
    glViewport(0, 0, width, height > 0 ? height : 1);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    gluOrtho2D(0, SCR_WIDTH, 0, SCR_HEIGHT);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action != GLFW_PRESS) return;
    if (key >= GLFW_KEY_1 && key <= GLFW_KEY_5) stage = key - GLFW_KEY_0;
    else if (key == GLFW_KEY_SPACE) {
        isPaused = !isPaused;
    }
    else if (key == GLFW_KEY_T) {
        // Manually trigger a test parcel
        static int manualCat = 1;
        manualCat = (manualCat % 3) + 1;
        const char* names[] = { "PCB Motherboard", "Laptop Battery", "Flat Monitor" };
        float wts[] = { 4.5f, 2.1f, 7.8f };
        spawnConveyorItem(manualCat, wts[manualCat - 1], names[manualCat - 1]);
    }
    else if (key == GLFW_KEY_S) {
        char name[32]; snprintf(name, sizeof(name), "stage%d_output.png", stage);
        saveScreenshot(window, name);
    }
}

void captureAllEvidence(GLFWwindow* window) {
    const char* f[] = { "bresenham_line_output.png", "bresenham_circle_output.png", "dda_primitives_output.png", "comparison_output.png", "ecosort_co2_station.png" };
    for (int i = 0; i < 5; i++) { stage = i + 1; display(); glFinish(); saveScreenshot(window, f[i]); }
}

void init() {
    glClearColor(1, 1, 1, 1);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    gluOrtho2D(0, SCR_WIDTH, 0, SCR_HEIGHT);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    glPointSize(1.5f);
}

int main(int argc, char** argv) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--capture-all") == 0) captureAll = true;
        else if (strcmp(argv[i], "--stage") == 0 && i + 1 < argc) stage = atoi(argv[++i]);
    }
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, SCR_TITLE, NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSwapInterval(1);
    init(); initFont();

    if (captureAll) {
        captureAllEvidence(window);
        glDeleteLists(fontBase, 128);
        glfwDestroyWindow(window); glfwTerminate();
        return 0;
    }

    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

        double currentTime = glfwGetTime();
        float dt = (float)(currentTime - lastTime);
        if (dt > 0.1f) dt = 0.1f;
        lastTime = currentTime;

        updateSimulation(dt);

        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteLists(fontBase, 128);
    glfwDestroyWindow(window); glfwTerminate();
    return 0;
}

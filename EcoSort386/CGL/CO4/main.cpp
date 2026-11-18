#include <windows.h>
#include <GLFW/glfw3.h>
#include <GL/glu.h>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <algorithm>
#include <queue>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "../../ecosort_bridge.h"

using namespace EcoSortCore;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ============================================================================
// EcoSort 386 - CGL Course Outcome 4 (CO4)
// Practical: Polygon Filling Algorithms (Scan-Line, Boundary Fill, Flood Fill)
// ============================================================================

const unsigned int SCR_WIDTH = 960, SCR_HEIGHT = 680;
const char* SCR_TITLE = "EcoSort 386 - CGL CO4 Polygon Filling & Infill Station";

int stage = 1; // 1: Scanline Fill, 2: Boundary Fill (4-Way), 3: Flood Fill, 4: Interactive Infill
bool animating = true;
bool captureRequested = false;
GLuint fontBase = 0;

// Algorithm Animation States
float scanlineY = 480.0f;
float scanlineSpeed = 120.0f; // pixels/sec
bool scanlineDone = false;
int pixelsFilledCount = 0;

// Grid buffer for Boundary & Flood fill demonstrations (300x220 logical resolution)
const int GRID_W = 320;
const int GRID_H = 240;
unsigned char gridBuffer[GRID_H][GRID_W]; // 0: empty, 1: boundary, 2: old_fill, 3: new_fill

struct Point2D {
    int x, y;
};

// Queue for BFS-based step-by-step Boundary / Flood fill animation
std::queue<Point2D> fillQueue;
bool fillActive = false;
int fillAlgorithmType = 1; // 1: Boundary Fill (4-way), 2: Flood Fill

// Geometry Vertices for Scan-Line Polygon Demo (Irregular E-Waste PCB Polygon)
const std::vector<Point2D> pcbPolygon = {
    {180, 160}, {320, 140}, {460, 200}, {540, 160},
    {680, 260}, {620, 420}, {480, 360}, {380, 480},
    {240, 440}, {160, 300}
};

// ----------------------------------------------------------------------------
// Font & 2D Helper Functions
// ----------------------------------------------------------------------------
void initFont() {
    HDC hdc = wglGetCurrentDC();
    HFONT font = CreateFontA(14, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                             ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
                             ANTIALIASED_QUALITY, FF_DONTCARE, "Arial");
    SelectObject(hdc, font);
    fontBase = glGenLists(128);
    wglUseFontBitmaps(hdc, 0, 128, fontBase);
}

void drawText(float x, float y, const char* s, float r = 1.0f, float g = 1.0f, float b = 1.0f) {
    glColor3f(r, g, b);
    glRasterPos2f(x, y);
    glPushAttrib(GL_LIST_BIT);
    glListBase(fontBase);
    glCallLists((GLsizei)strlen(s), GL_UNSIGNED_BYTE, s);
    glPopAttrib();
}

void drawRect(float x, float y, float w, float h, float r, float g, float b, float a = 1.0f) {
    glColor4f(r, g, b, a);
    glRectf(x, y, x + w, y + h);
}

void drawRectOutline(float x, float y, float w, float h, float r, float g, float b, float lw = 1.5f) {
    glLineWidth(lw);
    glColor3f(r, g, b);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
}

bool saveScreenshot(GLFWwindow* window, const char* filename) {
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    if (w <= 0) w = SCR_WIDTH;
    if (h <= 0) h = SCR_HEIGHT;

    std::vector<unsigned char> pixels(w * h * 3);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    std::vector<unsigned char> flipped(w * h * 3);
    for (int y = 0; y < h; y++) {
        memcpy(&flipped[y * w * 3], &pixels[(h - 1 - y) * w * 3], w * 3);
    }

    int res = stbi_write_png(filename, w, h, 3, flipped.data(), w * 3);
    return res != 0;
}

// ----------------------------------------------------------------------------
// Grid Buffer Initialization for Boundary & Flood Fill
// ----------------------------------------------------------------------------
void initBoundaryGrid() {
    memset(gridBuffer, 0, sizeof(gridBuffer));
    while (!fillQueue.empty()) fillQueue.pop();
    fillActive = false;
    pixelsFilledCount = 0;

    // Draw boundary box (simulating an e-waste component container)
    int cx = GRID_W / 2;
    int cy = GRID_H / 2;
    int rw = 90, rh = 65;

    // Outer boundary with an internal dividing wall
    for (int x = cx - rw; x <= cx + rw; x++) {
        gridBuffer[cy - rh][x] = 1;
        gridBuffer[cy + rh][x] = 1;
    }
    for (int y = cy - rh; y <= cy + rh; y++) {
        gridBuffer[y][cx - rw] = 1;
        gridBuffer[y][cx + rw] = 1;
    }

    // Add irregular internal partition
    for (int y = cy - rh + 20; y <= cy + rh - 20; y++) {
        gridBuffer[y][cx - 15] = 1;
    }
    for (int x = cx - 15; x <= cx + 45; x++) {
        gridBuffer[cy + 15][x] = 1;
    }

    // If Flood Fill demo (Stage 3), pre-populate interior with old color (2)
    if (stage == 3) {
        for (int y = cy - rh + 1; y < cy + rh; y++) {
            for (int x = cx - rw + 1; x < cx + rw; x++) {
                if (gridBuffer[y][x] == 0) {
                    gridBuffer[y][x] = 2; // Old color
                }
            }
        }
    }
}

void startFillAt(int startX, int startY, int algoType) {
    if (startX < 0 || startX >= GRID_W || startY < 0 || startY >= GRID_H) return;
    while (!fillQueue.empty()) fillQueue.pop();

    fillAlgorithmType = algoType;
    fillActive = true;

    if (algoType == 1) { // Boundary Fill: stop at boundary (1) or already filled (3)
        if (gridBuffer[startY][startX] != 1 && gridBuffer[startY][startX] != 3) {
            gridBuffer[startY][startX] = 3;
            fillQueue.push({startX, startY});
            pixelsFilledCount = 1;
        }
    } else { // Flood Fill: replace old color (2) with new color (3)
        if (gridBuffer[startY][startX] == 2) {
            gridBuffer[startY][startX] = 3;
            fillQueue.push({startX, startY});
            pixelsFilledCount = 1;
        }
    }
}

void stepFillBFS(int stepsPerFrame = 65) {
    if (!fillActive || fillQueue.empty()) {
        fillActive = false;
        return;
    }

    // 4-Connected directional offsets (North, South, East, West)
    const int dx[] = {0, 0, 1, -1};
    const int dy[] = {1, -1, 0, 0};

    for (int s = 0; s < stepsPerFrame && !fillQueue.empty(); s++) {
        Point2D pt = fillQueue.front();
        fillQueue.pop();

        for (int i = 0; i < 4; i++) {
            int nx = pt.x + dx[i];
            int ny = pt.y + dy[i];

            if (nx >= 0 && nx < GRID_W && ny >= 0 && ny < GRID_H) {
                if (fillAlgorithmType == 1) {
                    // Boundary fill: expand if not boundary (1) and not filled (3)
                    if (gridBuffer[ny][nx] != 1 && gridBuffer[ny][nx] != 3) {
                        gridBuffer[ny][nx] = 3;
                        fillQueue.push({nx, ny});
                        pixelsFilledCount++;
                    }
                } else {
                    // Flood fill: expand if matching old target color (2)
                    if (gridBuffer[ny][nx] == 2) {
                        gridBuffer[ny][nx] = 3;
                        fillQueue.push({nx, ny});
                        pixelsFilledCount++;
                    }
                }
            }
        }
    }
}

// ----------------------------------------------------------------------------
// Stage 1: Scan-Line Polygon Fill Algorithm
// ----------------------------------------------------------------------------
struct Edge {
    float yMax;
    float x;
    float invSlope; // dx / dy
};

void renderStage1_ScanLine(float currentY) {
    // 1. Draw Background grid & header
    drawText(40, 635, "STAGE 1: SCAN-LINE POLYGON FILL ALGORITHM (EDGE TABLE & PARITY RULE)", 0.2f, 0.8f, 1.0f);
    drawText(40, 615, "Concept: Compute horizontal edge intersections, sort ascending by X, and fill spans [X1, X2], [X3, X4].", 0.7f, 0.75f, 0.8f);

    // 2. Draw Polygon Boundary
    glLineWidth(2.5f);
    glColor3f(0.2f, 0.9f, 0.3f);
    glBegin(GL_LINE_LOOP);
    for (const auto& pt : pcbPolygon) {
        glVertex2i(pt.x, pt.y);
    }
    glEnd();

    // 3. Find Global yMin and yMax
    int yMin = 1000, yMax = -1000;
    for (const auto& pt : pcbPolygon) {
        if (pt.y < yMin) yMin = pt.y;
        if (pt.y > yMax) yMax = pt.y;
    }

    // 4. Fill scanlines from yMax down to currentY
    glPointSize(1.0f);
    glColor3f(0.12f, 0.55f, 0.35f); // PCB Emerald fill color

    int filledLines = 0;
    for (int y = yMax; y >= yMin; y--) {
        if (y < currentY) break; // Animation clamp

        // Find intersections with all polygon edges
        std::vector<float> xInts;
        size_t n = pcbPolygon.size();
        for (size_t i = 0; i < n; i++) {
            Point2D p1 = pcbPolygon[i];
            Point2D p2 = pcbPolygon[(i + 1) % n];

            if (p1.y == p2.y) continue; // Horizontal edge skip

            if ((y >= p1.y && y < p2.y) || (y >= p2.y && y < p1.y)) {
                float xi = p1.x + (float)(y - p1.y) * (p2.x - p1.x) / (p2.y - p1.y);
                xInts.push_back(xi);
            }
        }

        // Sort intersections left-to-right (Parity Principle)
        std::sort(xInts.begin(), xInts.end());

        // Draw horizontal spans between adjacent pairs
        glBegin(GL_LINES);
        for (size_t i = 0; i + 1 < xInts.size(); i += 2) {
            glVertex2f(xInts[i], (float)y);
            glVertex2f(xInts[i + 1], (float)y);
        }
        glEnd();

        filledLines++;

        // If this is the active scanning line, highlight intersections
        if (std::abs(y - (int)currentY) < 2) {
            // Draw active scanline vector
            glLineWidth(1.5f);
            glColor3f(1.0f, 0.85f, 0.1f);
            glBegin(GL_LINES);
            glVertex2f(60.0f, (float)y);
            glVertex2f(880.0f, (float)y);
            glEnd();

            // Draw intersection points
            glPointSize(8.0f);
            glColor3f(1.0f, 0.2f, 0.2f);
            glBegin(GL_POINTS);
            for (float xi : xInts) {
                glVertex2f(xi, (float)y);
            }
            glEnd();
        }
    }

    // Telemetry Box
    drawRect(700, 360, 220, 180, 0.08f, 0.12f, 0.18f, 0.95f);
    drawRectOutline(700, 360, 220, 180, 0.2f, 0.6f, 0.9f);
    drawText(715, 515, "SCAN-LINE TELEMETRY", 0.3f, 0.85f, 1.0f);

    char buf[64];
    snprintf(buf, sizeof(buf), "Active Y: %.0f px", currentY);
    drawText(715, 485, buf, 1.0f, 1.0f, 1.0f);

    snprintf(buf, sizeof(buf), "Y Range: %d to %d", yMin, yMax);
    drawText(715, 460, buf, 0.7f, 0.8f, 0.9f);

    snprintf(buf, sizeof(buf), "Lines Filled: %d", filledLines);
    drawText(715, 435, buf, 0.2f, 1.0f, 0.4f);

    snprintf(buf, sizeof(buf), "Status: %s", (currentY <= yMin ? "COMPLETED" : "SCANNING..."));
    drawText(715, 405, buf, (currentY <= yMin ? 0.2f : 1.0f), (currentY <= yMin ? 1.0f : 0.8f), 0.2f);

    drawText(715, 375, "[SPACE] Step / Re-run", 0.7f, 0.7f, 0.7f);
}

// ----------------------------------------------------------------------------
// Stage 2 & 3: Boundary Fill (4-Way) and Flood Fill Demos
// ----------------------------------------------------------------------------
void renderStage23_GridFill(const char* title, const char* subtitle) {
    drawText(40, 635, title, 0.2f, 0.8f, 1.0f);
    drawText(40, 615, subtitle, 0.7f, 0.75f, 0.8f);

    float originX = 140.0f;
    float originY = 120.0f;
    float cellScale = 2.0f;

    // Render grid buffer
    glPointSize(cellScale * 1.05f);
    glBegin(GL_POINTS);
    for (int y = 0; y < GRID_H; y++) {
        for (int x = 0; x < GRID_W; x++) {
            unsigned char val = gridBuffer[y][x];
            if (val == 0) continue; // background

            if (val == 1) { // Boundary (Cyan/White)
                glColor3f(0.85f, 0.95f, 1.0f);
            } else if (val == 2) { // Old Interior (Navy Blue)
                glColor3f(0.15f, 0.25f, 0.45f);
            } else if (val == 3) { // Newly Filled (Vibrant Amber / Gold)
                glColor3f(0.98f, 0.72f, 0.12f);
            }
            glVertex2f(originX + x * cellScale, originY + y * cellScale);
        }
    }
    glEnd();

    // Border around grid arena
    drawRectOutline(originX - 2, originY - 2, GRID_W * cellScale + 4, GRID_H * cellScale + 4, 0.3f, 0.5f, 0.8f);

    // Telemetry Panel
    drawRect(710, 360, 210, 180, 0.08f, 0.12f, 0.18f, 0.95f);
    drawRectOutline(710, 360, 210, 180, 0.2f, 0.6f, 0.9f);
    drawText(725, 515, (stage == 2 ? "BOUNDARY FILL (4-WAY)" : "FLOOD FILL ALGO"), 0.3f, 0.85f, 1.0f);

    char buf[64];
    snprintf(buf, sizeof(buf), "Pixels Infilled: %d", pixelsFilledCount);
    drawText(725, 485, buf, 1.0f, 1.0f, 1.0f);

    snprintf(buf, sizeof(buf), "Active Queue: %zu", fillQueue.size());
    drawText(725, 460, buf, 0.7f, 0.8f, 0.9f);

    snprintf(buf, sizeof(buf), "Expansion: 4-Connected");
    drawText(725, 435, buf, 0.9f, 0.7f, 0.2f);

    snprintf(buf, sizeof(buf), "State: %s", (fillActive ? "SPREADING..." : (pixelsFilledCount > 0 ? "DONE" : "IDLE (CLICK)")));
    drawText(725, 405, buf, 0.2f, 1.0f, 0.4f);

    drawText(725, 375, "[Click] Set Seed Point", 0.7f, 0.7f, 0.7f);
}

// ----------------------------------------------------------------------------
// Stage 4: Interactive Municipal Infill Station
// ----------------------------------------------------------------------------
void renderStage4_Station() {
    drawText(40, 635, "STAGE 4: INTERACTIVE E-WASTE MUNICIPAL INFILL FACILITY (CO4 CAPSTONE)", 0.2f, 0.8f, 1.0f);
    drawText(40, 615, "Apply polygon fill algorithms to color-code segregated bins and identify hazardous battery zones.", 0.7f, 0.75f, 0.8f);

    // 3 Segregated Bins with distinct boundary fills
    const char* binNames[] = {"1. REUSABLE CONTAINER", "2. RECYCLABLE CONTAINER", "3. HAZARDOUS QUARANTINE"};
    float binColors[3][3] = {
        {0.15f, 0.45f, 0.85f}, // Reusable: Royal Blue
        {0.12f, 0.75f, 0.42f}, // Recyclable: Emerald Green
        {0.92f, 0.55f, 0.10f}  // Hazardous: Amber / Orange
    };

    float bx = 60.0f;
    for (int i = 0; i < 3; i++) {
        // Bin Outer Frame
        drawRect(bx, 180, 240, 320, 0.08f, 0.12f, 0.18f);
        drawRectOutline(bx, 180, 240, 320, binColors[i][0], binColors[i][1], binColors[i][2], 2.0f);

        // Header Tab
        drawRect(bx, 460, 240, 40, binColors[i][0] * 0.7f, binColors[i][1] * 0.7f, binColors[i][2] * 0.7f);
        drawText(bx + 15, 475, binNames[i], 1.0f, 1.0f, 1.0f);

        // Fill indicator lines inside bin using scanline spacing
        for (int y = 200; y < 430; y += 12) {
            glLineWidth(1.5f);
            glColor3f(binColors[i][0] * 0.8f, binColors[i][1] * 0.8f, binColors[i][2] * 0.8f);
            glBegin(GL_LINES);
            glVertex2f(bx + 15, (float)y);
            glVertex2f(bx + 225, (float)y);
            glEnd();
        }

        // Capacity and Fill metric
        char buf[64];
        snprintf(buf, sizeof(buf), "Scanline Spans: %d lines", (430 - 200) / 12);
        drawText(bx + 20, 220, buf, 0.85f, 0.85f, 0.85f);
        snprintf(buf, sizeof(buf), "Capacity: 100%% Filled");
        drawText(bx + 20, 195, buf, 0.3f, 1.0f, 0.4f);

        bx += 280.0f;
    }

    // Telemetry Footer
    drawRect(60, 60, 840, 80, 0.06f, 0.09f, 0.14f);
    drawRectOutline(60, 60, 840, 80, 0.2f, 0.5f, 0.8f);
    drawText(80, 115, "INFILL STATION CONTROLS & IPC INTEGRATION", 0.3f, 0.85f, 1.0f);
    drawText(80, 90, "Keys [1-4]: Switch Algorithm Demos | [SPACE]: Trigger Sweep | [R]: Reset All Fills", 0.8f, 0.85f, 0.9f);
    drawText(80, 70, "Bridge Status: Synchronized with PL/PSOOP Core and COA 64-bit Scrap Multiplier.", 0.3f, 1.0f, 0.4f);
}

// ----------------------------------------------------------------------------
// Display & Render Loop
// ----------------------------------------------------------------------------
void display(GLFWwindow* window) {
    glClearColor(0.04f, 0.06f, 0.09f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, SCR_WIDTH, 0, SCR_HEIGHT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Top Navigation Tabs
    for (int i = 1; i <= 4; i++) {
        float tx = 40.0f + (i - 1) * 220.0f;
        bool active = (stage == i);
        drawRect(tx, 645, 205, 28, (active ? 0.15f : 0.08f), (active ? 0.35f : 0.12f), (active ? 0.65f : 0.18f));
        drawRectOutline(tx, 645, 205, 28, (active ? 0.3f : 0.15f), (active ? 0.8f : 0.3f), (active ? 1.0f : 0.5f));

        const char* tabTitles[] = {"1: Scan-Line Fill", "2: Boundary Fill", "3: Flood Fill", "4: Infill Station"};
        drawText(tx + 25, 655, tabTitles[i - 1], (active ? 1.0f : 0.6f), (active ? 1.0f : 0.6f), (active ? 1.0f : 0.6f));
    }

    if (stage == 1) {
        renderStage1_ScanLine(scanlineY);
    } else if (stage == 2) {
        renderStage23_GridFill("STAGE 2: BOUNDARY FILL ALGORITHM (4-CONNECTED RECURSIVE INFILL)",
                               "Concept: Expand recursively in 4 orthogonal directions until boundary color is encountered.");
    } else if (stage == 3) {
        renderStage23_GridFill("STAGE 3: FLOOD FILL ALGORITHM (TARGET COLOR REPLACEMENT)",
                               "Concept: Replaces all contiguous pixels matching the old target color with the new fill color.");
    } else if (stage == 4) {
        renderStage4_Station();
    }

    glfwSwapBuffers(window);
}

// ----------------------------------------------------------------------------
// Key & Mouse Callbacks
// ----------------------------------------------------------------------------
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action != GLFW_PRESS) return;

    if (key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    } else if (key == GLFW_KEY_1) {
        stage = 1;
        scanlineY = 480.0f;
    } else if (key == GLFW_KEY_2) {
        stage = 2;
        initBoundaryGrid();
        startFillAt(GRID_W / 2, GRID_H / 2, 1);
    } else if (key == GLFW_KEY_3) {
        stage = 3;
        initBoundaryGrid();
        startFillAt(GRID_W / 2, GRID_H / 2, 2);
    } else if (key == GLFW_KEY_4) {
        stage = 4;
    } else if (key == GLFW_KEY_SPACE) {
        if (stage == 1) {
            scanlineY = 480.0f;
        } else if (stage == 2 || stage == 3) {
            initBoundaryGrid();
            startFillAt(GRID_W / 2, GRID_H / 2, (stage == 2 ? 1 : 2));
        }
    } else if (key == GLFW_KEY_R) {
        scanlineY = 480.0f;
        initBoundaryGrid();
    } else if (key == GLFW_KEY_P || key == GLFW_KEY_S) {
        saveScreenshot(window, "ecosort_co4_filling.png");
        printf("[SUCCESS] Captured screenshot: ecosort_co4_filling.png\n");
    }
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double mx, my;
        glfwGetCursorPos(window, &mx, &my);
        my = SCR_HEIGHT - my; // Invert to OpenGL bottom-left origin

        if (stage == 2 || stage == 3) {
            float originX = 140.0f;
            float originY = 120.0f;
            float cellScale = 2.0f;

            int gx = (int)((mx - originX) / cellScale);
            int gy = (int)((my - originY) / cellScale);

            if (gx >= 0 && gx < GRID_W && gy >= 0 && gy < GRID_H) {
                startFillAt(gx, gy, (stage == 2 ? 1 : 2));
            }
        }
    }
}

// ----------------------------------------------------------------------------
// Main Function
// ----------------------------------------------------------------------------
int main(int argc, char** argv) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--capture") == 0 || strcmp(argv[i], "--capture-all") == 0) {
            captureRequested = true;
        }
    }

    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW.\n");
        return -1;
    }

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, SCR_TITLE, NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window.\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable V-Sync (60 FPS)

    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    initFont();
    initBoundaryGrid();

    // Start initial boundary fill demo
    startFillAt(GRID_W / 2, GRID_H / 2, 1);

    if (captureRequested) {
        // Render Stage 1 Scanline midway for evidence screenshot
        stage = 1;
        scanlineY = 310.0f;
        display(window);
        glFinish();
        saveScreenshot(window, "ecosort_co4_filling.png");
        printf("[SUCCESS] Generated evidence screenshot: ecosort_co4_filling.png\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return 0;
    }

    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        float dt = (float)(currentTime - lastTime);
        lastTime = currentTime;

        // Animate Scanline sweep
        if (stage == 1) {
            scanlineY -= scanlineSpeed * dt;
            if (scanlineY < 130.0f) {
                scanlineY = 480.0f; // Loop scanline
            }
        }

        // Animate Boundary / Flood fill BFS expansion
        if (stage == 2 || stage == 3) {
            stepFillBFS(75);
        }

        display(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

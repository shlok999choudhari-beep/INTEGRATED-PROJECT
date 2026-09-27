#include <windows.h>
#include <GLFW/glfw3.h>
#include <GL/glu.h>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <vector>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

const unsigned int SCR_WIDTH = 900, SCR_HEIGHT = 650;
const char* SCR_TITLE = "EcoSort 386 - CGL CO2: Bresenham & DDA Algorithms";
int stage = 5;
bool captureAll = false;
GLuint fontBase = 0;

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

void drawCircleDDA(int xc, int yc, int radius, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_POINTS);
    int n = 0; while ((1 << n) < radius) n++;
    float eps = 1.0f / (float)(1 << (n + 1));
    float x = 0.0f, y = (float)radius;
    int steps = (int)(2.0f * (float)M_PI / eps) + 1;
    for (int i = 0; i <= steps; i++) {
        glVertex2i(xc + (int)roundf(x), yc + (int)roundf(y));
        x = x + eps * y;
        y = y - eps * x;
    }
    glEnd();
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

void stage1() {
    glClearColor(0.96f, 0.97f, 0.98f, 1); glClear(GL_COLOR_BUFFER_BIT);
    int cx = 450, cy = 320, len = 250;
    for (int deg = 0; deg < 360; deg += 15) {
        float rad = deg * (float)M_PI / 180.0f;
        int tx = cx + (int)(len * cosf(rad)), ty = cy + (int)(len * sinf(rad));
        float r = 0.2f + 0.7f * (deg / 360.0f), g = 0.3f, b = 1.0f - 0.7f * (deg / 360.0f);
        drawLineBresenham(cx, cy, tx, ty, r, g, b);
    }
    drawText(50, 610, "EcoSort 386 - CGL CO2 | Stage 1: Bresenham Line Drawing (All 8 Octants)", 0.1f, 0.2f, 0.4f);
    drawText(50, 585, "Formula: P_k+1 = P_k + 2dy - 2dx (Pure Integer Arithmetic, No Floats)", 0.3f, 0.3f, 0.3f);
    drawText(50, 40, "[Keys 1-5]: Switch Stages | [S]: Save Screenshot | [ESC]: Quit", 0.5f, 0.5f, 0.5f);
}

void stage2() {
    glClearColor(0.96f, 0.97f, 0.98f, 1); glClear(GL_COLOR_BUFFER_BIT);
    int cx = 450, cy = 330;
    drawCircleBresenham(cx, cy, 180, 0, 0, 0, true);
    for (int r : { 50, 90, 135, 225, 270 }) drawCircleBresenham(cx, cy, r, 0.4f, 0.5f, 0.6f);
    drawLineBresenham(cx - 280, cy, cx + 280, cy, 0.75f, 0.75f, 0.75f);
    drawLineBresenham(cx, cy - 280, cx, cy + 280, 0.75f, 0.75f, 0.75f);
    drawLineBresenham(cx - 200, cy - 200, cx + 200, cy + 200, 0.85f, 0.85f, 0.85f);
    drawLineBresenham(cx - 200, cy + 200, cx + 200, cy - 200, 0.85f, 0.85f, 0.85f);
    drawText(50, 610, "EcoSort 386 - CGL CO2 | Stage 2: Bresenham / Midpoint Circle (8-Way Symmetry)", 0.1f, 0.2f, 0.4f);
    drawText(50, 585, "8 Colors Prove 8-Way Symmetry: Compute (x, y) for 45 deg, Mirror to All 8 Octants (87.5% Math Savings)", 0.3f, 0.3f, 0.3f);
    drawText(50, 40, "[Keys 1-5]: Switch Stages | [S]: Save Screenshot | [ESC]: Quit", 0.5f, 0.5f, 0.5f);
}

void stage3() {
    glClearColor(0.96f, 0.97f, 0.98f, 1); glClear(GL_COLOR_BUFFER_BIT);
    for (int y = 140; y <= 500; y += 40) drawLineDDA(80, y, 420, y, 0.2f, 0.6f, 0.3f);
    for (int x = 80; x <= 420; x += 40) drawLineDDA(x, 140, x, 500, 0.2f, 0.6f, 0.3f);
    drawLineDDA(80, 140, 420, 500, 0.9f, 0.2f, 0.2f);
    drawLineDDA(80, 500, 420, 140, 0.1f, 0.4f, 0.9f);
    int cx = 660, cy = 320;
    for (int r = 40; r <= 190; r += 30) drawCircleDDA(cx, cy, r, 0.8f - r * 0.003f, 0.3f, 0.2f + r * 0.003f);
    drawText(50, 610, "EcoSort 386 - CGL CO2 | Stage 3: DDA Line & DDA Circle Scan Conversion", 0.1f, 0.2f, 0.4f);
    drawText(80, 105, "DDA Line Grid: x_k+1 = x_k + 1, y_k+1 = y_k + m", 0.2f, 0.4f, 0.2f);
    drawText(520, 105, "DDA Circle: x_n+1 = x_n + eps*y_n, y_n+1 = y_n - eps*x_n+1", 0.6f, 0.2f, 0.2f);
    drawText(50, 40, "[Keys 1-5]: Switch Stages | [S]: Save Screenshot | [ESC]: Quit", 0.5f, 0.5f, 0.5f);
}

void stage4() {
    glClearColor(0.96f, 0.97f, 0.98f, 1); glClear(GL_COLOR_BUFFER_BIT);
    drawLineBresenham(450, 70, 450, 570, 0.7f, 0.7f, 0.7f);
    for (int deg = 0; deg < 360; deg += 30) {
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
    drawText(50, 30, "[Keys 1-5]: Switch Stages | [S]: Save Screenshot | [ESC]: Quit", 0.5f, 0.5f, 0.5f);
}

void stage5() {
    glClearColor(0.93f, 0.95f, 0.97f, 1); glClear(GL_COLOR_BUFFER_BIT);
    // Draw Conveyor Frame using Bresenham Lines
    drawLineBresenham(80, 340, 820, 340, 0.2f, 0.25f, 0.3f);
    drawLineBresenham(80, 280, 820, 280, 0.2f, 0.25f, 0.3f);
    for (int x = 80; x <= 820; x += 40) drawLineBresenham(x, 280, x, 340, 0.4f, 0.45f, 0.5f);
    for (int x = 120; x <= 780; x += 110) {
        drawCircleBresenham(x, 310, 22, 0.15f, 0.55f, 0.4f);
        drawCircleBresenham(x, 310, 10, 0.7f, 0.75f, 0.8f);
    }
    // Overhead Optical Sensor Scanner using Bresenham & DDA Circles
    int sx = 450, sy = 480;
    drawLineBresenham(sx - 120, 560, sx + 120, 560, 0.1f, 0.2f, 0.35f);
    drawLineBresenham(sx, 560, sx, sy + 40, 0.2f, 0.3f, 0.4f);
    drawCircleBresenham(sx, sy, 40, 0.1f, 0.4f, 0.85f);
    drawCircleDDA(sx, sy, 55, 0.85f, 0.65f, 0.1f);
    for (int deg = -55; deg <= 55; deg += 11) {
        float a = (deg - 90) * (float)M_PI / 180.0f;
        drawLineBresenham(sx, sy, sx + (int)(140 * cosf(a)), sy + (int)(140 * sinf(a)), 0.95f, 0.25f, 0.15f);
    }
    // 3 E-Waste Sorting Bins below conveyor
    auto drawSortBin = [&](int bx, int by, int bw, int bh, float r, float g, float b, const char* code, const char* label) {
        drawLineBresenham(bx, by, bx + bw, by, r, g, b);
        drawLineBresenham(bx, by, bx, by + bh, r, g, b);
        drawLineBresenham(bx + bw, by, bx + bw, by + bh, r, g, b);
        drawLineBresenham(bx - 8, by + bh, bx + bw + 8, by + bh, r * 0.8f, g * 0.8f, b * 0.8f);
        drawCircleBresenham(bx + bw / 2, by + bh / 2, 24, r, g, b);
        drawCircleDDA(bx + bw / 2, by + bh / 2, 32, r * 0.7f, g * 0.7f, b * 0.7f);
        drawText(bx + bw / 2 - 5, by + bh / 2 - 5, code, r, g, b);
        drawText(bx, by - 22, label, 0.2f, 0.2f, 0.25f);
    };
    drawSortBin(120, 90, 160, 130, 0, 0.65f, 0.25f, "R", "Recyclable (Bresenham)");
    drawSortBin(370, 90, 160, 130, 0, 0.45f, 0.85f, "U", "Reusable (DDA Circles)");
    drawSortBin(620, 90, 160, 130, 0.85f, 0.2f, 0.2f, "H", "Hazardous (Hybrid)");
    drawText(50, 615, "ECOSORT 386 - AUTOMATED OPTICAL E-WASTE SORTING STATION", 0.08f, 0.18f, 0.32f);
    drawText(50, 590, "CGL CO2 Capstone: Conveyor, Optical Sensor & Bins Drawn Purely with Bresenham & DDA Algorithms", 0.3f, 0.35f, 0.4f);
    drawText(50, 25, "[Keys 1-5]: Switch Stages | [S]: Save Current Screenshot | [ESC]: Quit", 0.4f, 0.4f, 0.4f);
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
    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteLists(fontBase, 128);
    glfwDestroyWindow(window); glfwTerminate();
    return 0;
}

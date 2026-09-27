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
const char* SCR_TITLE = "EcoSort 386 - CGL CO1 Kiosk & Dispatch";
int stage = 5;
bool captureAll = false, animating = true;
float truckX = -0.05f, destX = -0.28f;
GLuint fontBase = 0;

void initFont() {
    HDC hdc = wglGetCurrentDC();
    HFONT font = CreateFontA(14, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE, "Arial");
    SelectObject(hdc, font);
    fontBase = glGenLists(128);
    wglUseFontBitmaps(hdc, 0, 128, fontBase);
}
void drawText(float x, float y, const char* s, float r = 0.1f, float g = 0.1f, float b = 0.1f) {
    glColor3f(r, g, b); glRasterPos2f(x, y);
    glPushAttrib(GL_LIST_BIT); glListBase(fontBase); glCallLists((GLsizei)strlen(s), GL_UNSIGNED_BYTE, s); glPopAttrib();
}
void drawRect(float x, float y, float w, float h, float r, float g, float b, float a = 1.0f) {
    glColor4f(r, g, b, a); glRectf(x, y, x + w, y + h);
}
void drawRectOutline(float x, float y, float w, float h, float r, float g, float b, float lw = 1.5f) {
    glLineWidth(lw); glColor3f(r, g, b); glBegin(GL_LINE_LOOP);
    glVertex2f(x, y); glVertex2f(x + w, y); glVertex2f(x + w, y + h); glVertex2f(x, y + h); glEnd();
}
void drawCircle(float cx, float cy, float r, int segs, float red, float green, float blue) {
    glColor3f(red, green, blue); glBegin(GL_TRIANGLE_FAN); glVertex2f(cx, cy);
    for (int i = 0; i <= segs; i++) glVertex2f(cx + r * cosf(2.0f * (float)M_PI * i / segs), cy + r * sinf(2.0f * (float)M_PI * i / segs));
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

void drawBin(float x, float y, float w, float h, float r, float g, float b, const char* code, const char* label = "", bool showLabel = true, bool highlight = false) {
    if (highlight) drawRectOutline(x - 0.02f, y - 0.02f, w + 0.04f, h + 0.04f, 1, 0.85f, 0.1f, 3.0f);
    drawRect(x + 0.02f, y - 0.02f, w, h, 0, 0, 0, 0.15f);
    glBegin(GL_QUADS);
    glColor3f(r * 0.85f, g * 0.85f, b * 0.85f); glVertex2f(x, y); glColor3f(r, g, b); glVertex2f(x + w, y);
    glColor3f(r * 1.15f, g * 1.15f, b * 1.15f); glVertex2f(x + w, y + h); glColor3f(r * 0.95f, g * 0.95f, b * 0.95f); glVertex2f(x, y + h);
    glEnd();
    drawRectOutline(x, y, w, h, r * 0.4f, g * 0.4f, b * 0.4f, 2.0f);
    float ov = (w < 0.2f) ? 0.015f : 0.03f, lh = (w < 0.2f) ? 0.035f : 0.06f;
    drawRect(x - ov, y + h, w + 2 * ov, lh, r * 0.6f, g * 0.6f, b * 0.6f);
    drawRect(x + w * 0.25f, y + h + lh * 0.35f, w * 0.5f, lh * 0.4f, 0.2f, 0.2f, 0.2f);
    glBegin(GL_TRIANGLES);
    glColor3f(1, 1, 1); glVertex2f(x + w * 0.25f, y + h * 0.55f); glVertex2f(x + w * 0.75f, y + h * 0.55f); glVertex2f(x + w * 0.5f, y + h * 0.80f);
    glEnd();
    drawText(x + (w < 0.2f ? w * 0.35f : w * 0.42f), y + h * 0.22f, code, 1, 1, 1);
    if (showLabel && label && label[0]) drawText(x - (w < 0.2f ? 0.015f : 0.02f), y - (w < 0.2f ? 0.045f : 0.09f), label, 0.15f, 0.15f, 0.15f);
}

void drawBuilding(float x, float y, float w, float h, float r, float g, float b, const char* label) {
    glBegin(GL_QUADS); glColor3f(r, g, b); glVertex2f(x, y);
    glColor3f(r * 1.15f, g * 1.15f, b * 1.15f); glVertex2f(x + w, y); glVertex2f(x + w, y + h);
    glColor3f(r * 0.95f, g * 0.95f, b * 0.95f); glVertex2f(x, y + h); glEnd();
    drawRectOutline(x, y, w, h, r * 0.4f, g * 0.4f, b * 0.4f);
    float px = w * 0.12f, py = h * 0.08f, ww = (w - px * 4) / 3, wh = (h - py * 6) / 5;
    for (int ro = 0; ro < 5; ro++)
        for (int co = 0; co < 3; co++)
            drawRect(x + px + co * (ww + px), y + py + ro * (wh + py), ww, wh, (ro + co) % 3 == 0 ? 1.0f : 0.35f, (ro + co) % 3 == 0 ? 0.92f : 0.45f, (ro + co) % 3 == 0 ? 0.55f : 0.60f);
    drawText(x + w * 0.10f, y + h + 0.018f, label, 0.15f, 0.22f, 0.32f);
}

void drawTruck(float x, float y) {
    drawRect(x, y + 0.08f, 0.38f, 0.24f, 0, 0.65f, 0.55f);
    drawRectOutline(x, y + 0.08f, 0.38f, 0.24f, 0, 0.35f, 0.30f, 2.0f);
    glColor3f(0.95f, 0.95f, 0.95f);
    glBegin(GL_POLYGON);
    glVertex2f(x + 0.38f, y + 0.08f); glVertex2f(x + 0.54f, y + 0.08f); glVertex2f(x + 0.54f, y + 0.20f);
    glVertex2f(x + 0.48f, y + 0.28f); glVertex2f(x + 0.38f, y + 0.28f); glEnd();
    drawRect(x + 0.42f, y + 0.18f, 0.07f, 0.08f, 0.4f, 0.75f, 0.95f);
    drawRect(x - 0.02f, y + 0.05f, 0.57f, 0.03f, 0.2f, 0.2f, 0.25f);
    drawRect(x + 0.53f, y + 0.10f, 0.02f, 0.05f, 1, 0.9f, 0.2f);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_TRIANGLES);
    glColor4f(1, 0.95f, 0.4f, 0.45f); glVertex2f(x + 0.55f, y + 0.125f);
    glColor4f(1, 0.95f, 0.4f, 0); glVertex2f(x + 0.85f, y + 0.25f); glVertex2f(x + 0.85f, y);
    glEnd(); glDisable(GL_BLEND);
    drawText(x + 0.05f, y + 0.18f, "ECOSORT 386", 1, 1, 1);
    drawText(x + 0.07f, y + 0.12f, "E-Logistics", 0.85f, 1, 0.9f);
    for (float wx : { x + 0.10f, x + 0.44f }) {
        drawCircle(wx, y + 0.05f, 0.055f, 24, 0.15f, 0.15f, 0.15f);
        drawCircle(wx, y + 0.05f, 0.030f, 16, 0.75f, 0.75f, 0.80f);
        float a = -x * 30.0f; glLineWidth(1.8f); glColor3f(0.2f, 0.2f, 0.2f); glBegin(GL_LINES);
        for (int i = 0; i < 4; i++, a += (float)M_PI / 2.0f) {
            glVertex2f(wx, y + 0.05f); glVertex2f(wx + 0.045f * cosf(a), y + 0.05f + 0.045f * sinf(a));
        }
        glEnd();
    }
}

void stage1() {
    glClearColor(1, 1, 1, 1); glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_TRIANGLES); glColor3f(0, 1, 0); glVertex2f(-0.5f, -0.5f); glVertex2f(0.5f, -0.5f); glVertex2f(0, 0.5f); glEnd();
    drawText(-0.85f, 0.85f, "EcoSort 386 - CGL CO1 | Practical No. 01: OpenGL Triangle", 0.1f, 0.3f, 0.1f);
    drawText(-0.85f, 0.78f, "Primitive: GL_TRIANGLES | Projection: gluOrtho2D(-1.0, 1.0, -1.0, 1.0)", 0.4f, 0.4f, 0.4f);
    drawText(-0.85f, -0.92f, "[Keys 1-5]: Switch Stages | [S]: Save Screenshot | [ESC]: Quit", 0.5f, 0.5f, 0.5f);
}
void stage2() {
    glClearColor(1, 1, 1, 1); glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_TRIANGLES); glColor3f(0, 0.85f, 0.2f); glVertex2f(-0.8f, 0.4f); glVertex2f(-0.3f, 0.4f); glVertex2f(-0.55f, 0.85f); glEnd();
    drawRect(-0.8f, -0.4f, 0.4f, 0.6f, 0, 0.5f, 1);
    glColor3f(1, 0.6f, 0.1f); glBegin(GL_POLYGON); glVertex2f(0.2f, -0.3f); glVertex2f(0.75f, -0.3f); glVertex2f(0.85f, 0.15f); glVertex2f(0.48f, 0.45f); glVertex2f(0.1f, 0.15f); glEnd();
    glLineWidth(4.0f); glColor3f(0.2f, 0.2f, 0.2f); glBegin(GL_LINES); glVertex2f(-1, -0.7f); glVertex2f(1, -0.7f); glVertex2f(-1, -0.85f); glVertex2f(1, -0.85f); glEnd();
    glLineWidth(2.5f); glColor3f(0.95f, 0.8f, 0); glBegin(GL_LINES); for (float x = -0.95f; x < 1; x += 0.25f) { glVertex2f(x, -0.775f); glVertex2f(x + 0.12f, -0.775f); } glEnd();
    drawText(-0.85f, 0.92f, "EcoSort 386 - CGL CO1 | Stage 2: Geometric Primitives (Triangles, Quads, Lines, Polygon)", 0.1f, 0.2f, 0.4f);
    drawText(-0.85f, -0.95f, "[Keys 1-5]: Switch Stages | [S]: Save Screenshot | [ESC]: Quit", 0.5f, 0.5f, 0.5f);
}
void stage3() {
    glClearColor(0.96f, 0.97f, 0.98f, 1); glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_TRIANGLES); glColor3f(1, 0.1f, 0.1f); glVertex2f(-0.75f, 0.2f); glColor3f(0.1f, 0.9f, 0.2f); glVertex2f(-0.25f, 0.2f); glColor3f(0.1f, 0.4f, 1); glVertex2f(-0.50f, 0.8f); glEnd();
    glBegin(GL_QUADS); glColor3f(0.15f, 0.20f, 0.35f); glVertex2f(0.1f, 0.2f); glColor3f(0.25f, 0.35f, 0.55f); glVertex2f(0.7f, 0.2f);
    glColor3f(0.45f, 0.65f, 0.90f); glVertex2f(0.7f, 0.8f); glColor3f(0.30f, 0.45f, 0.70f); glVertex2f(0.1f, 0.8f); glEnd();
    drawCircle(-0.5f, -0.35f, 0.25f, 48, 0.2f, 0.2f, 0.25f); drawCircle(-0.5f, -0.35f, 0.18f, 36, 0.8f, 0.85f, 0.9f); drawCircle(-0.5f, -0.35f, 0.08f, 24, 0.2f, 0.6f, 0.3f);
    glBegin(GL_POLYGON); glColor3f(1, 0.85f, 0); glVertex2f(0.4f, -0.65f); glColor3f(0.95f, 0.45f, 0); glVertex2f(0.65f, -0.40f); glColor3f(0.9f, 0.1f, 0.1f); glVertex2f(0.4f, -0.15f); glColor3f(0.95f, 0.45f, 0); glVertex2f(0.15f, -0.40f); glEnd();
    drawText(-0.85f, 0.92f, "EcoSort 386 - CGL CO1 | Stage 3: Colored Objects & Interpolated Shading", 0.1f, 0.2f, 0.3f);
    drawText(-0.85f, -0.95f, "[Keys 1-5]: Switch Stages | [S]: Save Screenshot | [ESC]: Quit", 0.5f, 0.5f, 0.5f);
}
void stage4() {
    glClearColor(0.94f, 0.96f, 0.97f, 1); glClear(GL_COLOR_BUFFER_BIT);
    drawRect(-1, 0.80f, 2, 0.20f, 0.1f, 0.25f, 0.4f);
    drawText(-0.85f, 0.88f, "ECOSORT 386 - E-WASTE SEGREGATION BINS", 1, 1, 1);
    drawText(-0.85f, 0.83f, "Stage 4: Modular Graphics Primitives (GL_QUADS, GL_TRIANGLES)", 0.8f, 0.9f, 1);
    drawRect(-1, -0.6f, 2, 0.2f, 0.82f, 0.85f, 0.88f);
    drawBin(-0.75f, -0.40f, 0.36f, 0.65f, 0, 0.70f, 0.25f, "R", "Recyclable E-Waste");
    drawBin(-0.18f, -0.40f, 0.36f, 0.65f, 0, 0.45f, 0.90f, "U", "Reusable Hardware");
    drawBin( 0.38f, -0.40f, 0.36f, 0.65f, 0.88f, 0.15f, 0.15f, "H", "Hazardous / Batteries");
    drawText(-0.85f, -0.95f, "[Keys 1-5]: Switch Stages | [S]: Save Screenshot | [ESC]: Quit", 0.5f, 0.5f, 0.5f);
}

void triggerDeposit() {
    auto& br = Bridge::get();
    br.triggerDeposit();
    truckX = br.truckX; destX = br.destX;
}

void stage5() {
    auto& br = Bridge::get();
    glClearColor(0.85f, 0.92f, 0.98f, 1); glClear(GL_COLOR_BUFFER_BIT);
    drawRect(-1, 0.82f, 2, 0.18f, 0.08f, 0.18f, 0.28f);
    drawText(-0.90f, 0.93f, "ECOSORT 386 - SMART E-WASTE DEPOSIT KIOSK & DISPATCH", 1, 1, 1);
    char buf[128];
    if (br.txState == 0) snprintf(buf, sizeof(buf), "[KIOSK] Active: %s (+%d Cr) | Press [D / ENTER] or Click Button to Deposit", br.getCategoryName(), br.getCategoryReward());
    else if (br.txState == 1) snprintf(buf, sizeof(buf), "[TRANSACTION IN PROGRESS] Dispatching E-Waste Truck to Intake Bay %d...", br.getTargetBay());
    else snprintf(buf, sizeof(buf), "[TRANSACTION COMPLETE] Verified at Bay %d! User Balance: %d Credits", br.getTargetBay(), br.userCredits);
    drawText(-0.90f, 0.86f, buf, br.txState == 2 ? 0.3f : 0.4f, br.txState == 2 ? 1.0f : 0.85f, br.txState == 2 ? 0.4f : 0.75f);

    drawRect(0.52f, 0.85f, 0.42f, 0.10f, br.txState == 1 ? 0.5f : 0.0f, br.txState == 1 ? 0.5f : 0.65f, br.txState == 1 ? 0.5f : 0.35f);
    drawRectOutline(0.52f, 0.85f, 0.42f, 0.10f, 1, 1, 1, 1.5f);
    drawText(0.56f, 0.89f, br.txState == 1 ? "DISPATCHING..." : ">> CLICK TO DEPOSIT <<", 1, 1, 1);

    drawBuilding(-0.92f, 0.18f, 0.30f, 0.48f, 0.35f, 0.40f, 0.48f, "IT Tech Park");
    drawBuilding(-0.55f, 0.18f, 0.26f, 0.44f, 0.48f, 0.42f, 0.38f, "Residency Block");
    drawBuilding( 0.32f, 0.18f, 0.26f, 0.46f, 0.38f, 0.44f, 0.42f, "Electronics Hub");
    drawBuilding( 0.64f, 0.18f, 0.30f, 0.50f, 0.32f, 0.36f, 0.46f, "Data Center");
    drawRect(-1, 0.12f, 2, 0.06f, 0.72f, 0.74f, 0.76f); drawRect(-1, -0.22f, 2, 0.34f, 0.22f, 0.24f, 0.26f);
    glLineWidth(2.5f); glColor3f(0.9f, 0.9f, 0.9f); glBegin(GL_LINES); glVertex2f(-1, 0.11f); glVertex2f(1, 0.11f); glVertex2f(-1, -0.21f); glVertex2f(1, -0.21f); glEnd();
    glLineWidth(3.0f); glColor3f(0.98f, 0.85f, 0.1f); glBegin(GL_LINES); for (float x = -0.96f; x < 1; x += 0.20f) { glVertex2f(x, -0.05f); glVertex2f(x + 0.10f, -0.05f); } glEnd();
    drawRect(-1, -0.32f, 2, 0.10f, 0.68f, 0.70f, 0.72f);

    drawBin(-0.90f, 0.15f, 0.14f, 0.24f, 0, 0.75f, 0.25f, "R", "Recycle [R]", true, br.activeCategory == 1);
    drawBin(-0.25f, 0.15f, 0.14f, 0.24f, 0.88f, 0.15f, 0.15f, "H", "Hazard [H]", true, br.activeCategory == 2);
    drawBin( 0.72f, 0.15f, 0.14f, 0.24f, 0, 0.45f, 0.90f, "U", "Reuse [U]", true, br.activeCategory == 3);

    drawTruck(truckX, -0.18f);

    float fx = -0.65f, fy = -0.85f, fw = 1.30f, fh = 0.48f;
    drawRect(fx, fy, fw, fh, 0.88f, 0.90f, 0.92f);
    glBegin(GL_TRIANGLES); glColor3f(0.12f, 0.45f, 0.35f); glVertex2f(fx - 0.05f, fy + fh); glVertex2f(fx + fw + 0.05f, fy + fh); glVertex2f(fx + fw * 0.5f, fy + fh + 0.15f); glEnd();
    drawRectOutline(fx, fy, fw, fh, 0.2f, 0.3f, 0.35f, 2.0f);
    const char* bays[] = { "BAY 1: INTAKE", "BAY 2: HAZARD", "BAY 3: REUSE" };
    for (int i = 0; i < 3; i++) {
        bool act = (br.txState > 0 && br.activeCategory == i + 1);
        drawRect(fx + 0.12f + i * 0.38f, fy, 0.26f, 0.28f, act ? 0.12f : 0.3f, act ? 0.60f : 0.35f, act ? 0.35f : 0.40f);
        drawText(fx + 0.16f + i * 0.38f, fy + 0.12f, bays[i], 1, 1, 1);
    }
    drawRect(fx + 0.22f, fy + fh - 0.12f, fw - 0.44f, 0.09f, 0, 0.45f, 0.35f);
    drawText(fx + 0.27f, fy + fh - 0.09f, "CENTRAL E-WASTE RECOVERY & PROCESSING DEPOT", 1, 1, 1);
    drawText(-0.95f, -0.95f, "[Keys 1-5]: Stages | [R/H/U/Click Bins]: Select | [D/Enter]: Deposit & Dispatch | [Space]: Pause", 0.2f, 0.2f, 0.2f);
    snprintf(buf, sizeof(buf), "Load: %.1f kg | %d Cr", br.lastItemWeight, br.userCredits);
    drawText(0.60f, -0.95f, buf, 0, 0.45f, 0.25f);
}

void display() {
    if (stage == 1) stage1(); else if (stage == 2) stage2(); else if (stage == 3) stage3();
    else if (stage == 4) stage4(); else stage5();
    glFlush();
}
void framebuffer_size_callback(GLFWwindow* w, int width, int height) {
    glViewport(0, 0, width, height > 0 ? height : 1);
    glMatrixMode(GL_PROJECTION); glLoadIdentity(); gluOrtho2D(-1, 1, -1, 1);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
}
void mouse_button_callback(GLFWwindow* w, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && stage == 5) {
        double mx, my; glfwGetCursorPos(w, &mx, &my);
        int width, height; glfwGetWindowSize(w, &width, &height);
        float ox = (float)mx / width * 2.0f - 1.0f, oy = 1.0f - (float)my / height * 2.0f;
        if (ox >= -0.92f && ox <= -0.74f && oy >= 0.12f && oy <= 0.45f) Bridge::get().setCategory(1);
        else if (ox >= -0.27f && ox <= -0.09f && oy >= 0.12f && oy <= 0.45f) Bridge::get().setCategory(2);
        else if (ox >= 0.70f && ox <= 0.88f && oy >= 0.12f && oy <= 0.45f) Bridge::get().setCategory(3);
        else if (ox >= 0.52f && ox <= 0.94f && oy >= 0.85f && oy <= 0.95f) triggerDeposit();
    }
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action != GLFW_PRESS && action != GLFW_REPEAT) return;
    if (key >= GLFW_KEY_1 && key <= GLFW_KEY_5) stage = key - GLFW_KEY_0;
    else if (key == GLFW_KEY_SPACE) animating = !animating;
    else if (key == GLFW_KEY_R) Bridge::get().setCategory(1);
    else if (key == GLFW_KEY_H) Bridge::get().setCategory(2);
    else if (key == GLFW_KEY_U) Bridge::get().setCategory(3);
    else if (key == GLFW_KEY_D || key == GLFW_KEY_ENTER) triggerDeposit();
    else if (key == GLFW_KEY_RIGHT) truckX += 0.035f;
    else if (key == GLFW_KEY_LEFT) truckX -= 0.035f;
    else if (key == GLFW_KEY_S) {
        char name[32]; snprintf(name, sizeof(name), "stage%d_output.png", stage); saveScreenshot(window, name);
    }
}
void captureAllEvidence(GLFWwindow* window) {
    const char* f[] = { "triangle_output.png", "primitives_output.png", "colored_objects.png", "ecosort_bins.png", "ecosort_week1_final.png" };
    truckX = -0.05f;
    for (int i = 0; i < 5; i++) { stage = i + 1; display(); glFinish(); saveScreenshot(window, f[i]); }
}
void init() {
    glClearColor(1, 1, 1, 1);
    glMatrixMode(GL_PROJECTION); glLoadIdentity(); gluOrtho2D(-1, 1, -1, 1);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    glEnable(GL_LINE_SMOOTH); glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
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
    glfwSetMouseButtonCallback(window, mouse_button_callback);
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
        if (animating && stage == 5) {
            auto& br = Bridge::get();
            br.syncIPC();
            if (br.txState == 1) {
                if (destX != br.destX) { truckX = br.truckX; destX = br.destX; }
                truckX += 0.005f;
                if (truckX >= destX) {
                    br.completeDeposit();
                    truckX = destX;
                }
            } else {
                truckX += 0.0032f;
                if (truckX > 1.25f) truckX = -1.65f;
            }
        }
        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteLists(fontBase, 128);
    glfwDestroyWindow(window); glfwTerminate();
    return 0;
}

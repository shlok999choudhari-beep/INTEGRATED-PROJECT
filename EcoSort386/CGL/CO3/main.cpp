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

struct Pt { float x, y; };
const unsigned int SCR_W = 950, SCR_H = 650;
int stage = 5;
bool captureAll = false, autoRotate = true, doClip = true;
float rotX = 22.0f, rotY = 35.0f, rotZ = 0.0f, transX = 0.0f, transY = 0.0f, scaleVal = 1.0f, angle2D = 0.0f, graphT = 0.0f;
GLuint fontBase = 0;

void initFont() {
    HDC hdc = wglGetCurrentDC();
    HFONT f = CreateFontA(14, 0, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE, "Arial");
    SelectObject(hdc, f); fontBase = glGenLists(128); wglUseFontBitmaps(hdc, 0, 128, fontBase);
}
void drawText(float x, float y, const char* s, float r = 0.1f, float g = 0.1f, float b = 0.1f) {
    glColor3f(r, g, b); glRasterPos2f(x, y);
    glPushAttrib(GL_LIST_BIT); glListBase(fontBase); glCallLists((GLsizei)strlen(s), GL_UNSIGNED_BYTE, s); glPopAttrib();
}
void drawRect(float x, float y, float w, float h, float r, float g, float b, float a = 1.0f) { glColor4f(r, g, b, a); glRectf(x, y, x + w, y + h); }
void drawRectOutline(float x, float y, float w, float h, float r, float g, float b, float lw = 1.5f) {
    glLineWidth(lw); glColor3f(r, g, b); glBegin(GL_LINE_LOOP);
    glVertex2f(x, y); glVertex2f(x + w, y); glVertex2f(x + w, y + h); glVertex2f(x, y + h); glEnd();
}
bool saveScreenshot(GLFWwindow* win, const char* name) {
    int w, h; glfwGetFramebufferSize(win, &w, &h); if (w <= 0) w = SCR_W; if (h <= 0) h = SCR_H;
    std::vector<unsigned char> px(w * h * 3); glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, px.data()); stbi_flip_vertically_on_write(1);
    return stbi_write_png(name, w, h, 3, px.data(), w * 3);
}

const int C_IN = 0, C_L = 1, C_R = 2, C_B = 4, C_T = 8;
int getOutCode(float x, float y, float xmin, float ymin, float xmax, float ymax) {
    int c = C_IN;
    if (x < xmin) c |= C_L; else if (x > xmax) c |= C_R;
    if (y < ymin) c |= C_B; else if (y > ymax) c |= C_T;
    return c;
}
bool clipLineCS(float& x1, float& y1, float& x2, float& y2, float xmin, float ymin, float xmax, float ymax) {
    int c1 = getOutCode(x1, y1, xmin, ymin, xmax, ymax), c2 = getOutCode(x2, y2, xmin, ymin, xmax, ymax);
    while (true) {
        if ((c1 | c2) == 0) return true;
        if (c1 & c2) return false;
        float x, y; int cOut = c1 ? c1 : c2;
        if (cOut & C_T) { x = x1 + (x2 - x1) * (ymax - y1) / (y2 - y1); y = ymax; }
        else if (cOut & C_B) { x = x1 + (x2 - x1) * (ymin - y1) / (y2 - y1); y = ymin; }
        else if (cOut & C_R) { y = y1 + (y2 - y1) * (xmax - x1) / (x2 - x1); x = xmax; }
        else { y = y1 + (y2 - y1) * (xmin - x1) / (x2 - x1); x = xmin; }
        if (cOut == c1) { x1 = x; y1 = y; c1 = getOutCode(x1, y1, xmin, ymin, xmax, ymax); }
        else { x2 = x; y2 = y; c2 = getOutCode(x2, y2, xmin, ymin, xmax, ymax); }
    }
}

Pt intersectEdge(Pt p1, Pt p2, int edge, float xmin, float ymin, float xmax, float ymax) {
    Pt o;
    if (edge == 0) { o.x = xmin; o.y = p1.y + (p2.y - p1.y) * (xmin - p1.x) / (p2.x - p1.x); }
    else if (edge == 1) { o.x = xmax; o.y = p1.y + (p2.y - p1.y) * (xmax - p1.x) / (p2.x - p1.x); }
    else if (edge == 2) { o.y = ymin; o.x = p1.x + (p2.x - p1.x) * (ymin - p1.y) / (p2.y - p1.y); }
    else { o.y = ymax; o.x = p1.x + (p2.x - p1.x) * (ymax - p1.y) / (p2.y - p1.y); }
    return o;
}
bool insideEdge(Pt p, int edge, float xmin, float ymin, float xmax, float ymax) {
    return (edge == 0) ? (p.x >= xmin) : (edge == 1 ? (p.x <= xmax) : (edge == 2 ? (p.y >= ymin) : (p.y <= ymax)));
}
std::vector<Pt> clipPolygonSH(const std::vector<Pt>& poly, float xmin, float ymin, float xmax, float ymax) {
    std::vector<Pt> out = poly;
    for (int e = 0; e < 4; e++) {
        if (out.empty()) break;
        std::vector<Pt> in = out; out.clear();
        for (size_t i = 0; i < in.size(); i++) {
            Pt curr = in[i], prev = in[(i + in.size() - 1) % in.size()];
            bool cIn = insideEdge(curr, e, xmin, ymin, xmax, ymax), pIn = insideEdge(prev, e, xmin, ymin, xmax, ymax);
            if (cIn) {
                if (!pIn) out.push_back(intersectEdge(prev, curr, e, xmin, ymin, xmax, ymax));
                out.push_back(curr);
            } else if (pIn) out.push_back(intersectEdge(prev, curr, e, xmin, ymin, xmax, ymax));
        }
    }
    return out;
}

void draw3DChassis(float s) {
    float w = s * 0.40f, h = s * 0.65f, d = s * 0.75f;
    glBegin(GL_QUADS);
    glColor3f(0.20f, 0.22f, 0.26f); glVertex3f(-w,-h, d); glVertex3f( w,-h, d); glVertex3f( w, h, d); glVertex3f(-w, h, d);
    glColor3f(0.12f, 0.14f, 0.16f); glVertex3f(-w,-h,-d); glVertex3f(-w, h,-d); glVertex3f( w, h,-d); glVertex3f( w,-h,-d);
    glColor3f(0.35f, 0.38f, 0.42f); glVertex3f(-w, h,-d); glVertex3f(-w, h, d); glVertex3f( w, h, d); glVertex3f( w, h,-d);
    glColor3f(0.15f, 0.16f, 0.18f); glVertex3f(-w,-h,-d); glVertex3f( w,-h,-d); glVertex3f( w,-h, d); glVertex3f(-w,-h, d);
    glColor3f(0.28f, 0.32f, 0.36f); glVertex3f( w,-h,-d); glVertex3f( w, h,-d); glVertex3f( w, h, d); glVertex3f( w,-h, d);
    glColor3f(0.24f, 0.26f, 0.30f); glVertex3f(-w,-h,-d); glVertex3f(-w,-h, d); glVertex3f(-w, h, d); glVertex3f(-w, h,-d);
    glEnd();
    glLineWidth(2.0f); glColor3f(0.0f, 0.85f, 0.65f); glBegin(GL_LINES);
    glVertex3f(-w,-h, d); glVertex3f( w,-h, d); glVertex3f( w,-h, d); glVertex3f( w, h, d); glVertex3f( w, h, d); glVertex3f(-w, h, d); glVertex3f(-w, h, d); glVertex3f(-w,-h, d);
    glVertex3f(-w,-h,-d); glVertex3f( w,-h,-d); glVertex3f( w,-h,-d); glVertex3f( w, h,-d); glVertex3f( w, h,-d); glVertex3f(-w, h,-d); glVertex3f(-w, h,-d); glVertex3f(-w,-h,-d);
    glVertex3f(-w,-h,-d); glVertex3f(-w,-h, d); glVertex3f( w,-h,-d); glVertex3f( w,-h, d); glVertex3f( w, h,-d); glVertex3f( w, h, d); glVertex3f(-w, h,-d); glVertex3f(-w, h, d);
    glEnd();
}

void stage1() {
    glClearColor(0.96f, 0.97f, 0.98f, 1); glClear(GL_COLOR_BUFFER_BIT);
    drawText(-0.90f, 0.88f, "EcoSort 386 - CGL CO3 | Stage 1: 2D Geometric Transformations (T, R, S)", 0.1f, 0.2f, 0.3f);
    drawText(-0.90f, 0.82f, "Robotic Sorting Arm & PCB Component Geometry Manipulation via Matrix Stack", 0.3f, 0.4f, 0.4f);
    glPushMatrix();
    glTranslatef(transX, transY, 0.0f); glRotatef(angle2D, 0.0f, 0.0f, 1.0f); glScalef(scaleVal, scaleVal, 1.0f);
    drawRect(-0.25f, -0.20f, 0.50f, 0.40f, 0.10f, 0.55f, 0.35f);
    drawRectOutline(-0.25f, -0.20f, 0.50f, 0.40f, 0.8f, 0.9f, 0.2f, 2.5f);
    drawRect(-0.15f, -0.12f, 0.30f, 0.24f, 0.15f, 0.20f, 0.25f);
    drawText(-0.12f, -0.02f, "ARM CPU", 1, 1, 1);
    glLineWidth(2.5f); glColor3f(0.85f, 0.70f, 0.10f);
    glBegin(GL_LINES); for (float p = -0.20f; p <= 0.20f; p += 0.05f) { glVertex2f(p, 0.20f); glVertex2f(p, 0.26f); glVertex2f(p, -0.20f); glVertex2f(p, -0.26f); } glEnd();
    glPopMatrix();
    char b[128]; snprintf(b, sizeof(b), "Tx: %.2f | Ty: %.2f | Angle: %.1f deg | Scale: %.2fx", transX, transY, angle2D, scaleVal);
    drawText(-0.90f, -0.78f, b, 0.15f, 0.25f, 0.4f);
    drawText(-0.90f, -0.86f, "Controls: [Arrows]: Translate | [R]: Rotate | [S/Z]: Scale Up/Down | [SPACE]: Reset", 0.3f, 0.3f, 0.3f);
    drawText(-0.90f, -0.94f, "[Keys 1-5]: Switch Stages | [P]: Save Screenshot | [ESC]: Quit", 0.5f, 0.5f, 0.5f);
}

void stage2() {
    glClearColor(0.95f, 0.96f, 0.98f, 1); glClear(GL_COLOR_BUFFER_BIT);
    drawText(-0.90f, 0.88f, "EcoSort 386 - CGL CO3 | Stage 2: Cohen-Sutherland Line Clipping", 0.1f, 0.2f, 0.3f);
    drawText(-0.90f, 0.82f, "Clipping Municipal E-Waste Logistics Vectors against Sensor Boundary Aperture", 0.3f, 0.4f, 0.4f);
    float xmin = -0.45f, ymin = -0.35f, xmax = 0.45f, ymax = 0.35f;
    drawRect(xmin, ymin, xmax - xmin, ymax - ymin, 0.90f, 0.94f, 0.92f);
    drawRectOutline(xmin, ymin, xmax - xmin, ymax - ymin, 0.1f, 0.5f, 0.7f, 2.5f);
    drawText(xmin + 0.02f, ymax - 0.06f, "CLIPPING WINDOW [Wmin: (-0.45, -0.35), Wmax: (0.45, 0.35)]", 0.1f, 0.4f, 0.6f);
    float routes[5][4] = { { -0.75f, -0.55f, 0.60f, 0.50f }, { -0.60f, 0.20f, 0.20f, -0.55f }, { -0.30f, -0.20f, 0.30f, 0.25f }, { -0.80f, 0.10f, -0.55f, 0.35f }, { 0.10f, 0.55f, 0.55f, 0.15f } };
    for (int i = 0; i < 5; i++) {
        float x1 = routes[i][0], y1 = routes[i][1], x2 = routes[i][2], y2 = routes[i][3];
        glLineWidth(1.5f); glColor3f(0.85f, 0.3f, 0.3f); glBegin(GL_LINES); glVertex2f(x1, y1); glVertex2f(x2, y2); glEnd();
        if (doClip && clipLineCS(x1, y1, x2, y2, xmin, ymin, xmax, ymax)) {
            glLineWidth(3.5f); glColor3f(0.0f, 0.75f, 0.35f); glBegin(GL_LINES); glVertex2f(x1, y1); glVertex2f(x2, y2); glEnd();
            glPointSize(7.0f); glColor3f(0.1f, 0.2f, 0.8f); glBegin(GL_POINTS); glVertex2f(x1, y1); glVertex2f(x2, y2); glEnd();
        }
    }
    drawText(-0.90f, -0.75f, "Outcodes Bitmask: [TOP: 1000, BOTTOM: 0100, RIGHT: 0010, LEFT: 0001, INSIDE: 0000]", 0.2f, 0.3f, 0.4f);
    drawText(-0.90f, -0.83f, "[C]: Toggle Clipping State (ON/OFF) | Green: Accepted Visible | Red: Rejected Line Segment", 0.1f, 0.5f, 0.2f);
    drawText(-0.90f, -0.92f, "[Keys 1-5]: Switch Stages | [P]: Save Screenshot | [ESC]: Quit", 0.5f, 0.5f, 0.5f);
}

void stage3() {
    glClearColor(0.96f, 0.97f, 0.98f, 1); glClear(GL_COLOR_BUFFER_BIT);
    drawText(-0.90f, 0.88f, "EcoSort 386 - CGL CO3 | Stage 3: Sutherland-Hodgman Polygon Clipping", 0.1f, 0.2f, 0.3f);
    drawText(-0.90f, 0.82f, "Conveyor Hopper Aperture Clipping for Irregular Fragmented Circuit Board Polygon", 0.3f, 0.4f, 0.4f);
    float xmin = -0.40f, ymin = -0.35f, xmax = 0.40f, ymax = 0.35f;
    drawRect(xmin, ymin, xmax - xmin, ymax - ymin, 0.88f, 0.92f, 0.96f);
    drawRectOutline(xmin, ymin, xmax - xmin, ymax - ymin, 0.2f, 0.4f, 0.8f, 2.5f);
    std::vector<Pt> origPoly = { { -0.65f, -0.20f }, { -0.25f, 0.55f }, { 0.20f, 0.45f }, { 0.60f, -0.10f }, { 0.35f, -0.55f }, { -0.30f, -0.45f } };
    glLineWidth(1.8f); glColor3f(0.85f, 0.35f, 0.35f); glBegin(GL_LINE_LOOP); for (auto& p : origPoly) glVertex2f(p.x, p.y); glEnd();
    if (doClip) {
        std::vector<Pt> cp = clipPolygonSH(origPoly, xmin, ymin, xmax, ymax);
        if (!cp.empty()) {
            glBegin(GL_POLYGON); glColor3f(0.12f, 0.65f, 0.45f); for (auto& p : cp) glVertex2f(p.x, p.y); glEnd();
            glLineWidth(3.0f); glColor3f(0.95f, 0.95f, 1.0f); glBegin(GL_LINE_LOOP); for (auto& p : cp) glVertex2f(p.x, p.y); glEnd();
        }
    }
    drawText(-0.90f, -0.75f, "Pipeline: Left Clipper -> Right Clipper -> Bottom Clipper -> Top Clipper", 0.2f, 0.3f, 0.4f);
    drawText(-0.90f, -0.83f, "[C]: Toggle Polygon Clipping | Red Outline: Unclipped Geometry | Teal Solid: Clipped Polygon", 0.1f, 0.5f, 0.3f);
    drawText(-0.90f, -0.92f, "[Keys 1-5]: Switch Stages | [P]: Save Screenshot | [ESC]: Quit", 0.5f, 0.5f, 0.5f);
}

void stage4() {
    glClearColor(0.10f, 0.12f, 0.16f, 1); glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity(); gluPerspective(45.0, (double)SCR_W / SCR_H, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); glTranslatef(0.0f, 0.0f, -3.2f);
    glRotatef(rotX, 1.0f, 0.0f, 0.0f); glRotatef(rotY, 0.0f, 1.0f, 0.0f); glRotatef(rotZ, 0.0f, 0.0f, 1.0f);
    draw3DChassis(1.2f);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION); glPopMatrix(); glMatrixMode(GL_MODELVIEW);
    glDisable(GL_DEPTH_TEST);
    drawText(-0.90f, 0.90f, "EcoSort 386 - CGL CO5 | Stage 4: 3D Object Manipulation (E-Waste Inspection)", 0.2f, 0.9f, 0.7f);
    char buf[128]; snprintf(buf, sizeof(buf), "Euler Angles: RotX: %.1f | RotY: %.1f | RotZ: %.1f", rotX, rotY, rotZ);
    drawText(-0.90f, 0.84f, buf, 0.8f, 0.85f, 0.9f);
    drawText(-0.90f, -0.85f, "[Arrow Keys]: Pitch / Yaw | [Q/E]: Roll | [SPACE]: Auto-Rotate Toggle", 0.6f, 0.7f, 0.8f);
    drawText(-0.90f, -0.92f, "[Keys 1-5]: Switch Stages | [P]: Save Screenshot | [ESC]: Quit", 0.5f, 0.5f, 0.5f);
}

void stage5(int w, int h) {
    int v1W = (int)(w * 0.62f);
    glViewport(0, 0, v1W, h);
    glMatrixMode(GL_PROJECTION); glLoadIdentity(); gluPerspective(45.0, (double)v1W / h, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    glClearColor(0.08f, 0.10f, 0.14f, 1); glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glTranslatef(0.0f, 0.05f, -3.0f);
    glRotatef(rotX, 1.0f, 0.0f, 0.0f); glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    draw3DChassis(1.15f);
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION); glLoadIdentity(); gluOrtho2D(-1, 1, -1, 1);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    drawText(-0.95f, 0.92f, "ECOSORT 386 - INTEGRATED 3D INSPECTION CHAMBER (CO3, CO5)", 0.2f, 0.95f, 0.7f);
    drawText(-0.95f, 0.86f, "Viewport 1: Real-time 3D Server Chassis Geometry with Depth Buffer Testing", 0.7f, 0.75f, 0.8f);
    drawText(-0.95f, -0.92f, "[Arrow Keys]: Orbit 3D Asset | [SPACE]: Auto-Turntable | [Keys 1-5]: Stage Select", 0.5f, 0.6f, 0.7f);

    int v2X = v1W, v2W = w - v1W;
    glViewport(v2X, 0, v2W, h);
    glMatrixMode(GL_PROJECTION); glLoadIdentity(); gluOrtho2D(0, 100, 0, 100);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    drawRect(0, 0, 100, 100, 0.12f, 0.15f, 0.20f);
    drawRectOutline(0.5f, 0.5f, 99.0f, 99.0f, 0.0f, 0.75f, 0.55f, 2.0f);
    drawText(5.0f, 93.0f, "TELEMETRY VIEWPORT 2", 0.2f, 0.95f, 0.7f);
    drawText(5.0f, 88.0f, "E-Waste Processing Rate (kg/hr)", 0.8f, 0.85f, 0.9f);
    glLineWidth(2.0f); glColor3f(0.4f, 0.5f, 0.6f); glBegin(GL_LINES);
    glVertex2f(12.0f, 18.0f); glVertex2f(92.0f, 18.0f); glVertex2f(12.0f, 18.0f); glVertex2f(12.0f, 82.0f); glEnd();
    drawText(85.0f, 12.0f, "Time", 0.5f, 0.6f, 0.7f); drawText(5.0f, 82.0f, "Kg/h", 0.5f, 0.6f, 0.7f);

    glLineWidth(3.0f); glColor3f(0.0f, 0.85f, 0.95f); glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 30; i++) {
        float gx = 12.0f + i * (80.0f / 30.0f), gy = 45.0f + 18.0f * sinf(graphT * 2.0f + i * 0.35f) + 8.0f * cosf(graphT * 3.5f + i * 0.2f);
        glVertex2f(gx, gy);
    }
    glEnd();
    glLineWidth(2.0f); glColor3f(1.0f, 0.75f, 0.15f); glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 30; i++) {
        float gx = 12.0f + i * (80.0f / 30.0f), gy = 32.0f + 10.0f * cosf(graphT * 1.5f + i * 0.4f);
        glVertex2f(gx, gy);
    }
    glEnd();
    drawRect(10.0f, 2.0f, 80.0f, 12.0f, 0.08f, 0.10f, 0.14f);
    drawText(13.0f, 8.0f, "Recyclable: 84.2 kg/h | Gold Extracted: 99.4%", 0.1f, 0.9f, 0.4f);
    drawText(13.0f, 4.0f, "Hazard Quarantine: ZERO TOXIC LEAKAGE", 0.95f, 0.8f, 0.2f);
}

void display(GLFWwindow* win) {
    int w, h; glfwGetFramebufferSize(win, &w, &h);
    if (stage == 5) stage5(w, h);
    else {
        glViewport(0, 0, w, h);
        glMatrixMode(GL_PROJECTION); glLoadIdentity(); gluOrtho2D(-1, 1, -1, 1);
        glMatrixMode(GL_MODELVIEW); glLoadIdentity();
        if (stage == 1) stage1(); else if (stage == 2) stage2();
        else if (stage == 3) stage3(); else stage4();
    }
    glFlush();
}
void key_callback(GLFWwindow* win, int key, int scancode, int act, int mods) {
    if (act != GLFW_PRESS && act != GLFW_REPEAT) return;
    if (key >= GLFW_KEY_1 && key <= GLFW_KEY_5) stage = key - GLFW_KEY_0;
    else if (key == GLFW_KEY_SPACE) { autoRotate = !autoRotate; if (stage == 1) { transX = transY = angle2D = 0; scaleVal = 1; } }
    else if (key == GLFW_KEY_C) doClip = !doClip;
    else if (key == GLFW_KEY_R) angle2D += 15.0f;
    else if (key == GLFW_KEY_S) scaleVal = (scaleVal >= 1.8f) ? 0.6f : scaleVal + 0.15f;
    else if (key == GLFW_KEY_Z) scaleVal = (scaleVal <= 0.4f) ? 1.0f : scaleVal - 0.15f;
    else if (key == GLFW_KEY_LEFT) { transX -= 0.05f; rotY -= 6.0f; }
    else if (key == GLFW_KEY_RIGHT) { transX += 0.05f; rotY += 6.0f; }
    else if (key == GLFW_KEY_UP) { transY += 0.05f; rotX -= 6.0f; }
    else if (key == GLFW_KEY_DOWN) { transY -= 0.05f; rotX += 6.0f; }
    else if (key == GLFW_KEY_Q) rotZ += 6.0f;
    else if (key == GLFW_KEY_E) rotZ -= 6.0f;
    else if (key == GLFW_KEY_P) {
        char name[32]; snprintf(name, sizeof(name), "co3_stage%d_output.png", stage); saveScreenshot(win, name);
    }
}
void captureAllEvidence(GLFWwindow* win) {
    const char* f[] = { "transformations_2d.png", "cohen_sutherland_clipping.png", "sutherland_hodgman_clipping.png", "manipulating_3d_objects.png", "dynamic_graph_viewports.png" };
    for (int i = 0; i < 5; i++) { stage = i + 1; display(win); glFinish(); saveScreenshot(win, f[i]); }
}
void init() {
    glClearColor(0.95f, 0.96f, 0.98f, 1);
    glEnable(GL_LINE_SMOOTH); glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

int main(int argc, char** argv) {
    for (int i = 1; i < argc; i++) if (strcmp(argv[i], "--capture-all") == 0) captureAll = true;
    if (!glfwInit()) return -1;
    GLFWwindow* win = glfwCreateWindow(SCR_W, SCR_H, "EcoSort 386 - CGL CO3 & CO5 (Transformations & Clipping)", NULL, NULL);
    if (!win) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(win);
    glfwSetKeyCallback(win, key_callback);
    glfwSwapInterval(1);
    init(); initFont();
    if (captureAll) {
        captureAllEvidence(win);
        glDeleteLists(fontBase, 128); glfwDestroyWindow(win); glfwTerminate(); return 0;
    }
    while (!glfwWindowShouldClose(win)) {
        if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(win, true);
        if (autoRotate && (stage == 4 || stage == 5)) { rotY += 1.2f; rotX = 18.0f + 8.0f * sinf(graphT * 0.8f); }
        graphT += 0.025f;
        display(win);
        glfwSwapBuffers(win);
        glfwPollEvents();
    }
    glDeleteLists(fontBase, 128);
    glfwDestroyWindow(win); glfwTerminate();
    return 0;
}

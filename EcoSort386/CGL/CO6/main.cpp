#include <windows.h>
#include <GLFW/glfw3.h>
#include <GL/glu.h>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <algorithm>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "../../ecosort_bridge.h"

using namespace EcoSortCore;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

inline float clampf(float v, float lo, float hi) { return v < lo ? lo : (v > hi ? hi : v); }
inline int clampi(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }

// ============================================================================
// EcoSort 386 - CGL CO6 Capstone: Texture Mapping & Realistic 3D Lighting
// 3D Municipal E-Waste Collection Truck Unloading & Inspection Station
// Satisfies Course Outcome CO6 (Texture Mapping, RGB Lighting Models, Materials)
// ============================================================================

const unsigned int WIN_W = 960, WIN_H = 680;
const char* WIN_TITLE = "EcoSort 386 - CGL CO6: 3D Texture Mapping & Realistic Lighting";

GLuint fontBase = 0;
bool useTextures = true;
int lightMode = 1; // 1 = Full Facility Studio, 2 = Inspection Spotlight Only, 3 = Ambient Night
bool autoOrbit = false;
bool captureRequested = false;

// Camera state
float camPitch = 24.0f;
float camYaw = 40.0f;
float camDist = 7.8f;
double mouseLastX = 0, mouseLastY = 0;
bool mouseDragging = false;

// Animation state
float truckX = 0.0f;
float bedTilt = 0.0f;        // 0 to 42 degrees
float tailgateAngle = 0.0f;  // 0 to 75 degrees
float cargoSlide = 0.0f;     // Cargo sliding distance down tilted bed
bool isUnloading = false;
bool unloadComplete = false;
float animTimer = 0.0f;

// Telemetry Counters
int batchesUnloaded = 3;
float totalRecycledKg = 124.5f;
int creditsEarned = 220;

// Texture IDs
GLuint texStripes = 0;
GLuint texPCB = 0;
GLuint texHazard = 0;
GLuint texMetal = 0;
GLuint texTread = 0;
GLuint texLogo = 0;

// ----------------------------------------------------------------------------
// Procedural Texture Generators (Zero External File Dependencies)
// ----------------------------------------------------------------------------
GLuint createProceduralTexture(int w, int h, const unsigned char* data) {
    GLuint tid;
    glGenTextures(1, &tid);
    glBindTexture(GL_TEXTURE_2D, tid);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, w, h, GL_RGB, GL_UNSIGNED_BYTE, data);
    return tid;
}

void initTextures() {
    const int S = 128;

    // 1. Hazard Safety Stripes (Yellow & Black diagonal warning bands)
    std::vector<unsigned char> pStripes(S * S * 3);
    for (int y = 0; y < S; y++) {
        for (int x = 0; x < S; x++) {
            int band = (x + y) / 16;
            bool isYellow = (band % 2 == 0);
            int idx = (y * S + x) * 3;
            pStripes[idx + 0] = isYellow ? 245 : 28;
            pStripes[idx + 1] = isYellow ? 195 : 28;
            pStripes[idx + 2] = isYellow ? 20  : 30;
        }
    }
    texStripes = createProceduralTexture(S, S, pStripes.data());

    // 2. PCB Circuit Board (Emerald green substrate, gold pads, copper traces)
    std::vector<unsigned char> pPCB(S * S * 3);
    for (int y = 0; y < S; y++) {
        for (int x = 0; x < S; x++) {
            int idx = (y * S + x) * 3;
            // Base green
            pPCB[idx + 0] = 18;
            pPCB[idx + 1] = 68;
            pPCB[idx + 2] = 34;

            // Copper trace lines
            if ((x % 32 == 16 && (y > 20 && y < 108)) ||
                (y % 32 == 16 && (x > 20 && x < 108)) ||
                (x + y == 64) || (x - y == 32)) {
                pPCB[idx + 0] = 180;
                pPCB[idx + 1] = 135;
                pPCB[idx + 2] = 45;
            }

            // Golden IC solder pads
            int gx = x % 32 - 16, gy = y % 32 - 16;
            if (gx * gx + gy * gy <= 20) {
                pPCB[idx + 0] = 230;
                pPCB[idx + 1] = 195;
                pPCB[idx + 2] = 55;
            }
        }
    }
    texPCB = createProceduralTexture(S, S, pPCB.data());

    // 3. Toxic / Biohazard Warning Symbol (High-contrast circular emblem)
    std::vector<unsigned char> pHazard(S * S * 3);
    for (int y = 0; y < S; y++) {
        for (int x = 0; x < S; x++) {
            int idx = (y * S + x) * 3;
            int dx = x - 64, dy = y - 64;
            int distSq = dx * dx + dy * dy;

            // Bright safety yellow background
            pHazard[idx + 0] = 255;
            pHazard[idx + 1] = 210;
            pHazard[idx + 2] = 0;

            // Outer black ring & inner emblem
            if ((distSq > 48 * 48 && distSq < 56 * 56) || distSq < 12 * 12) {
                pHazard[idx + 0] = 20;
                pHazard[idx + 1] = 20;
                pHazard[idx + 2] = 20;
            }
            // 3 radial nodes at 120-degree intervals
            for (int k = 0; k < 3; k++) {
                float a = k * 2.0944f - 1.5708f;
                int nx = 64 + (int)(28.0f * cosf(a));
                int ny = 64 + (int)(28.0f * sinf(a));
                int ndx = x - nx, ndy = y - ny;
                if (ndx * ndx + ndy * ndy < 13 * 13) {
                    pHazard[idx + 0] = 20;
                    pHazard[idx + 1] = 20;
                    pHazard[idx + 2] = 20;
                }
            }
        }
    }
    texHazard = createProceduralTexture(S, S, pHazard.data());

    // 4. Brushed Stainless Steel Metal Texture
    std::vector<unsigned char> pMetal(S * S * 3);
    for (int y = 0; y < S; y++) {
        for (int x = 0; x < S; x++) {
            int idx = (y * S + x) * 3;
            int grain = (rand() % 24) - 12;
            int base = 190 + grain;
            pMetal[idx + 0] = (unsigned char)clampi(base - 5, 0, 255);
            pMetal[idx + 1] = (unsigned char)clampi(base, 0, 255);
            pMetal[idx + 2] = (unsigned char)clampi(base + 8, 0, 255);
        }
    }
    texMetal = createProceduralTexture(S, S, pMetal.data());

    // 5. Heavy Vehicle Rubber Tire Tread
    std::vector<unsigned char> pTread(64 * 64 * 3);
    for (int y = 0; y < 64; y++) {
        for (int x = 0; x < 64; x++) {
            int idx = (y * 64 + x) * 3;
            bool groove = (x % 16 < 4) || (y % 16 < 3);
            unsigned char c = groove ? 18 : 55;
            pTread[idx + 0] = c;
            pTread[idx + 1] = c;
            pTread[idx + 2] = c + 4;
        }
    }
    texTread = createProceduralTexture(64, 64, pTread.data());

    // 6. EcoSort 386 Municipal Emblem Banner
    std::vector<unsigned char> pLogo(S * S * 3);
    for (int y = 0; y < S; y++) {
        for (int x = 0; x < S; x++) {
            int idx = (y * S + x) * 3;
            pLogo[idx + 0] = 14;
            pLogo[idx + 1] = 32;
            pLogo[idx + 2] = 58;

            int dx = x - 64, dy = y - 64;
            int dSq = dx * dx + dy * dy;
            if (dSq > 36 * 36 && dSq < 48 * 48) {
                pLogo[idx + 0] = 0;
                pLogo[idx + 1] = 200;
                pLogo[idx + 2] = 110;
            }
            if (abs(dx) < 24 && abs(dy) < 6) {
                pLogo[idx + 0] = 240;
                pLogo[idx + 1] = 245;
                pLogo[idx + 2] = 250;
            }
        }
    }
    texLogo = createProceduralTexture(S, S, pLogo.data());
}

// ----------------------------------------------------------------------------
// Font & HUD Rendering
// ----------------------------------------------------------------------------
bool saveScreenshot(GLFWwindow* window, const char* filename) {
    int w, h; glfwGetFramebufferSize(window, &w, &h);
    if (w <= 0) w = WIN_W; if (h <= 0) h = WIN_H;
    std::vector<unsigned char> pixels(w * h * 3);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
    stbi_flip_vertically_on_write(1);
    return stbi_write_png(filename, w, h, 3, pixels.data(), w * 3);
}

void initFont() {
    HDC hdc = wglGetCurrentDC();
    HFONT font = CreateFontA(14, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE, "Arial");
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

// ----------------------------------------------------------------------------
// OpenGL Material Setup
// ----------------------------------------------------------------------------
void setMaterial(float ar, float ag, float ab,
                 float dr, float dg, float db,
                 float sr, float sg, float sb, float shininess) {
    float amb[] = { ar, ag, ab, 1.0f };
    float diff[] = { dr, dg, db, 1.0f };
    float spec[] = { sr, sg, sb, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

// Helper: Textured 3D Box with explicit Normals and Texture Coordinates
void drawTexturedBox(float wx, float wy, float wz, GLuint texId, float repS = 1.0f, float repT = 1.0f) {
    float x = wx * 0.5f, y = wy * 0.5f, z = wz * 0.5f;

    if (useTextures && texId > 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texId);
    } else {
        glDisable(GL_TEXTURE_2D);
    }

    glBegin(GL_QUADS);
    // Front (+Z)
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-x, -y,  z);
    glTexCoord2f(repS, 0.0f); glVertex3f( x, -y,  z);
    glTexCoord2f(repS, repT); glVertex3f( x,  y,  z);
    glTexCoord2f(0.0f, repT); glVertex3f(-x,  y,  z);

    // Back (-Z)
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(repS, 0.0f); glVertex3f(-x, -y, -z);
    glTexCoord2f(repS, repT); glVertex3f(-x,  y, -z);
    glTexCoord2f(0.0f, repT); glVertex3f( x,  y, -z);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( x, -y, -z);

    // Top (+Y)
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, repT); glVertex3f(-x,  y, -z);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-x,  y,  z);
    glTexCoord2f(repS, 0.0f); glVertex3f( x,  y,  z);
    glTexCoord2f(repS, repT); glVertex3f( x,  y, -z);

    // Bottom (-Y)
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-x, -y, -z);
    glTexCoord2f(repS, 0.0f); glVertex3f( x, -y, -z);
    glTexCoord2f(repS, repT); glVertex3f( x, -y,  z);
    glTexCoord2f(0.0f, repT); glVertex3f(-x, -y,  z);

    // Right (+X)
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( x, -y, -z);
    glTexCoord2f(repS, 0.0f); glVertex3f( x,  y, -z);
    glTexCoord2f(repS, repT); glVertex3f( x,  y,  z);
    glTexCoord2f(0.0f, repT); glVertex3f( x, -y,  z);

    // Left (-X)
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(repS, 0.0f); glVertex3f(-x, -y, -z);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-x, -y,  z);
    glTexCoord2f(0.0f, repT); glVertex3f(-x,  y,  z);
    glTexCoord2f(repS, repT); glVertex3f(-x,  y, -z);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

// Draw 3D Wheel with Rubber Tread Texture
void drawTexturedWheel(float radius, float width) {
    int segs = 18;
    float halfW = width * 0.5f;

    if (useTextures) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texTread);
    }

    setMaterial(0.1f, 0.1f, 0.12f, 0.2f, 0.2f, 0.22f, 0.3f, 0.3f, 0.3f, 20.0f);

    // Outer tread cylinder
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segs; i++) {
        float a = i * 2.0f * (float)M_PI / segs;
        float cy = radius * cosf(a);
        float cz = radius * sinf(a);
        glNormal3f(0.0f, cosf(a), sinf(a));
        glTexCoord2f((float)i / segs * 4.0f, 0.0f); glVertex3f(-halfW, cy, cz);
        glTexCoord2f((float)i / segs * 4.0f, 1.0f); glVertex3f( halfW, cy, cz);
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);

    // Steel Metallic Hubcap
    setMaterial(0.2f, 0.22f, 0.25f, 0.7f, 0.75f, 0.8f, 0.9f, 0.9f, 0.95f, 80.0f);
    for (int side = -1; side <= 1; side += 2) {
        glBegin(GL_TRIANGLE_FAN);
        glNormal3f((float)side, 0.0f, 0.0f);
        glVertex3f(side * halfW, 0.0f, 0.0f);
        for (int i = 0; i <= segs; i++) {
            float a = i * 2.0f * (float)M_PI / segs;
            glVertex3f(side * halfW, radius * cosf(a), radius * sinf(a));
        }
        glEnd();
    }
}

// ----------------------------------------------------------------------------
// 3D Scene Geometry: Collection Truck, Unloading Hopper, Facilities
// ----------------------------------------------------------------------------
void drawFacilityDock() {
    // 1. Concrete Unloading Floor
    setMaterial(0.25f, 0.26f, 0.28f, 0.55f, 0.58f, 0.60f, 0.2f, 0.2f, 0.2f, 10.0f);
    glPushMatrix();
    glTranslatef(0.0f, -0.45f, 0.0f);
    drawTexturedBox(12.0f, 0.3f, 14.0f, texMetal, 6.0f, 6.0f);
    glPopMatrix();

    // 2. Safety Hazard Edge Markings
    setMaterial(0.3f, 0.3f, 0.1f, 0.9f, 0.9f, 0.2f, 0.4f, 0.4f, 0.4f, 32.0f);
    glPushMatrix();
    glTranslatef(0.0f, -0.28f, -2.6f);
    drawTexturedBox(5.0f, 0.06f, 0.4f, texStripes, 6.0f, 1.0f);
    glPopMatrix();

    // 3. Intake Hopper Basin (Receiving Pit)
    setMaterial(0.12f, 0.14f, 0.16f, 0.28f, 0.32f, 0.36f, 0.6f, 0.6f, 0.7f, 40.0f);
    glPushMatrix();
    glTranslatef(0.0f, -0.7f, -3.8f);
    drawTexturedBox(3.4f, 0.8f, 2.0f, texMetal, 2.0f, 1.0f);
    glPopMatrix();

    // 4. Facility Logo Backing Wall
    setMaterial(0.18f, 0.20f, 0.24f, 0.35f, 0.40f, 0.45f, 0.2f, 0.2f, 0.2f, 15.0f);
    glPushMatrix();
    glTranslatef(0.0f, 1.8f, -6.5f);
    drawTexturedBox(12.0f, 4.5f, 0.4f, texMetal, 4.0f, 2.0f);
    glPopMatrix();

    // 5. EcoSort 386 Signboard Banner
    setMaterial(0.2f, 0.3f, 0.4f, 0.8f, 0.85f, 0.95f, 0.5f, 0.5f, 0.6f, 32.0f);
    glPushMatrix();
    glTranslatef(0.0f, 3.2f, -6.2f);
    drawTexturedBox(4.5f, 1.2f, 0.1f, texLogo, 1.0f, 1.0f);
    glPopMatrix();
}

void drawCollectionTruck() {
    glPushMatrix();
    glTranslatef(truckX, 0.0f, 0.0f);

    // 1. Heavy Chassis Frame (Dark Steel)
    setMaterial(0.10f, 0.12f, 0.14f, 0.22f, 0.25f, 0.28f, 0.6f, 0.6f, 0.6f, 60.0f);
    glPushMatrix();
    glTranslatef(0.0f, 0.28f, -0.1f);
    drawTexturedBox(1.35f, 0.22f, 4.2f, texMetal, 1.0f, 3.0f);
    glPopMatrix();

    // 2. Driver Cabin (Front: +Z)
    setMaterial(0.05f, 0.25f, 0.15f, 0.10f, 0.65f, 0.35f, 0.8f, 0.9f, 0.85f, 90.0f);
    glPushMatrix();
    glTranslatef(0.0f, 0.95f, 1.45f);
    drawTexturedBox(1.4f, 1.15f, 1.3f, texLogo, 1.0f, 1.0f);
    glPopMatrix();

    // Windshield (Translucent Tinted Specular Glass)
    setMaterial(0.1f, 0.2f, 0.3f, 0.3f, 0.6f, 0.8f, 0.95f, 0.98f, 1.0f, 120.0f);
    glPushMatrix();
    glTranslatef(0.0f, 1.15f, 2.12f);
    drawTexturedBox(1.2f, 0.55f, 0.05f, 0);
    glPopMatrix();

    // Headlights (High-intensity emissive)
    setMaterial(0.8f, 0.8f, 0.6f, 1.0f, 1.0f, 0.8f, 1.0f, 1.0f, 1.0f, 100.0f);
    glPushMatrix();
    glTranslatef(-0.52f, 0.55f, 2.12f); drawTexturedBox(0.22f, 0.18f, 0.05f, 0); glPopMatrix();
    glPushMatrix();
    glTranslatef( 0.52f, 0.55f, 2.12f); drawTexturedBox(0.22f, 0.18f, 0.05f, 0); glPopMatrix();

    // Front Bumper (Hazard safety striped)
    setMaterial(0.3f, 0.3f, 0.1f, 0.8f, 0.8f, 0.2f, 0.4f, 0.4f, 0.4f, 32.0f);
    glPushMatrix();
    glTranslatef(0.0f, 0.22f, 2.16f);
    drawTexturedBox(1.5f, 0.20f, 0.15f, texStripes, 3.0f, 1.0f);
    glPopMatrix();

    // 3. 6 Heavy-Duty Wheels
    float wheelRadius = 0.38f, wheelW = 0.24f;
    float wheelZ[] = { 1.40f, -0.65f, -1.55f };
    for (int i = 0; i < 3; i++) {
        glPushMatrix();
        glTranslatef(-0.80f, 0.0f, wheelZ[i]);
        drawTexturedWheel(wheelRadius, wheelW);
        glPopMatrix();

        glPushMatrix();
        glTranslatef( 0.80f, 0.0f, wheelZ[i]);
        drawTexturedWheel(wheelRadius, wheelW);
        glPopMatrix();
    }

    // 4. Hydraulic Telescoping Cylinder (Extends as bed tilts)
    glPushMatrix();
    glTranslatef(0.0f, 0.45f, 0.2f);
    float pistonLength = 0.5f + (bedTilt / 42.0f) * 0.9f;
    setMaterial(0.3f, 0.3f, 0.35f, 0.8f, 0.8f, 0.85f, 0.95f, 0.95f, 0.95f, 90.0f);
    drawTexturedBox(0.12f, pistonLength, 0.12f, texMetal, 1.0f, 2.0f);
    glPopMatrix();

    // 5. Tiltable Dump Bed (Pivoted at rear hinge: z = -1.9, y = 0.55)
    glPushMatrix();
    glTranslatef(0.0f, 0.55f, -1.90f);
    glRotatef(-bedTilt, 1.0f, 0.0f, 0.0f); // Hydraulic rotation around hinge axis
    glTranslatef(0.0f, 0.45f, 1.35f);

    // Bed Floor (Metallic brushed steel)
    setMaterial(0.2f, 0.22f, 0.25f, 0.75f, 0.78f, 0.82f, 0.85f, 0.85f, 0.90f, 75.0f);
    drawTexturedBox(1.50f, 0.08f, 2.70f, texMetal, 2.0f, 3.0f);

    // Bed Side Walls (Branded with EcoSort Emblem)
    setMaterial(0.08f, 0.25f, 0.18f, 0.15f, 0.65f, 0.40f, 0.7f, 0.7f, 0.7f, 50.0f);
    glPushMatrix(); glTranslatef(-0.73f, 0.45f, 0.0f); drawTexturedBox(0.06f, 0.85f, 2.70f, texLogo, 2.0f, 1.0f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.73f, 0.45f, 0.0f); drawTexturedBox(0.06f, 0.85f, 2.70f, texLogo, 2.0f, 1.0f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.0f,  0.45f, 1.32f); drawTexturedBox(1.46f, 0.85f, 0.06f, texMetal, 1.0f, 1.0f); glPopMatrix();

    // Rear Tailgate (Hinged at top: swings open when bed tilts)
    glPushMatrix();
    glTranslatef(0.0f, 0.88f, -1.35f);
    glRotatef(tailgateAngle, 1.0f, 0.0f, 0.0f);
    glTranslatef(0.0f, -0.42f, 0.0f);
    setMaterial(0.3f, 0.3f, 0.1f, 0.85f, 0.85f, 0.25f, 0.4f, 0.4f, 0.4f, 32.0f);
    drawTexturedBox(1.46f, 0.85f, 0.06f, texStripes, 3.0f, 1.0f);
    glPopMatrix();

    // 6. 3D E-Waste Cargo inside Bed (Slides out into hopper when tilted!)
    glPushMatrix();
    glTranslatef(0.0f, 0.28f, -cargoSlide);

    // Crate 1: Recyclable Server Chassis & PCB Waste (PCB Texture)
    setMaterial(0.1f, 0.3f, 0.15f, 0.2f, 0.8f, 0.4f, 0.6f, 0.8f, 0.6f, 60.0f);
    glPushMatrix();
    glTranslatef(-0.35f, 0.0f, 0.45f);
    drawTexturedBox(0.55f, 0.45f, 0.65f, texPCB, 1.0f, 1.0f);
    glPopMatrix();

    // Crate 2: Hazardous Batteries & Chemical Cells (Toxic Warning Texture)
    setMaterial(0.3f, 0.25f, 0.05f, 0.95f, 0.85f, 0.15f, 0.7f, 0.7f, 0.3f, 50.0f);
    glPushMatrix();
    glTranslatef(0.35f, 0.0f, 0.25f);
    drawTexturedBox(0.50f, 0.40f, 0.50f, texHazard, 1.0f, 1.0f);
    glPopMatrix();

    // Crate 3: Reusable Computer Parts (Brushed Metal Texture)
    setMaterial(0.2f, 0.22f, 0.25f, 0.7f, 0.75f, 0.8f, 0.9f, 0.9f, 0.95f, 85.0f);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -0.55f);
    drawTexturedBox(0.60f, 0.50f, 0.70f, texMetal, 1.0f, 1.0f);
    glPopMatrix();

    glPopMatrix(); // End Cargo Slide

    glPopMatrix(); // End Tiltable Bed

    glPopMatrix(); // End Truck
}

// ----------------------------------------------------------------------------
// Lighting Configuration (Ambient, Diffuse, Specular & Spotlights)
// ----------------------------------------------------------------------------
void setupLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);

    if (lightMode == 1) {
        // Mode 1: Full Facility Studio (Warm Sun + Overhead Floodlights)
        glEnable(GL_LIGHT0);
        float light0Pos[] = { 4.0f, 8.0f, 6.0f, 1.0f };
        float light0Amb[] = { 0.25f, 0.26f, 0.28f, 1.0f };
        float light0Diff[] = { 0.92f, 0.90f, 0.88f, 1.0f };
        float light0Spec[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
        glLightfv(GL_LIGHT0, GL_AMBIENT, light0Amb);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light0Diff);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light0Spec);

        // Inspection Spotlight directly over the Unloading Bay
        glEnable(GL_LIGHT1);
        float light1Pos[] = { 0.0f, 5.5f, -3.5f, 1.0f };
        float light1Dir[] = { 0.0f, -1.0f, 0.0f };
        float light1Diff[] = { 0.85f, 0.90f, 1.0f, 1.0f };
        float light1Spec[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glLightfv(GL_LIGHT1, GL_POSITION, light1Pos);
        glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light1Dir);
        glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 45.0f);
        glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 12.0f);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, light1Diff);
        glLightfv(GL_LIGHT1, GL_SPECULAR, light1Spec);
    } else if (lightMode == 2) {
        // Mode 2: Focused Inspection Spotlight Only
        glDisable(GL_LIGHT0);
        glEnable(GL_LIGHT1);
        float light1Pos[] = { 0.0f, 5.0f, -3.2f, 1.0f };
        float light1Dir[] = { 0.0f, -1.0f, 0.0f };
        float light1Diff[] = { 1.0f, 0.95f, 0.9f, 1.0f };
        float light1Spec[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glLightfv(GL_LIGHT1, GL_POSITION, light1Pos);
        glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light1Dir);
        glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 35.0f);
        glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 18.0f);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, light1Diff);
        glLightfv(GL_LIGHT1, GL_SPECULAR, light1Spec);
    } else {
        // Mode 3: Ambient Night Mode with Truck Headlight Beams
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHT1);
        float ambLight[] = { 0.12f, 0.14f, 0.18f, 1.0f };
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambLight);
    }
}

// ----------------------------------------------------------------------------
// Animation Update Loop
// ----------------------------------------------------------------------------
void updateAnimation(float dt) {
    animTimer += dt;
    if (autoOrbit) camYaw += 18.0f * dt;

    // Check IPC Bridge: auto-trigger dump if a deposit is dispatched from PL/PSOOP or Master Interface
    Bridge& br = Bridge::get();
    if (br.syncIPC()) {
        if (br.txState == 1 && !isUnloading) {
            isUnloading = true;
            unloadComplete = false;
        }
    }

    if (isUnloading) {
        // Phase 1: Bed tilts upward smoothly
        if (bedTilt < 42.0f) {
            bedTilt += 22.0f * dt;
            if (bedTilt > 15.0f && tailgateAngle < 65.0f) {
                tailgateAngle += 45.0f * dt;
            }
        }
        // Phase 2: Cargo slides down bed into intake hopper
        else if (cargoSlide < 2.6f) {
            cargoSlide += 2.2f * dt;
        }
        // Phase 3: Unload completed, log batch
        else if (!unloadComplete) {
            unloadComplete = true;
            batchesUnloaded++;
            totalRecycledKg += 18.5f;
            creditsEarned += 80;

            // Notify IPC Bridge
            Bridge& br = Bridge::get();
            br.syncIPC();
            br.completeDeposit();
        }
    } else {
        // Return bed to resting horizontal position
        if (bedTilt > 0.0f) bedTilt -= 25.0f * dt;
        if (bedTilt < 0.0f) bedTilt = 0.0f;
        if (tailgateAngle > 0.0f) tailgateAngle -= 35.0f * dt;
        if (tailgateAngle < 0.0f) tailgateAngle = 0.0f;
        if (cargoSlide > 0.0f) cargoSlide -= 2.0f * dt;
        if (cargoSlide < 0.0f) cargoSlide = 0.0f;
    }
}

// ----------------------------------------------------------------------------
// 2D Dashboard & Telemetry HUD Overlay
// ----------------------------------------------------------------------------
void renderHUD() {
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, WIN_W, 0, WIN_H);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Semi-transparent top banner
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.08f, 0.10f, 0.14f, 0.85f);
    glRectf(20, WIN_H - 105, WIN_W - 20, WIN_H - 20);

    // Border
    glColor3f(0.0f, 0.80f, 0.45f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(20, WIN_H - 105); glVertex2f(WIN_W - 20, WIN_H - 105);
    glVertex2f(WIN_W - 20, WIN_H - 20); glVertex2f(20, WIN_H - 20);
    glEnd();

    drawText(35, WIN_H - 45, "ECOSORT 386 - 3D COLLECTION TRUCK UNLOADING STATION (CGL CO6)", 0.2f, 0.95f, 0.65f);
    drawText(35, WIN_H - 68, "Curriculum Rubric: Procedural Texture Mapping (PCB/Hazard/Steel) & Multi-Light Specular Shading", 0.75f, 0.80f, 0.85f);

    char statusBuf[160];
    const char* lModeStr = (lightMode == 1 ? "Studio Flood + Spotlight" : (lightMode == 2 ? "Spotlight Only" : "Ambient Night"));
    snprintf(statusBuf, sizeof(statusBuf), "Textures: [%s] | Lighting: [%s] | Bed Tilt: %.1f deg | Batches: %d | Recycled: %.1f kg | Credits: %d",
             (useTextures ? "ENABLED" : "FLAT (OFF)"), lModeStr, bedTilt, batchesUnloaded, totalRecycledKg, creditsEarned);
    drawText(35, WIN_H - 92, statusBuf, 1.0f, 0.85f, 0.25f);

    // Lower Footer Keybinding Guide
    glColor4f(0.08f, 0.10f, 0.14f, 0.85f);
    glRectf(20, 20, WIN_W - 20, 65);
    glColor3f(0.35f, 0.40f, 0.45f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(20, 20); glVertex2f(WIN_W - 20, 20);
    glVertex2f(WIN_W - 20, 65); glVertex2f(20, 65);
    glEnd();

    drawText(35, 45, "[SPACE]: Unload/Reset Truck | [T]: Toggle Textures | [L]: Toggle Lighting | [O]: Auto-Orbit | [P]: Save Screenshot", 0.6f, 0.85f, 1.0f);
    drawText(35, 28, "[Drag Mouse]: Orbit 3D Camera | [Scroll / Keys W-S]: Zoom In/Out | [ESC]: Quit", 0.5f, 0.6f, 0.7f);

    glDisable(GL_BLEND);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_DEPTH_TEST);
}

// ----------------------------------------------------------------------------
// Display Callback
// ----------------------------------------------------------------------------
void display(GLFWwindow* window) {
    glClearColor(0.06f, 0.08f, 0.11f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    if (w <= 0) w = WIN_W;
    if (h <= 0) h = WIN_H;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w / h, 0.1, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Orbital camera calculation
    float radPitch = camPitch * (float)M_PI / 180.0f;
    float radYaw = camYaw * (float)M_PI / 180.0f;
    float eyeX = camDist * cosf(radPitch) * sinf(radYaw);
    float eyeY = camDist * sinf(radPitch);
    float eyeZ = camDist * cosf(radPitch) * cosf(radYaw);

    gluLookAt(eyeX, eyeY + 1.2f, eyeZ, 0.0, 1.2, -0.8, 0.0, 1.0, 0.0);

    setupLighting();

    drawFacilityDock();
    drawCollectionTruck();

    renderHUD();

    if (captureRequested) {
        captureRequested = false;
        saveScreenshot(window, "ecosort_co6_unloading.png");
    }

    glfwSwapBuffers(window);
}

// ----------------------------------------------------------------------------
// Input Callbacks
// ----------------------------------------------------------------------------
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action != GLFW_PRESS && action != GLFW_REPEAT) return;

    if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, true);
    else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        isUnloading = !isUnloading;
        if (!isUnloading) unloadComplete = false;
    }
    else if (key == GLFW_KEY_T && action == GLFW_PRESS) {
        useTextures = !useTextures;
    }
    else if (key == GLFW_KEY_L && action == GLFW_PRESS) {
        lightMode = (lightMode % 3) + 1;
    }
    else if (key == GLFW_KEY_O && action == GLFW_PRESS) {
        autoOrbit = !autoOrbit;
    }
    else if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        captureRequested = true;
    }
    else if (key == GLFW_KEY_W) camDist = clampf(camDist - 0.4f, 3.5f, 16.0f);
    else if (key == GLFW_KEY_S) camDist = clampf(camDist + 0.4f, 3.5f, 16.0f);
    else if (key == GLFW_KEY_LEFT) camYaw -= 4.0f;
    else if (key == GLFW_KEY_RIGHT) camYaw += 4.0f;
    else if (key == GLFW_KEY_UP) camPitch = clampf(camPitch + 3.0f, 5.0f, 85.0f);
    else if (key == GLFW_KEY_DOWN) camPitch = clampf(camPitch - 3.0f, 5.0f, 85.0f);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            mouseDragging = true;
            glfwGetCursorPos(window, &mouseLastX, &mouseLastY);
        } else if (action == GLFW_RELEASE) {
            mouseDragging = false;
        }
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    if (mouseDragging) {
        float dx = (float)(xpos - mouseLastX);
        float dy = (float)(ypos - mouseLastY);
        mouseLastX = xpos;
        mouseLastY = ypos;

        camYaw += dx * 0.45f;
        camPitch += dy * 0.45f;
        if (camPitch > 85.0f) camPitch = 85.0f;
        if (camPitch < 5.0f) camPitch = 5.0f;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camDist -= (float)yoffset * 0.5f;
    if (camDist < 3.5f) camDist = 3.5f;
    if (camDist > 16.0f) camDist = 16.0f;
}

int main(int argc, char** argv) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--capture") == 0) captureRequested = true;
    }

    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(WIN_W, WIN_H, WIN_TITLE, NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSwapInterval(1);

    glEnable(GL_DEPTH_TEST);
    initFont();
    initTextures();
    if (captureRequested) {
        // Set up dramatic mid-unloading pose for evidence screenshot
        bedTilt = 32.0f;
        tailgateAngle = 45.0f;
        cargoSlide = 1.4f;
        display(window);
        glFinish();
        saveScreenshot(window, "ecosort_co6_unloading.png");
        glDeleteLists(fontBase, 128);
        glDeleteTextures(1, &texStripes);
        glDeleteTextures(1, &texPCB);
        glDeleteTextures(1, &texHazard);
        glDeleteTextures(1, &texMetal);
        glDeleteTextures(1, &texTread);
        glDeleteTextures(1, &texLogo);
        glfwDestroyWindow(window);
        glfwTerminate();
        return 0;
    }

    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        double curTime = glfwGetTime();
        float dt = (float)(curTime - lastTime);
        if (dt > 0.1f) dt = 0.1f;
        lastTime = curTime;

        updateAnimation(dt);
        display(window);
        glfwPollEvents();
    }

    glDeleteLists(fontBase, 128);
    glDeleteTextures(1, &texStripes);
    glDeleteTextures(1, &texPCB);
    glDeleteTextures(1, &texHazard);
    glDeleteTextures(1, &texMetal);
    glDeleteTextures(1, &texTread);
    glDeleteTextures(1, &texLogo);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

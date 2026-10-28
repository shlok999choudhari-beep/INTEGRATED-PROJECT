# EcoSort 386 – CGL CO6 Capstone
## Mapping of Texture and Light Color Effects

This module completes **Computer Graphics Laboratory Task 3 / Review 3 (Course Outcome CO6)** for the EcoSort 386 integrated project.

---

## 📌 Academic Mapping & Syllabus Rubrics

| Syllabus Criteria | Rubric Level | Implementation in EcoSort 386 CO6 |
| :--- | :--- | :--- |
| **Texture Mapping (CO6)** | **Level 3 - Excellent (3 Marks)** | Procedural generation of 6 distinct surface textures: Hazard Safety Stripes, Emerald PCB Green substrate with gold solder pads & copper traces, Biohazard warning decal, Brushed stainless steel, Rubber tire tread, and Municipal Facility Logo banner. |
| **RGB Color Models & Lighting (CO6)** | **Level 3 - Excellent (3 Marks)** | Dual-light setup combining `GL_LIGHT0` (ambient 0.25, diffuse 0.92, specular 1.0 warm facility floodlight) and `GL_LIGHT1` (focused 45° cutoff inspection spotlight over the unloading pit). |
| **Material Properties (CO6)** | **Level 3 - Excellent (3 Marks)** | Physical material definitions using `glMaterialfv` with specular highlights and shininess (32 to 120) on truck body, metallic dump bed, windshield glass, and e-waste crates. |
| **3D Object Interaction & Kinematics** | **Level 3 - Excellent (4 Marks)** | 3D collection truck with smooth hydraulic tilting bed (0° to 42°), swinging tailgate (0° to 65°), extending hydraulic piston cylinder, and cargo sliding physics into the receiving hopper. |

---

## 📂 Directory Structure

```text
EcoSort386/CGL/CO6/
├── main.cpp                  # 3D textured, lit truck unloading simulation & HUD
├── build_co6.bat             # Single-click build and evidence generator
├── run_co6.bat               # Single-click interactive launcher
├── ecosort_co6.exe           # Compiled 64-bit OpenGL binary
├── ecosort_co6_unloading.png # Rendered high-resolution evidence screenshot
└── README.md                 # Academic documentation
```

---

## 🎮 Interactive Controls

| Key / Mouse Action | Function |
| :--- | :--- |
| **`[SPACE]`** | **Trigger / Reset Truck Unloading Cycle** (Hydraulic tilt, tailgate open, cargo slide) |
| **`[T]`** | **Toggle Textures ON / OFF** (Switch between textured mode and flat shaded mode for grading demonstration) |
| **`[L]`** | **Cycle Lighting Models** (Full Studio Floodlights / Inspection Spotlight Only / Night Ambient Mode) |
| **`[O]`** | **Toggle Automatic Turntable Orbit** |
| **`[Left Mouse Drag]`** | **Interactive 3D Camera Orbit** (Pitch & Yaw) |
| **`[Mouse Scroll / W / S]`** | **Camera Zoom In / Out** |
| **`[Arrow Keys]`** | **Fine Camera Orientation Adjustments** |
| **`[P]`** | **Save Screenshot** (`ecosort_co6_unloading.png`) |
| **`[ESC]`** | **Exit Application** |

---

## ⚙️ Build and Run Instructions

### To Build and Run:
```cmd
build_co6.bat --run
```
Or simply double-click:
```cmd
run_co6.bat
```

### To Generate Evidence Screenshots:
```cmd
build_co6.bat --capture
```

# Antigravity OpenGL Development Environment & Playground

Welcome to your modern **OpenGL 3.3+ Core Profile** development playground in Antigravity.

This workspace comes fully configured with both **Python** and **C++** modern graphics pipelines, utilizing shared GLSL shaders, camera projection systems, and interactive 3D rendering.

---

## 📁 Project Structure

```text
opengl-playground/
├── .vscode/
│   └── settings.json         # GLSL shader syntax associations & IntelliSense configuration
├── include/
│   ├── glad/
│   │   └── glad.h            # GLAD modern OpenGL 3.3/4.6 function loader header
│   └── KHR/
│       └── khrplatform.h     # Khronos cross-platform definitions
├── python/
│   └── main.py               # Modern OpenGL 3D demo in Python (PyOpenGL + GLFW + NumPy)
├── shaders/
│   ├── vertex.glsl           # Model-View-Projection matrix transformation shader
│   └── fragment.glsl         # Color interpolation fragment shader
├── src/
│   ├── main.cpp              # Modern C++17 OpenGL 3.3 Core demo application
│   └── glad.c                # GLAD OpenGL loader implementation
├── CMakeLists.txt            # CMake build configuration with FetchContent (GLFW, GLM)
├── build_and_run.bat         # Single-click C++ build & launch script
├── run_python.bat            # Single-click Python OpenGL launch script
├── requirements.txt          # Python dependencies
└── README.md                 # This documentation
```

---

# 🐍 1. Python OpenGL (Rapid Prototyping)

### Features
- **Modern OpenGL Core Profile**: Uses VAO, VBO, and custom GLSL vertex/fragment shaders.
- **Interactive Camera & Controls**:
  - **`Left-Click + Drag`**: Orbit and tilt around the 3D cube.
  - **`SPACE`**: Pause or resume auto-rotation.
  - **`ESC`**: Close the window.
  - **Window Resize**: Dynamic viewport and perspective aspect ratio updates.
- **Hardware Diagnostics**: Prints GPU vendor, renderer name, and OpenGL version directly on launch.

### Quick Start
Run from the terminal:
```bash
python python/main.py
```
Or double-click:
```cmd
run_python.bat
```

---

## ⚡ 2. C++ Modern OpenGL (High Performance & LearnOpenGL Standard)

### Features
- **Pure Modern C++17**: Clean architecture matching industry standards and [LearnOpenGL.com](https://learnopengl.com/).
- **Zero Manual Dependency Dragging**: CMake `FetchContent` automatically downloads and configures **GLFW 3.4** and **GLM (OpenGL Mathematics)** directly.
- **Local Pre-configured GLAD**: Includes official Khronos GLAD 3.3 Core loader (`glad.h` and `glad.c`).
- **Post-Build Shader Copy**: Shaders are automatically mirrored to the output build folder.

### Quick Start
To configure, compile, and run:
```cmd
build_and_run.bat
```
Or manually using CMake:
```bash
# 1. Configure CMake
cmake -B build -S .

# 2. Build Release binary
cmake --build build --config Release

# 3. Launch
./build/Release/opengl_demo.exe
```

---

## 🎨 3. Shader Pipeline (`shaders/`)

Both Python and C++ implementations share the exact same GLSL 330 core shaders:

- **`vertex.glsl`**:
  Receives vertex attributes:
  - `location = 0`: Position (`vec3 aPos`)
  - `location = 1`: Color (`vec3 aColor`)
  Applies `gl_Position = projection * view * model * vec4(aPos, 1.0)`.

- **`fragment.glsl`**:
  Interpolates vertex colors and writes final `FragColor`.

Feel free to modify these shaders (add lighting, textures, animations) and restart the demo to see immediate visual results.

---

---

# 🌿 EcoSort 386 — Integrated Multi-Subject Engineering Project

**EcoSort 386** is an integrated Smart Municipal E-Waste Segregation & Recovery System uniting 4 engineering laboratory disciplines:

```text
EcoSort386/
├── CGL/                                 # Computer Graphics Laboratory (OpenGL / GLFW)
│   ├── CO1/                             # Primitives, 2D Kiosk, Truck Animation
│   ├── CO2/                             # Dynamic Conveyor Assembly Line via Bresenham & DDA Algorithms
│   ├── CO3/                             # 2D/3D Transforms, Cohen-Sutherland & Sutherland-Hodgman Clipping, Dual Viewports
│   └── CO6/                             # 3D Collection Truck Unloading, Multi-Light Shading & Procedural Textures
├── COA/                                 # Computer Organization & Architecture
│   ├── ecosort.asm & ecosort_functions.asm # 64-bit x86 NASM Assembly (ADD, SUB, MUL, DIV, CMP, JGE, Decision Classifier)
│   ├── ecosort_coa_driver.cpp           # 64-bit C++ Architecture Verification Driver
│   └── build_coa.bat & run_coa.bat      # Single-click build & launcher
├── PL and PSOOP/                        # Programming Laboratory & PSOOP (Integrated)
│   ├── PL&PSOOPCO1&2.cpp                # Pure Virtual Waste, Dynamic Polymorphism, Linked List, Queue, Stacks, Hash Table
│   ├── build.bat & run.bat              # Single-click build & launcher
│   └── README.md                        # Combined architecture & practical documentation
├── Interface/                           # Master 2D Control Dashboard (Bidirectional IPC Bridge)
└── run_all.bat                          # Integrated multi-process launcher menu
```

### ⚡ Quick-Run All Subjects

| Subject | Directory | Command | What It Runs |
| :--- | :--- | :--- | :--- |
| **CGL CO1** | `EcoSort386/CGL/CO1` | `run_co1.bat` | Interactive 2D Kiosk, moving collection truck, spoke rotation, buttons |
| **CGL CO2** | `EcoSort386/CGL/CO2` | `run_co2.bat` | Dynamic optical sorting assembly line, moving belt, laser scanner, Bresenham & DDA bins |
| **CGL CO3** | `EcoSort386/CGL/CO3` | `run_co3.bat` | 2D/3D Transforms, Line & Polygon clipping, dynamic dual viewports |
| **CGL CO6** | `EcoSort386/CGL/CO6` | `run_co6.bat` | 3D collection truck unloading station with procedural textures and multi-light shading |
| **COA CO2** | `EcoSort386/COA` | `run_coa.bat` | 64-bit assembly arithmetic, register telemetry, priority branching & decision rules |
| **COA CO3** | `EcoSort386/COA/CO3_Algorithm_String` | `run_co3.bat` | Add-and-Shift binary multiplication algorithm & 64-bit assembly string processing |
| **PL & PSOOP** | `EcoSort386/PL and PSOOP` | `run.bat` | Citizen request Linked List, truck dispatch Queue, segregation Stacks, and OOP hierarchy |
| **Interface** | `EcoSort386/Interface` | `run.bat` | Master graphical control panel with live activity logs and synchronized state buttons |
| **Integrated Suite** | `EcoSort386` | `run_all.bat` | Unified multi-window launcher menu for all modules simultaneously |



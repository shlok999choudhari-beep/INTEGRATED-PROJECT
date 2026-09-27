# EcoSort 386 — Computer Graphics Laboratory (CGL)
## Course Outcome 3 & 5 (CO3, CO5) — Transformations, Clipping & 3D Viewports

---

### 📋 Academic Practical Information
- **Course Outcomes**:
  - **CO3**: *Apply the transformation effects on region geometry and apply clipping.* `[Bloom's Level: L3 - Apply]`
  - **CO5**: *Manipulate 3D objects and handle dynamic graph viewports.* `[Bloom's Level: L3 - Apply]`
- **Practical Experiments Covered**:
  1. 2D Geometric Transformations (Translation, Rotation, Scaling via OpenGL Matrix Stack).
  2. Cohen-Sutherland Line Clipping Algorithm against an e-waste containment window.
  3. Sutherland-Hodgman Polygon Clipping Algorithm for conveyor hopper apertures.
  4. 3D Object Manipulation with Depth Buffer Testing (`GL_DEPTH_TEST`).
  5. Dual Dynamic Viewports (`glViewport`) integrating 3D inspection and real-time 2D telemetry curves.
- **Application Context**: **EcoSort 386** — Smart Municipal E-Waste Segregation & Recovery System.

---

## 🎯 Aim & Objectives

1. Implement 2D affine transformations using homogeneous coordinates and OpenGL matrix manipulation routines (`glPushMatrix`, `glPopMatrix`, `glTranslatef`, `glRotatef`, `glScalef`).
2. Implement the **Cohen-Sutherland Line Clipping Algorithm** using 4-bit region outcodes (`TOP=8`, `BOTTOM=4`, `RIGHT=2`, `LEFT=1`) to clip logistics routes and scanner beams against active inspection windows.
3. Implement the **Sutherland-Hodgman Polygon Clipping Algorithm** to clip fragmented polygon geometries against rectangular hopper boundaries.
4. Construct and manipulate 3D geometric models with Euler angle rotations ($X, Y, Z$) and depth testing (`glEnable(GL_DEPTH_TEST)`).
5. Configure multi-viewport rendering using `glViewport` to display concurrent 3D CAD inspection and 2D animated analytical graphs.

---

## 🔬 Mathematical & Algorithmic Foundations

### 1. 2D Transformation Matrices (Homogeneous Coordinates)

Any 2D affine transformation is represented as a $3 \times 3$ matrix operating on homogeneous coordinate vectors $[x, y, 1]^T$:

$$\begin{bmatrix} x' \\ y' \\ 1 \end{bmatrix} = \mathbf{M} \begin{bmatrix} x \\ y \\ 1 \end{bmatrix}$$

- **Translation Matrix**:
  $$\mathbf{T}(t_x, t_y) = \begin{bmatrix} 1 & 0 & t_x \\ 0 & 1 & t_y \\ 0 & 0 & 1 \end{bmatrix}$$

- **Rotation Matrix (Counter-Clockwise by $\theta$)**:
  $$\mathbf{R}(\theta) = \begin{bmatrix} \cos\theta & -\sin\theta & 0 \\ \sin\theta & \cos\theta & 0 \\ 0 & 0 & 1 \end{bmatrix}$$

- **Scaling Matrix**:
  $$\mathbf{S}(s_x, s_y) = \begin{bmatrix} s_x & 0 & 0 \\ 0 & s_y & 0 \\ 0 & 0 & 1 \end{bmatrix}$$

In OpenGL, these transformations post-multiply the current matrix on top of the `GL_MODELVIEW` matrix stack.

---

### 2. Cohen-Sutherland Line Clipping Algorithm

The clipping region is defined by $[x_{min}, x_{max}] \times [y_{min}, y_{max}]$. Every 2D endpoint $(x, y)$ is assigned a 4-bit code:

| Bit Position | Direction | Condition | Code Value |
| :---: | :---: | :---: | :---: |
| Bit 3 (MSB) | **TOP** | $y > y_{max}$ | `1000` (8) |
| Bit 2 | **BOTTOM** | $y < y_{min}$ | `0100` (4) |
| Bit 1 | **RIGHT** | $x > x_{max}$ | `0010` (2) |
| Bit 0 (LSB) | **LEFT** | $x < x_{min}$ | `0001` (1) |
| — | **INSIDE** | Completely within window | `0000` (0) |

#### Decision Rules:
1. **Trivial Accept**: `(code1 | code2) == 0` (Both endpoints are inside the window).
2. **Trivial Reject**: `(code1 & code2) != 0` (Both endpoints lie entirely on the same exterior side of a boundary).
3. **Clipping Iteration**: If neither trivial condition holds, select an exterior endpoint and compute its intersection point with the boundary using line equations:
   $$x = x_1 + (x_2 - x_1) \cdot \frac{y_{clip} - y_1}{y_2 - y_1}, \quad y = y_1 + (y_2 - y_1) \cdot \frac{x_{clip} - x_1}{x_2 - x_1}$$

---

### 3. Sutherland-Hodgman Polygon Clipping

Clips an arbitrary $N$-sided polygon against 4 infinite clipping boundary planes in sequence:
$$\text{Input Polygon} \longrightarrow \text{Clip Left} \longrightarrow \text{Clip Right} \longrightarrow \text{Clip Bottom} \longrightarrow \text{Clip Top} \longrightarrow \text{Clipped Polygon}$$

For each boundary plane and each polygon directed edge from vertex $S$ to vertex $P$:
- Case 1: Both $S$ and $P$ are **INSIDE** $\implies$ Output $P$.
- Case 2: $S$ is **INSIDE** and $P$ is **OUTSIDE** $\implies$ Output intersection point $I$.
- Case 3: Both $S$ and $P$ are **OUTSIDE** $\implies$ Output nothing.
- Case 4: $S$ is **OUTSIDE** and $P$ is **INSIDE** $\implies$ Output intersection $I$ and vertex $P$.

---

### 4. Dynamic Dual Viewports (`glViewport`)

In Stage 5, the single GLFW window is partitioned dynamically:
- **Viewport 1 (Left 62%)**:
  ```cpp
  glViewport(0, 0, width * 0.62, height);
  gluPerspective(45.0, aspect, 0.1, 100.0);
  ```
  Renders the 3D E-Waste Server Chassis with depth buffering and real-time orbit controls.
- **Viewport 2 (Right 38%)**:
  ```cpp
  glViewport(width * 0.62, 0, width * 0.38, height);
  gluOrtho2D(0, 100, 0, 100);
  ```
  Renders dynamic, animated e-waste throughput metrics, coordinate axes, and recovery efficiency curves.

---

## 🎮 Interactive Controls

| Control | Action |
| :--- | :--- |
| `1` | Switch to **Stage 1**: 2D Transformations (Translation, Rotation, Scaling) |
| `2` | Switch to **Stage 2**: Cohen-Sutherland Line Clipping |
| `3` | Switch to **Stage 3**: Sutherland-Hodgman Polygon Clipping |
| `4` | Switch to **Stage 4**: 3D Object Manipulation (E-Waste Chassis) |
| `5` | Switch to **Stage 5**: Dynamic Dual Viewports (3D CAD + 2D Live Graph) |
| `Arrow Keys` | Translate geometry (Stage 1) / Orbit 3D Pitch & Yaw (Stage 4 & 5) |
| `R` | Rotate 2D geometry clockwise (Stage 1) |
| `S` / `Z` | Scale 2D geometry Up / Down (Stage 1) |
| `C` | Toggle Clipping State (ON / OFF) (Stages 2 & 3) |
| `Q` / `E` | Roll 3D object along Z-axis (Stage 4) |
| `SPACE` | Auto-turntable rotation toggle / Reset 2D transform |
| `P` | Save high-resolution PNG screenshot (`co3_stageX_output.png`) |
| `ESC` | Exit application cleanly |

---

## ⚡ Compilation & Execution

From the `EcoSort386/CGL/CO3/` directory:
```cmd
build_co3.bat
run_co3.bat
```
*(Or launch `ecosort_co3.exe` directly)*.

---

## 🧠 Viva Counter-Questions & Model Answers

### Q1: What is the significance of homogeneous coordinates in 2D transformations?
**Answer**: Homogeneous coordinates represent a 2D point as $[x, y, 1]^T$ in a 3D projective space. This allows non-linear operations like translation to be expressed as linear matrix multiplications, enabling composite transformations (translation, rotation, and scaling) to be concatenated into a single compound transformation matrix.

### Q2: Why is Cohen-Sutherland faster than direct parametric line intersection?
**Answer**: Cohen-Sutherland uses 4-bit region outcodes and fast bitwise operations (`AND`, `OR`). Lines completely inside (`code1 | code2 == 0`) or completely outside (`code1 & code2 != 0`) are identified immediately without performing any expensive floating-point division or multiplication.

### Q3: What is the primary limitation of Sutherland-Hodgman Polygon Clipping?
**Answer**: When clipping concave polygons, Sutherland-Hodgman may produce extraneous joining lines connecting disconnected components because it outputs a single vertex list. The Weiler-Atherton algorithm resolves this by supporting arbitrary concave polygons with interior holes.

### Q4: How does `glViewport` differ from `gluOrtho2D` or `gluPerspective`?
**Answer**: `gluOrtho2D` and `gluPerspective` define the **Projection Matrix** (what portion of virtual world space is visible and how it projects onto the normalized clipping volume $[-1, 1]^3$). `glViewport` defines the **Viewport Transformation** (how that normalized volume maps to physical window pixels on screen).

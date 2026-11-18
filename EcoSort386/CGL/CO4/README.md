# EcoSort 386 — Computer Graphics Laboratory (CGL)
## Course Outcome 4 (CO4) — Polygon Filling Algorithms

This module implements and demonstrates **Course Outcome 4 (CO4)**: *Implement scan-line, boundary fill, and flood fill algorithms for polygon rasterization and infill.* `[Bloom's Taxonomy Level: L3 - Apply]`

---

## 📌 Academic Mapping & Syllabus Rubrics

| Syllabus Criteria | Rubric Level | Implementation in EcoSort 386 CO4 |
| :--- | :--- | :--- |
| **Scan-Line Polygon Fill (CO4)** | **Level 3 - Excellent (4 Marks)** | Edge table construction, inverse slope calculations ($\Delta x / \Delta y$), sorting horizontal edge intersections by ascending $X$, and applying the **parity rule** to fill spans on irregular e-waste PCB polygons. |
| **Boundary Fill Algorithm (CO4)** | **Level 3 - Excellent (3 Marks)** | 4-connected recursive/BFS pixel in-filling against closed boundary geometry representing e-waste hazardous battery containers. |
| **Flood Fill Algorithm (CO4)** | **Level 3 - Excellent (3 Marks)** | Connected-component color replacement substituting old interior target colors with active infill indicators. |
| **Municipal Infill Station** | **Level 3 - Excellent (2 Marks)** | Interactive 3-bin municipal infill station demonstrating practical segregation color-coding with live IPC bridge telemetry. |

---

## 🔬 Mathematical & Algorithmic Foundations

### 1. Scan-Line Polygon Fill Algorithm
For each scanline $y$ from $y_{max}$ to $y_{min}$:
1. **Edge Intersections**: Find intersection point $x$ for all non-horizontal edges $(P_1, P_2)$ where $y \in [\min(y_1, y_2), \max(y_1, y_2))$:
   $$x = x_1 + \frac{y - y_1}{y_2 - y_1}(x_2 - x_1)$$
2. **Sort Intersections**: Sort all computed intersection points in ascending order:
   $$x_1 \le x_2 \le x_3 \le x_4 \dots$$
3. **Parity Span Drawing**: Fill discrete horizontal spans between consecutive pairs: $[x_1, x_2], [x_3, x_4], \dots$ using `GL_LINES`.

---

### 2. Boundary Fill Algorithm (4-Connected)
Starting from a seed point $(x, y)$, if current pixel color is neither the `boundaryColor` nor the `fillColor`:
1. Color the current pixel with `fillColor`.
2. Recursively expand to all 4 orthogonal neighbors:
   $$\text{BoundaryFill4}(x + 1, y), \quad \text{BoundaryFill4}(x - 1, y), \quad \text{BoundaryFill4}(x, y + 1), \quad \text{BoundaryFill4}(x, y - 1)$$

---

### 3. Flood Fill Algorithm
Starting from a seed point $(x, y)$, if current pixel color equals the `oldColor`:
1. Color current pixel with `newColor`.
2. Recursively expand to neighboring pixels:
   $$\text{FloodFill4}(x \pm 1, y), \quad \text{FloodFill4}(x, y \pm 1)$$

---

## 🎮 Interactive Controls

| Key / Mouse Action | Function |
| :--- | :--- |
| **`[1]`** | Switch to **Stage 1: Scan-Line Polygon Fill Demo** |
| **`[2]`** | Switch to **Stage 2: Boundary Fill (4-Way) Demo** |
| **`[3]`** | Switch to **Stage 3: Flood Fill Demo** |
| **`[4]`** | Switch to **Stage 4: Interactive Municipal Infill Station** |
| **`[SPACE]`** | Step / Re-trigger active filling animation sweep |
| **`[R]`** | Reset all infill buffers |
| **`[Left Click]`** | Set custom seed point inside boundary grid |
| **`[P] / [S]`** | Capture high-resolution screenshot (`ecosort_co4_filling.png`) |
| **`[ESC]`** | Quit application |

---

## ⚙️ Build and Run Instructions

```cmd
:: Build and launch
run_co4.bat

:: Headless evidence screenshot generation
build_co4.bat --capture
```

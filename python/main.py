"""
Modern OpenGL (3.3+ Core Profile) 3D Demo in Python
Uses PyOpenGL, GLFW, and native ctypes (zero native binary DLL dependencies).
"""

import sys
import os
import math
import ctypes
import glfw
from OpenGL.GL import *

# Window dimensions
WINDOW_WIDTH = 1000
WINDOW_HEIGHT = 700
WINDOW_TITLE = "Antigravity OpenGL - 3D Interactive Demo"

# Interaction state
is_paused = False
last_x, last_y = WINDOW_WIDTH / 2.0, WINDOW_HEIGHT / 2.0
mouse_pressed = False
yaw, pitch = 0.0, 0.0

def create_shader_program(vert_path, frag_path):
    """Loads, compiles, and links vertex and fragment shaders."""
    with open(vert_path, 'r') as f:
        vert_src = f.read()
    with open(frag_path, 'r') as f:
        frag_src = f.read()

    # Compile Vertex Shader
    vert_shader = glCreateShader(GL_VERTEX_SHADER)
    glShaderSource(vert_shader, vert_src)
    glCompileShader(vert_shader)
    if not glGetShaderiv(vert_shader, GL_COMPILE_STATUS):
        info = glGetShaderInfoLog(vert_shader).decode('utf-8')
        raise RuntimeError(f"Vertex Shader Compilation Failed:\n{info}")

    # Compile Fragment Shader
    frag_shader = glCreateShader(GL_FRAGMENT_SHADER)
    glShaderSource(frag_shader, frag_src)
    glCompileShader(frag_shader)
    if not glGetShaderiv(frag_shader, GL_COMPILE_STATUS):
        info = glGetShaderInfoLog(frag_shader).decode('utf-8')
        raise RuntimeError(f"Fragment Shader Compilation Failed:\n{info}")

    # Link Program
    program = glCreateProgram()
    glAttachShader(program, vert_shader)
    glAttachShader(program, frag_shader)
    glLinkProgram(program)
    if not glGetProgramiv(program, GL_LINK_STATUS):
        info = glGetProgramInfoLog(program).decode('utf-8')
        raise RuntimeError(f"Shader Program Linking Failed:\n{info}")

    glDeleteShader(vert_shader)
    glDeleteShader(frag_shader)
    return program

def get_cube_vertices():
    """Returns 36 vertices (positions x,y,z and colors r,g,b) for a colored 3D cube."""
    vertices = [
        # Front face (Vibrant Red)
        -0.5, -0.5,  0.5,   0.95, 0.25, 0.35,
         0.5, -0.5,  0.5,   0.95, 0.25, 0.35,
         0.5,  0.5,  0.5,   0.95, 0.25, 0.35,
         0.5,  0.5,  0.5,   0.95, 0.25, 0.35,
        -0.5,  0.5,  0.5,   0.95, 0.25, 0.35,
        -0.5, -0.5,  0.5,   0.95, 0.25, 0.35,

        # Back face (Vibrant Cyan)
        -0.5, -0.5, -0.5,   0.15, 0.85, 0.85,
         0.5, -0.5, -0.5,   0.15, 0.85, 0.85,
         0.5,  0.5, -0.5,   0.15, 0.85, 0.85,
         0.5,  0.5, -0.5,   0.15, 0.85, 0.85,
        -0.5,  0.5, -0.5,   0.15, 0.85, 0.85,
        -0.5, -0.5, -0.5,   0.15, 0.85, 0.85,

        # Left face (Emerald Green)
        -0.5,  0.5,  0.5,   0.20, 0.85, 0.45,
        -0.5,  0.5, -0.5,   0.20, 0.85, 0.45,
        -0.5, -0.5, -0.5,   0.20, 0.85, 0.45,
        -0.5, -0.5, -0.5,   0.20, 0.85, 0.45,
        -0.5, -0.5,  0.5,   0.20, 0.85, 0.45,
        -0.5,  0.5,  0.5,   0.20, 0.85, 0.45,

        # Right face (Electric Purple)
         0.5,  0.5,  0.5,   0.65, 0.35, 0.95,
         0.5,  0.5, -0.5,   0.65, 0.35, 0.95,
         0.5, -0.5, -0.5,   0.65, 0.35, 0.95,
         0.5, -0.5, -0.5,   0.65, 0.35, 0.95,
         0.5, -0.5,  0.5,   0.65, 0.35, 0.95,
         0.5,  0.5,  0.5,   0.65, 0.35, 0.95,

        # Bottom face (Amber Gold)
        -0.5, -0.5, -0.5,   0.95, 0.75, 0.15,
         0.5, -0.5, -0.5,   0.95, 0.75, 0.15,
         0.5, -0.5,  0.5,   0.95, 0.75, 0.15,
         0.5, -0.5,  0.5,   0.95, 0.75, 0.15,
        -0.5, -0.5,  0.5,   0.95, 0.75, 0.15,
        -0.5, -0.5, -0.5,   0.95, 0.75, 0.15,

        # Top face (Royal Blue)
        -0.5,  0.5, -0.5,   0.25, 0.55, 0.95,
         0.5,  0.5, -0.5,   0.25, 0.55, 0.95,
         0.5,  0.5,  0.5,   0.25, 0.55, 0.95,
         0.5,  0.5,  0.5,   0.25, 0.55, 0.95,
        -0.5,  0.5,  0.5,   0.25, 0.55, 0.95,
        -0.5,  0.5, -0.5,   0.25, 0.55, 0.95
    ]
    return vertices

# Pure Python 4x4 column-major matrix utilities
def perspective_matrix(fov_deg, aspect, near, far):
    f = 1.0 / math.tan(math.radians(fov_deg) / 2.0)
    return [
        f / aspect, 0.0, 0.0, 0.0,
        0.0, f, 0.0, 0.0,
        0.0, 0.0, (far + near) / (near - far), -1.0,
        0.0, 0.0, (2.0 * far * near) / (near - far), 0.0
    ]

def look_at(eye, target, up):
    # Forward vector
    fx = target[0] - eye[0]
    fy = target[1] - eye[1]
    fz = target[2] - eye[2]
    flen = math.sqrt(fx*fx + fy*fy + fz*fz)
    fx, fy, fz = fx/flen, fy/flen, fz/flen

    # Side vector (cross product f x up)
    sx = fy * up[2] - fz * up[1]
    sy = fz * up[0] - fx * up[2]
    sz = fx * up[1] - fy * up[0]
    slen = math.sqrt(sx*sx + sy*sy + sz*sz)
    sx, sy, sz = sx/slen, sy/slen, sz/slen

    # Up vector (cross product s x f)
    ux = sy * fz - sz * fy
    uy = sz * fx - sx * fz
    uz = sx * fy - sy * fx

    # Dot products for translation
    tx = -(sx * eye[0] + sy * eye[1] + sz * eye[2])
    ty = -(ux * eye[0] + uy * eye[1] + uz * eye[2])
    tz =  (fx * eye[0] + fy * eye[1] + fz * eye[2])

    return [
         sx,  ux, -fx, 0.0,
         sy,  uy, -fy, 0.0,
         sz,  uz, -fz, 0.0,
         tx,  ty,  tz, 1.0
    ]

def rotate_y(rad):
    c, s = math.cos(rad), math.sin(rad)
    return [
         c,   0.0, -s,  0.0,
        0.0,  1.0, 0.0, 0.0,
         s,   0.0,  c,  0.0,
        0.0,  0.0, 0.0, 1.0
    ]

def rotate_x(rad):
    c, s = math.cos(rad), math.sin(rad)
    return [
        1.0, 0.0,  0.0, 0.0,
        0.0,  c,   s,   0.0,
        0.0, -s,   c,   0.0,
        0.0, 0.0,  0.0, 1.0
    ]

def mat4_mul(a, b):
    # Column-major 4x4 matrix multiplication
    res = [0.0] * 16
    for col in range(4):
        for row in range(4):
            val = 0.0
            for k in range(4):
                val += a[k * 4 + row] * b[col * 4 + k]
            res[col * 4 + row] = val
    return res

# Callbacks
def framebuffer_size_callback(window, width, height):
    if height == 0:
        height = 1
    glViewport(0, 0, width, height)

def key_callback(window, key, scancode, action, mods):
    global is_paused
    if action == glfw.PRESS:
        if key == glfw.KEY_ESCAPE:
            glfw.set_window_should_close(window, True)
        elif key == glfw.KEY_SPACE:
            is_paused = not is_paused

def mouse_button_callback(window, button, action, mods):
    global mouse_pressed, last_x, last_y
    if button == glfw.MOUSE_BUTTON_LEFT:
        if action == glfw.PRESS:
            mouse_pressed = True
            pos = glfw.get_cursor_pos(window)
            last_x, last_y = pos[0], pos[1]
        elif action == glfw.RELEASE:
            mouse_pressed = False

def cursor_pos_callback(window, xpos, ypos):
    global mouse_pressed, last_x, last_y, yaw, pitch
    if mouse_pressed:
        dx = xpos - last_x
        dy = ypos - last_y
        last_x = xpos
        last_y = ypos
        yaw += dx * 0.01
        pitch += dy * 0.01

def main():
    if not glfw.init():
        print("[ERROR] Failed to initialize GLFW", file=sys.stderr)
        return -1

    # Request OpenGL 3.3 Core Profile
    glfw.window_hint(glfw.CONTEXT_VERSION_MAJOR, 3)
    glfw.window_hint(glfw.CONTEXT_VERSION_MINOR, 3)
    glfw.window_hint(glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE)
    glfw.window_hint(glfw.OPENGL_FORWARD_COMPAT, GL_TRUE)
    glfw.window_hint(glfw.SAMPLES, 4)

    window = glfw.create_window(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, None, None)
    if not window:
        print("[ERROR] Failed to create GLFW window", file=sys.stderr)
        glfw.terminate()
        return -1

    glfw.make_context_current(window)
    glfw.set_framebuffer_size_callback(window, framebuffer_size_callback)
    glfw.set_key_callback(window, key_callback)
    glfw.set_mouse_button_callback(window, mouse_button_callback)
    glfw.set_cursor_pos_callback(window, cursor_pos_callback)
    glfw.swap_interval(1)

    # Print GPU & OpenGL diagnostics
    print("==================================================", flush=True)
    print(" OpenGL Python Context Initialized Successfully", flush=True)
    print("==================================================", flush=True)
    print(f" Vendor:   {glGetString(GL_VENDOR).decode('utf-8')}", flush=True)
    print(f" Renderer: {glGetString(GL_RENDERER).decode('utf-8')}", flush=True)
    print(f" Version:  {glGetString(GL_VERSION).decode('utf-8')}", flush=True)
    print(f" GLSL:     {glGetString(GL_SHADING_LANGUAGE_VERSION).decode('utf-8')}", flush=True)
    print("==================================================", flush=True)
    print(" Controls:", flush=True)
    print("   [ESC]       : Quit demo", flush=True)
    print("   [SPACE]     : Pause / Resume rotation", flush=True)
    print("   [Left-Drag] : Orbit camera manually with mouse", flush=True)
    print("==================================================", flush=True)

    # OpenGL state
    glEnable(GL_DEPTH_TEST)
    glEnable(GL_MULTISAMPLE)
    glClearColor(0.08, 0.09, 0.12, 1.0)

    # Shaders
    base_dir = os.path.dirname(os.path.abspath(__file__))
    shaders_dir = os.path.join(base_dir, "..", "shaders")
    vert_path = os.path.join(shaders_dir, "vertex.glsl")
    frag_path = os.path.join(shaders_dir, "fragment.glsl")
    shader_program = create_shader_program(vert_path, frag_path)

    # Cube Geometry Setup
    raw_vertices = get_cube_vertices()
    vertex_array = (ctypes.c_float * len(raw_vertices))(*raw_vertices)
    vertex_buffer_size = ctypes.sizeof(vertex_array)

    vao = glGenVertexArrays(1)
    vbo = glGenBuffers(1)

    glBindVertexArray(vao)
    glBindBuffer(GL_ARRAY_BUFFER, vbo)
    glBufferData(GL_ARRAY_BUFFER, vertex_buffer_size, vertex_array, GL_STATIC_DRAW)

    stride = 6 * ctypes.sizeof(ctypes.c_float)
    # Position attribute (layout location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, ctypes.c_void_p(0))
    glEnableVertexAttribArray(0)

    # Color attribute (layout location = 1)
    color_offset = ctypes.c_void_p(3 * ctypes.sizeof(ctypes.c_float))
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, color_offset)
    glEnableVertexAttribArray(1)

    # Uniform locations
    glUseProgram(shader_program)
    model_loc = glGetUniformLocation(shader_program, "model")
    view_loc = glGetUniformLocation(shader_program, "view")
    proj_loc = glGetUniformLocation(shader_program, "projection")

    # Camera & Projection
    eye = (0.0, 1.5, 3.5)
    target = (0.0, 0.0, 0.0)
    up = (0.0, 1.0, 0.0)
    view = look_at(eye, target, up)
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, (ctypes.c_float * 16)(*view))

    auto_rot_angle = 0.0
    last_time = glfw.get_time()

    # Render Loop
    while not glfw.window_should_close(window):
        current_time = glfw.get_time()
        delta_time = current_time - last_time
        last_time = current_time

        if not is_paused:
            auto_rot_angle += delta_time * 0.8

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)

        # Dynamic perspective projection based on aspect ratio
        width, height = glfw.get_framebuffer_size(window)
        aspect = width / height if height > 0 else 1.0
        proj = perspective_matrix(45.0, aspect, 0.1, 100.0)

        # Rotation matrices
        rot_y = rotate_y(auto_rot_angle + yaw)
        rot_x = rotate_x(pitch + math.sin(auto_rot_angle * 0.5) * 0.3)
        model = mat4_mul(rot_y, rot_x)

        glUseProgram(shader_program)
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, (ctypes.c_float * 16)(*model))
        glUniformMatrix4fv(proj_loc, 1, GL_FALSE, (ctypes.c_float * 16)(*proj))

        glBindVertexArray(vao)
        glDrawArrays(GL_TRIANGLES, 0, 36)

        glfw.swap_buffers(window)
        glfw.poll_events()

    # Clean up
    glDeleteVertexArrays(1, [vao])
    glDeleteBuffers(1, [vbo])
    glDeleteProgram(shader_program)
    glfw.terminate()
    return 0

if __name__ == "__main__":
    sys.exit(main())

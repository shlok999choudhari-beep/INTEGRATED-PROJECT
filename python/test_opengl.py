"""
Automated headless test for OpenGL environment verification.
Runs with hidden window, tests shader compilation, VAO/VBO creation, and frame rendering.
"""

import sys
import os
import ctypes
import glfw
from OpenGL.GL import *

def main():
    if not glfw.init():
        print("[FAIL] Could not initialize GLFW")
        sys.exit(1)

    # Invisible window for automated testing
    glfw.window_hint(glfw.VISIBLE, glfw.FALSE)
    glfw.window_hint(glfw.CONTEXT_VERSION_MAJOR, 3)
    glfw.window_hint(glfw.CONTEXT_VERSION_MINOR, 3)
    glfw.window_hint(glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE)

    window = glfw.create_window(640, 480, "HeadlessTest", None, None)
    if not window:
        print("[FAIL] Could not create GLFW context/window")
        glfw.terminate()
        sys.exit(1)

    glfw.make_context_current(window)

    vendor = glGetString(GL_VENDOR).decode('utf-8')
    renderer = glGetString(GL_RENDERER).decode('utf-8')
    version = glGetString(GL_VERSION).decode('utf-8')
    glsl = glGetString(GL_SHADING_LANGUAGE_VERSION).decode('utf-8')

    print("==================================================")
    print(" AUTOMATED OPENGL VERIFICATION REPORT")
    print("==================================================")
    print(f" GPU Vendor:          {vendor}")
    print(f" Renderer:            {renderer}")
    print(f" OpenGL Version:      {version}")
    print(f" GLSL Version:        {glsl}")

    # Load and compile shaders
    base_dir = os.path.dirname(os.path.abspath(__file__))
    shaders_dir = os.path.join(base_dir, "..", "shaders")

    with open(os.path.join(shaders_dir, "vertex.glsl"), "r") as f:
        v_src = f.read()
    with open(os.path.join(shaders_dir, "fragment.glsl"), "r") as f:
        f_src = f.read()

    vs = glCreateShader(GL_VERTEX_SHADER)
    glShaderSource(vs, v_src)
    glCompileShader(vs)
    if not glGetShaderiv(vs, GL_COMPILE_STATUS):
        print("[FAIL] Vertex shader compilation failed")
        sys.exit(1)

    fs = glCreateShader(GL_FRAGMENT_SHADER)
    glShaderSource(fs, f_src)
    glCompileShader(fs)
    if not glGetShaderiv(fs, GL_COMPILE_STATUS):
        print("[FAIL] Fragment shader compilation failed")
        sys.exit(1)

    prog = glCreateProgram()
    glAttachShader(prog, vs)
    glAttachShader(prog, fs)
    glLinkProgram(prog)
    if not glGetProgramiv(prog, GL_LINK_STATUS):
        print("[FAIL] Shader program linking failed")
        sys.exit(1)

    # Test VAO & VBO
    vao = glGenVertexArrays(1)
    vbo = glGenBuffers(1)

    test_data = [0.0, 0.5, 0.0, 1.0, 0.0, 0.0,
                 -0.5, -0.5, 0.0, 0.0, 1.0, 0.0,
                  0.5, -0.5, 0.0, 0.0, 0.0, 1.0]
    c_arr = (ctypes.c_float * len(test_data))(*test_data)

    glBindVertexArray(vao)
    glBindBuffer(GL_ARRAY_BUFFER, vbo)
    glBufferData(GL_ARRAY_BUFFER, ctypes.sizeof(c_arr), c_arr, GL_STATIC_DRAW)

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * 4, ctypes.c_void_p(0))
    glEnableVertexAttribArray(0)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * 4, ctypes.c_void_p(12))
    glEnableVertexAttribArray(1)

    # Render test frame
    glClearColor(0.1, 0.2, 0.3, 1.0)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    glUseProgram(prog)
    glDrawArrays(GL_TRIANGLES, 0, 3)

    err = glGetError()
    if err != GL_NO_ERROR:
        print(f"[FAIL] glGetError returned {err}")
        sys.exit(1)

    glfw.swap_buffers(window)

    print(" Shaders & Pipeline:  PASSED")
    print(" Buffer Operations:   PASSED")
    print(" Frame Draw:          PASSED")
    print(" Status:              ALL TESTS PASSED")
    print("==================================================")

    glDeleteVertexArrays(1, [vao])
    glDeleteBuffers(1, [vbo])
    glDeleteProgram(prog)
    glfw.terminate()
    return 0

if __name__ == "__main__":
    sys.exit(main())

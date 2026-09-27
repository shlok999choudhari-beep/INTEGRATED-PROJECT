#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

// Window configuration
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 700;
const char* SCR_TITLE = "Antigravity OpenGL - C++ 3D Interactive Demo";

// Interaction State
bool isPaused = false;
bool mousePressed = false;
double lastX = SCR_WIDTH / 2.0;
double lastY = SCR_HEIGHT / 2.0;
float yawAngle = 0.0f;
float pitchAngle = 0.0f;

// Callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height > 0 ? height : 1);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            mousePressed = true;
            glfwGetCursorPos(window, &lastX, &lastY);
        } else if (action == GLFW_RELEASE) {
            mousePressed = false;
        }
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    if (mousePressed) {
        float xoffset = static_cast<float>(xpos - lastX);
        float yoffset = static_cast<float>(ypos - lastY);
        lastX = xpos;
        lastY = ypos;

        yawAngle += xoffset * 0.01f;
        pitchAngle += yoffset * 0.01f;
    }
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        isPaused = !isPaused;
    }
}

// Utility: Read shader source from file
std::string readFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "[ERROR] Failed to open shader file: " << filePath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Utility: Compile single shader
unsigned int compileShader(unsigned int type, const std::string& source, const std::string& typeName) {
    unsigned int shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "[ERROR] " << typeName << " compilation failed:\n" << infoLog << std::endl;
    }
    return shader;
}

// Utility: Build full shader program
unsigned int createShaderProgram(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vertexCode = readFile(vertexPath);
    std::string fragmentCode = readFile(fragmentPath);

    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexCode, "VERTEX");
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentCode, "FRAGMENT");

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "[ERROR] Shader Program linking failed:\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

int main() {
    // 1. Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "[ERROR] Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x MSAA

    // 2. Create Window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, SCR_TITLE, nullptr, nullptr);
    if (!window) {
        std::cerr << "[ERROR] Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // 3. Register Callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSwapInterval(1); // Enable V-Sync

    // 4. Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "[ERROR] Failed to initialize GLAD loader" << std::endl;
        return -1;
    }

    // 5. Diagnostics
    std::cout << "==================================================" << std::endl;
    std::cout << " OpenGL C++ Context Initialized Successfully" << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << " Vendor:   " << glGetString(GL_VENDOR) << std::endl;
    std::cout << " Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << " Version:  " << glGetString(GL_VERSION) << std::endl;
    std::cout << " GLSL:     " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << " Controls:" << std::endl;
    std::cout << "   [ESC]       : Quit demo" << std::endl;
    std::cout << "   [SPACE]     : Pause / Resume rotation" << std::endl;
    std::cout << "   [Left-Drag] : Orbit camera manually with mouse" << std::endl;
    std::cout << "==================================================" << std::endl;

    // 6. Global OpenGL Settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glClearColor(0.08f, 0.09f, 0.12f, 1.0f);

    // 7. Shaders
    unsigned int shaderProgram = createShaderProgram("shaders/vertex.glsl", "shaders/fragment.glsl");

    // 8. Geometry: 36 vertices with positions and colors for 3D Cube
    float vertices[] = {
        // Front face (Reddish)
        -0.5f, -0.5f,  0.5f,   0.9f, 0.2f, 0.3f,
         0.5f, -0.5f,  0.5f,   0.9f, 0.2f, 0.3f,
         0.5f,  0.5f,  0.5f,   0.9f, 0.2f, 0.3f,
         0.5f,  0.5f,  0.5f,   0.9f, 0.2f, 0.3f,
        -0.5f,  0.5f,  0.5f,   0.9f, 0.2f, 0.3f,
        -0.5f, -0.5f,  0.5f,   0.9f, 0.2f, 0.3f,

        // Back face (Cyan)
        -0.5f, -0.5f, -0.5f,   0.1f, 0.8f, 0.8f,
         0.5f, -0.5f, -0.5f,   0.1f, 0.8f, 0.8f,
         0.5f,  0.5f, -0.5f,   0.1f, 0.8f, 0.8f,
         0.5f,  0.5f, -0.5f,   0.1f, 0.8f, 0.8f,
        -0.5f,  0.5f, -0.5f,   0.1f, 0.8f, 0.8f,
        -0.5f, -0.5f, -0.5f,   0.1f, 0.8f, 0.8f,

        // Left face (Greenish)
        -0.5f,  0.5f,  0.5f,   0.2f, 0.8f, 0.4f,
        -0.5f,  0.5f, -0.5f,   0.2f, 0.8f, 0.4f,
        -0.5f, -0.5f, -0.5f,   0.2f, 0.8f, 0.4f,
        -0.5f, -0.5f, -0.5f,   0.2f, 0.8f, 0.4f,
        -0.5f, -0.5f,  0.5f,   0.2f, 0.8f, 0.4f,
        -0.5f,  0.5f,  0.5f,   0.2f, 0.8f, 0.4f,

        // Right face (Purple/Violet)
         0.5f,  0.5f,  0.5f,   0.6f, 0.3f, 0.9f,
         0.5f,  0.5f, -0.5f,   0.6f, 0.3f, 0.9f,
         0.5f, -0.5f, -0.5f,   0.6f, 0.3f, 0.9f,
         0.5f, -0.5f, -0.5f,   0.6f, 0.3f, 0.9f,
         0.5f, -0.5f,  0.5f,   0.6f, 0.3f, 0.9f,
         0.5f,  0.5f,  0.5f,   0.6f, 0.3f, 0.9f,

        // Bottom face (Orange/Yellow)
        -0.5f, -0.5f, -0.5f,   0.9f, 0.7f, 0.1f,
         0.5f, -0.5f, -0.5f,   0.9f, 0.7f, 0.1f,
         0.5f, -0.5f,  0.5f,   0.9f, 0.7f, 0.1f,
         0.5f, -0.5f,  0.5f,   0.9f, 0.7f, 0.1f,
        -0.5f, -0.5f,  0.5f,   0.9f, 0.7f, 0.1f,
        -0.5f, -0.5f, -0.5f,   0.9f, 0.7f, 0.1f,

        // Top face (Electric Blue)
        -0.5f,  0.5f, -0.5f,   0.2f, 0.5f, 0.95f,
         0.5f,  0.5f, -0.5f,   0.2f, 0.5f, 0.95f,
         0.5f,  0.5f,  0.5f,   0.2f, 0.5f, 0.95f,
         0.5f,  0.5f,  0.5f,   0.2f, 0.5f, 0.95f,
        -0.5f,  0.5f,  0.5f,   0.2f, 0.5f, 0.95f,
        -0.5f,  0.5f, -0.5f,   0.2f, 0.5f, 0.95f
    };

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute (layout = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color attribute (layout = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 9. Uniform locations
    glUseProgram(shaderProgram);
    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    int viewLoc  = glGetUniformLocation(shaderProgram, "view");
    int projLoc  = glGetUniformLocation(shaderProgram, "projection");

    // Camera view setup
    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, 1.5f, 3.5f), // Eye
        glm::vec3(0.0f, 0.0f, 0.0f), // Target
        glm::vec3(0.0f, 1.0f, 0.0f)  // Up
    );
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    float autoRotAngle = 0.0f;
    float lastFrameTime = static_cast<float>(glfwGetTime());

    // 10. Main Render Loop
    while (!glfwWindowShouldClose(window)) {
        float currentFrameTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        processInput(window);

        if (!isPaused) {
            autoRotAngle += deltaTime * 0.8f;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Dynamic projection based on current aspect ratio
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        float aspect = (height > 0) ? static_cast<float>(width) / static_cast<float>(height) : 1.0f;
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

        // Transformation: Model rotation
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, autoRotAngle + yawAngle, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, pitchAngle + std::sin(autoRotAngle * 0.5f) * 0.3f, glm::vec3(1.0f, 0.0f, 0.0f));

        glUseProgram(shaderProgram);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 11. Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

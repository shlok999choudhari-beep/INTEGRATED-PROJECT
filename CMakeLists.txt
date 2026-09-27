cmake_minimum_required(VERSION 3.20)
project(AntigravityOpenGL C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# 1. Fetch GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glfw
    URL https://github.com/glfw/glfw/archive/refs/tags/3.4.zip
)

# 2. Fetch GLM (Header-only mathematics library)
FetchContent_Declare(
    glm
    URL https://github.com/g-truc/glm/archive/refs/tags/1.0.1.zip
)

message(STATUS "Fetching external dependencies (GLFW, GLM)...")
FetchContent_MakeAvailable(glfw glm)

# 3. Include directories for local headers (GLAD)
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 4. Executable target
add_executable(opengl_demo
    src/main.cpp
    src/glad.c
)

target_link_libraries(opengl_demo
    PRIVATE
    glfw
    glm::glm
    opengl32
)

target_link_options(opengl_demo
    PRIVATE
    -static
)

# 5. Copy shaders to the build/executable directory
add_custom_command(TARGET opengl_demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders
    $<TARGET_FILE_DIR:opengl_demo>/shaders
)

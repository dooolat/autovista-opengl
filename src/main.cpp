#define GLFW_INCLUDE_NONE
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "AvtoVista.h"
#include "CameraControls.h"
#include "ConfiguratorControls.h"
#include "Lighting.h"
#include "ControlHelpOverlay.h"

#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef AUTOVISTA_SHADER_DIR
#define AUTOVISTA_SHADER_DIR "shaders"
#endif

namespace {

std::string readText(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) throw std::runtime_error("Cannot open shader: " + path.string());
    std::ostringstream stream;
    stream << file.rdbuf();
    return stream.str();
}

GLuint compileShader(GLenum type, const std::string& source, const char* label) {
    const GLuint shader = glCreateShader(type);
    const char* data = source.c_str();
    glShaderSource(shader, 1, &data, nullptr);
    glCompileShader(shader);
    GLint success = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_TRUE) return shader;
    GLint length = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    std::string log(static_cast<std::size_t>(length), '\0');
    glGetShaderInfoLog(shader, length, nullptr, log.data());
    std::cerr << label << " shader compilation failed:\n" << log << '\n';
    glDeleteShader(shader);
    return 0;
}

GLuint createProgram(const std::filesystem::path& shaderDir) {
    const GLuint vertex = compileShader(GL_VERTEX_SHADER, readText(shaderDir / "scene.vert"), "Vertex");
    const GLuint fragment = compileShader(GL_FRAGMENT_SHADER, readText(shaderDir / "scene.frag"), "Fragment");
    if (!vertex || !fragment) {
        if (vertex) glDeleteShader(vertex);
        if (fragment) glDeleteShader(fragment);
        return 0;
    }
    const GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    GLint success = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success == GL_TRUE) return program;
    GLint length = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
    std::string log(static_cast<std::size_t>(length), '\0');
    glGetProgramInfoLog(program, length, nullptr, log.data());
    std::cerr << "Shader program link failed:\n" << log << '\n';
    glDeleteProgram(program);
    return 0;
}

std::array<Light, kLightCount> gLights = makeStudioLights();

void framebufferSizeCallback(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
}

} // namespace

int main() {
    if (glfwInit() != GLFW_TRUE) {
        std::cerr << "GLFW initialization failed.\n";
        return 1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(960, 640, "AutoVista - Orbit Camera Scene", nullptr, nullptr);
    if (!window) {
        std::cerr << "Could not create an OpenGL 3.3 window.\n";
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    if (!gladLoadGL(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress))) {
        std::cerr << "GLAD initialization failed.\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetKeyCallback(window, [](GLFWwindow* current, int key, int, int action, int) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(current, GLFW_TRUE);
        if (action == GLFW_PRESS && key >= GLFW_KEY_1 && key < GLFW_KEY_1 + kLightCount) {
            Light& light = gLights[static_cast<std::size_t>(key - GLFW_KEY_1)];
            light.enabled = !light.enabled;
        }
    });

    GLuint program = 0;
    try {
        program = createProgram(std::filesystem::path(AUTOVISTA_SHADER_DIR));
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
    }
    if (!program) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    OrbitCamera camera(glm::vec3(0.0f), 8.0f);
    CameraControls controls(window, camera);
    ConfiguratorControls configurator(window);
    ControlHelpOverlay controlHelp(window);
    AvtoVistaScene scene;
    if (!scene.initialize() || !controlHelp.initialize(std::filesystem::path(AUTOVISTA_SHADER_DIR))) {
        std::cerr << "Could not create scene geometry.\n";
        controlHelp.destroy();
        glDeleteProgram(program);
        controls.detach();
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.72f, 0.75f, 0.78f, 1.0f);
    const GLint modelLocation = glGetUniformLocation(program, "uModel");
    const GLint viewLocation = glGetUniformLocation(program, "uView");
    const GLint projectionLocation = glGetUniformLocation(program, "uProjection");
    const GLint colorLocation = glGetUniformLocation(program, "uColor");
    const GLint texturedLocation = glGetUniformLocation(program, "uUseTexture");
    const GLint viewPosLocation = glGetUniformLocation(program, "uViewPos");
    const GLint shininessLocation = glGetUniformLocation(program, "uShininess");
    const GLint specularLocation = glGetUniformLocation(program, "uSpecularStrength");
    int width = 0, height = 0;
    double previousTime = glfwGetTime();
    while (glfwWindowShouldClose(window) == GLFW_FALSE) {
        glfwPollEvents();
        const double currentTime = glfwGetTime();
        const float deltaTime = static_cast<float>(currentTime - previousTime);
        previousTime = currentTime;
        controls.update(deltaTime);
        configurator.update();
        controlHelp.update();
        const ConfiguratorState& state = configurator.state();
        if (!state.turntablePaused) scene.update(deltaTime);

        glfwGetFramebufferSize(window, &width, &height);
        if (width <= 0 || height <= 0) continue;
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(program);
        const glm::mat4 view = camera.viewMatrix();
        const glm::mat4 projection = glm::perspective(
            glm::radians(45.0f), static_cast<float>(width) / static_cast<float>(height),
            0.1f, 100.0f);
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3fv(viewPosLocation, 1, glm::value_ptr(camera.position()));
        glUniform1f(shininessLocation, state.shininess);
        glUniform1f(specularLocation, state.specularStrength);
        uploadLights(program, gLights);
        scene.render(modelLocation, colorLocation, texturedLocation, state.paintColor);
        controlHelp.render(width, height);
        glBindVertexArray(0);
        glfwSwapBuffers(window);
    }

    scene.destroy();
    controlHelp.destroy();
    glDeleteProgram(program);
    controls.detach();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

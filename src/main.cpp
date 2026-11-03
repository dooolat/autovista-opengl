#define GLFW_INCLUDE_NONE
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <glm/mat4x4.hpp>
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
    const GLuint vertex = compileShader(GL_VERTEX_SHADER, readText(shaderDir / "basic.vert"), "Vertex");
    const GLuint fragment = compileShader(GL_FRAGMENT_SHADER, readText(shaderDir / "basic.frag"), "Fragment");
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

    GLFWwindow* window = glfwCreateWindow(960, 640, "AutoVista - OpenGL Foundation", nullptr, nullptr);
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
    });

    GLuint program = 0, vao = 0, vbo = 0, ebo = 0;
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

    const float vertices[] = {
        -0.75f, -0.65f, 0.0f, 0.12f, 0.46f, 0.86f,
         0.75f, -0.65f, 0.0f, 0.20f, 0.72f, 0.38f,
         0.00f,  0.75f, 0.0f, 0.94f, 0.56f, 0.16f
    };
    const unsigned int indices[] = {0, 1, 2};
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.72f, 0.75f, 0.78f, 1.0f);
    const GLint mvpLocation = glGetUniformLocation(program, "uMVP");
    const glm::mat4 mvp(1.0f);
    int width = 0, height = 0;
    while (glfwWindowShouldClose(window) == GLFW_FALSE) {
        glfwPollEvents();
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(program);
        glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);
        glfwSwapBuffers(window);
    }

    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(program);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

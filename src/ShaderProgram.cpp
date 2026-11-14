#include "ShaderProgram.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace {

std::string readText(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Cannot open shader file: " + path.string());
    }
    std::ostringstream stream;
    stream << file.rdbuf();
    return stream.str();
}

GLuint compileShader(GLenum type, const std::string& source,
                     const std::filesystem::path& path) {
    const GLuint shader = glCreateShader(type);
    if (shader == 0) {
        std::cerr << "Could not create shader object for " << path.string() << ".\n";
        return 0;
    }

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
    std::cerr << "Shader compilation failed for " << path.string() << ":\n"
              << log << '\n';
    glDeleteShader(shader);
    return 0;
}

} // namespace

GLuint createProgram(const std::filesystem::path& shaderDir) {
    const auto vertexPath = shaderDir / "scene.vert";
    const auto fragmentPath = shaderDir / "scene.frag";
    const std::string vertexSource = readText(vertexPath);
    const std::string fragmentSource = readText(fragmentPath);
    const GLuint vertex = compileShader(GL_VERTEX_SHADER, vertexSource, vertexPath);
    const GLuint fragment = compileShader(GL_FRAGMENT_SHADER, fragmentSource, fragmentPath);

    if (vertex == 0 || fragment == 0) {
        if (vertex != 0) glDeleteShader(vertex);
        if (fragment != 0) glDeleteShader(fragment);
        return 0;
    }

    const GLuint program = glCreateProgram();
    if (program == 0) {
        std::cerr << "Could not create shader program.\n";
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return 0;
    }

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
    std::cerr << "Shader program link failed for " << vertexPath.string()
              << " and " << fragmentPath.string() << ":\n" << log << '\n';
    glDeleteProgram(program);
    return 0;
}
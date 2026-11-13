#include "ControlHelpOverlay.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>

namespace {

struct UiVertex {
    float x;
    float y;
    float r;
    float g;
    float b;
    float a;
};

using Glyph = std::array<std::uint8_t, 7>;

std::string readText(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) throw std::runtime_error("Cannot open UI shader: " + path.string());
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
    std::string log(static_cast<std::size_t>(std::max(length, 0)), '\0');
    glGetShaderInfoLog(shader, length, nullptr, log.data());
    std::cerr << label << " UI shader compilation failed:\n" << log << '\n';
    glDeleteShader(shader);
    return 0;
}

GLuint createProgram(const std::filesystem::path& shaderDirectory) {
    const GLuint vertex = compileShader(
        GL_VERTEX_SHADER, readText(shaderDirectory / "ui.vert"), "Vertex");
    const GLuint fragment = compileShader(
        GL_FRAGMENT_SHADER, readText(shaderDirectory / "ui.frag"), "Fragment");
    if (vertex == 0 || fragment == 0) {
        if (vertex != 0) glDeleteShader(vertex);
        if (fragment != 0) glDeleteShader(fragment);
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
    std::string log(static_cast<std::size_t>(std::max(length, 0)), '\0');
    glGetProgramInfoLog(program, length, nullptr, log.data());
    std::cerr << "UI shader program link failed:\n" << log << '\n';
    glDeleteProgram(program);
    return 0;
}

Glyph glyphFor(char character) {
    switch (character) {
    case 'A': return {0x0E, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11};
    case 'B': return {0x1E, 0x11, 0x11, 0x1E, 0x11, 0x11, 0x1E};
    case 'C': return {0x0E, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0E};
    case 'D': return {0x1C, 0x12, 0x11, 0x11, 0x11, 0x12, 0x1C};
    case 'E': return {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x1F};
    case 'F': return {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x10};
    case 'G': return {0x0E, 0x11, 0x10, 0x17, 0x11, 0x11, 0x0F};
    case 'H': return {0x11, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11};
    case 'I': return {0x0E, 0x04, 0x04, 0x04, 0x04, 0x04, 0x0E};
    case 'J': return {0x07, 0x02, 0x02, 0x02, 0x12, 0x12, 0x0C};
    case 'K': return {0x11, 0x12, 0x14, 0x18, 0x14, 0x12, 0x11};
    case 'L': return {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F};
    case 'M': return {0x11, 0x1B, 0x15, 0x15, 0x11, 0x11, 0x11};
    case 'N': return {0x11, 0x19, 0x15, 0x13, 0x11, 0x11, 0x11};
    case 'O': return {0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E};
    case 'P': return {0x1E, 0x11, 0x11, 0x1E, 0x10, 0x10, 0x10};
    case 'Q': return {0x0E, 0x11, 0x11, 0x11, 0x15, 0x12, 0x0D};
    case 'R': return {0x1E, 0x11, 0x11, 0x1E, 0x14, 0x12, 0x11};
    case 'S': return {0x0F, 0x10, 0x10, 0x0E, 0x01, 0x01, 0x1E};
    case 'T': return {0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04};
    case 'U': return {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E};
    case 'V': return {0x11, 0x11, 0x11, 0x11, 0x11, 0x0A, 0x04};
    case 'W': return {0x11, 0x11, 0x11, 0x15, 0x15, 0x15, 0x0A};
    case 'X': return {0x11, 0x11, 0x0A, 0x04, 0x0A, 0x11, 0x11};
    case 'Y': return {0x11, 0x11, 0x0A, 0x04, 0x04, 0x04, 0x04};
    case 'Z': return {0x1F, 0x01, 0x02, 0x04, 0x08, 0x10, 0x1F};
    case '0': return {0x0E, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0E};
    case '1': return {0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E};
    case '2': return {0x0E, 0x11, 0x01, 0x02, 0x04, 0x08, 0x1F};
    case '3': return {0x1E, 0x01, 0x01, 0x0E, 0x01, 0x01, 0x1E};
    case '4': return {0x02, 0x06, 0x0A, 0x12, 0x1F, 0x02, 0x02};
    case '5': return {0x1F, 0x10, 0x10, 0x1E, 0x01, 0x01, 0x1E};
    case '6': return {0x0E, 0x10, 0x10, 0x1E, 0x11, 0x11, 0x0E};
    case '7': return {0x1F, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08};
    case '8': return {0x0E, 0x11, 0x11, 0x0E, 0x11, 0x11, 0x0E};
    case '9': return {0x0E, 0x11, 0x11, 0x0F, 0x01, 0x01, 0x0E};
    case '-': return {0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00};
    default: return {};
    }
}

void appendRectangle(std::vector<UiVertex>& vertices, float x, float y,
                     float width, float height, int framebufferWidth,
                     int framebufferHeight, const std::array<float, 4>& color) {
    const float left = 2.0f * x / static_cast<float>(framebufferWidth) - 1.0f;
    const float right = 2.0f * (x + width) / static_cast<float>(framebufferWidth) - 1.0f;
    const float top = 1.0f - 2.0f * y / static_cast<float>(framebufferHeight);
    const float bottom = 1.0f - 2.0f * (y + height) / static_cast<float>(framebufferHeight);
    const UiVertex tl{left, top, color[0], color[1], color[2], color[3]};
    const UiVertex tr{right, top, color[0], color[1], color[2], color[3]};
    const UiVertex bl{left, bottom, color[0], color[1], color[2], color[3]};
    const UiVertex br{right, bottom, color[0], color[1], color[2], color[3]};
    vertices.insert(vertices.end(), {tl, bl, tr, tr, bl, br});
}

void appendText(std::vector<UiVertex>& vertices, const std::string& text,
                float x, float y, float scale, int framebufferWidth,
                int framebufferHeight, const std::array<float, 4>& color) {
    float cursorX = x;
    for (const char character : text) {
        const Glyph glyph = glyphFor(character);
        for (std::size_t row = 0; row < glyph.size(); ++row) {
            for (int column = 0; column < 5; ++column) {
                const bool lit = (glyph[row] & (1u << (4 - column))) != 0;
                if (!lit) continue;
                appendRectangle(vertices, cursorX + static_cast<float>(column) * scale,
                                y + static_cast<float>(row) * scale, scale, scale,
                                framebufferWidth, framebufferHeight, color);
            }
        }
        cursorX += 6.0f * scale;
    }
}

} // namespace

bool ControlHelpOverlay::initialize(const std::filesystem::path& shaderDirectory) {
    try {
        program_ = createProgram(shaderDirectory);
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return false;
    }
    if (program_ == 0) return false;

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    if (vao_ == 0 || vbo_ == 0) {
        destroy();
        return false;
    }

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STREAM_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(UiVertex), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(UiVertex),
                          reinterpret_cast<void*>(offsetof(UiVertex, r)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    return true;
}

void ControlHelpOverlay::update() {
    if (window_ == nullptr) return;
    const bool helpKeyDown = glfwGetKey(window_, GLFW_KEY_H) == GLFW_PRESS;
    if (helpKeyDown && !helpKeyWasDown_) visible_ = !visible_;
    helpKeyWasDown_ = helpKeyDown;
}

void ControlHelpOverlay::render(int framebufferWidth, int framebufferHeight) const {
    if (!visible_ || framebufferWidth < 320 || framebufferHeight < 240 || program_ == 0) return;

    const std::array<std::string, 10> lines = {
        "H TO SHOW OR HIDE",
        "MOUSE DRAG TO ORBIT",
        "WHEEL TO ZOOM",
        "WASD OR ARROWS ORBIT",
        "Q OR E TO ZOOM",
        "1-3 TOGGLE LIGHTS",
        "5-8 SELECT PAINT",
        "M MATTE OR GLOSSY",
        "SPACE PAUSE SPIN",
        "ESC QUIT"
    };
    constexpr float maxFontScale = 2.4f;
    constexpr float margin = 18.0f;
    constexpr float textLeftPadding = 16.0f;
    constexpr float textTopPadding = 14.0f;
    constexpr float rowGap = 7.0f;
    const std::size_t longestLine = std::max_element(
        lines.begin(), lines.end(), [](const std::string& left, const std::string& right) {
            return left.size() < right.size();
        })->size();
    const float panelWidth = std::min(430.0f, static_cast<float>(framebufferWidth) - 2.0f * margin);
    const float fontScale = std::min(maxFontScale,
        std::max(1.0f, (panelWidth - 2.0f * textLeftPadding) /
                          (6.0f * static_cast<float>(longestLine))));
    const float lineHeight = 7.0f * fontScale + rowGap;
    const float panelHeight = 2.0f * textTopPadding +
                              lineHeight * static_cast<float>(lines.size());

    std::vector<UiVertex> vertices;
    vertices.reserve(6000);
    appendRectangle(vertices, margin, margin, panelWidth, panelHeight,
                    framebufferWidth, framebufferHeight, {0.025f, 0.04f, 0.065f, 0.88f});
    appendRectangle(vertices, margin, margin, panelWidth, 3.0f,
                    framebufferWidth, framebufferHeight, {0.18f, 0.78f, 0.86f, 1.0f});

    float textY = margin + textTopPadding;
    for (std::size_t i = 0; i < lines.size(); ++i) {
        const std::array<float, 4> color = i == 0
            ? std::array<float, 4>{0.35f, 0.91f, 0.95f, 1.0f}
            : std::array<float, 4>{0.91f, 0.94f, 0.97f, 1.0f};
        appendText(vertices, lines[i], margin + textLeftPadding, textY, fontScale,
                   framebufferWidth, framebufferHeight, color);
        textY += lineHeight;
    }

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(program_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(vertices.size() * sizeof(UiVertex)),
                 vertices.data(), GL_STREAM_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));
    glBindVertexArray(0);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void ControlHelpOverlay::destroy() {
    if (vbo_ != 0) glDeleteBuffers(1, &vbo_);
    if (vao_ != 0) glDeleteVertexArrays(1, &vao_);
    if (program_ != 0) glDeleteProgram(program_);
    vbo_ = 0;
    vao_ = 0;
    program_ = 0;
}

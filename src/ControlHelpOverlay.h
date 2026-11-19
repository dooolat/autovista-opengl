#pragma once

#include <glad/gl.h>
#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>

#include <array>
#include <filesystem>

#include "ConfiguratorControls.h"

class ControlHelpOverlay {
public:
    explicit ControlHelpOverlay(GLFWwindow* window) : window_(window) {}

    bool initialize(const std::filesystem::path& shaderDirectory);
    void update();
    void render(int framebufferWidth, int framebufferHeight,
                const ConfiguratorState& state,
                const std::array<bool, 3>& studioLightsEnabled) const;
    void destroy();

private:
    GLFWwindow* window_ = nullptr;
    GLuint program_ = 0;
    GLuint vao_ = 0;
    GLuint vbo_ = 0;
    bool visible_ = true;
    bool helpKeyWasDown_ = false;
};

#pragma once

#include <glm/vec3.hpp>

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>

struct ConfiguratorState {
    glm::vec3 paintColor{0.78f, 0.10f, 0.055f};
    float shininess = 96.0f;
    float specularStrength = 1.0f;
    bool glossy = true;
    bool turntablePaused = false;
    bool lampsEnabled = true;
    bool reflectionsEnabled = true;
    int carVariant = 0;
    int paintPreset = 5;
};

class ConfiguratorControls {
public:
    explicit ConfiguratorControls(GLFWwindow* window) : window_(window) {
        if (window_ != nullptr) glfwSetInputMode(window_, GLFW_STICKY_KEYS, GLFW_TRUE);
    }

    void update();
    const ConfiguratorState& state() const noexcept { return state_; }

private:
    void setFinish(bool glossy);

    GLFWwindow* window_ = nullptr;
    ConfiguratorState state_;
    bool matteKeyWasDown_ = false;
    bool pauseKeyWasDown_ = false;
    bool lampToggleKeyWasDown_ = false;
    bool reflectionToggleKeyWasDown_ = false;
    bool previousCarKeyWasDown_ = false;
    bool nextCarKeyWasDown_ = false;
};

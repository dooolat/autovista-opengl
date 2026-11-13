#include "ConfiguratorControls.h"

namespace {

bool keyDown(GLFWwindow* window, int key) {
    return glfwGetKey(window, key) == GLFW_PRESS;
}

} // namespace

void ConfiguratorControls::setFinish(bool glossy) {
    state_.glossy = glossy;
    state_.shininess = glossy ? 96.0f : 10.0f;
    state_.specularStrength = glossy ? 1.0f : 0.12f;
}

void ConfiguratorControls::update() {
    if (window_ == nullptr) return;

    // 1-3 toggle studio lights; 5-8 select paint presets.
    if (keyDown(window_, GLFW_KEY_5)) state_.paintColor = {0.88f, 0.035f, 0.055f};
    if (keyDown(window_, GLFW_KEY_6)) state_.paintColor = {0.035f, 0.26f, 0.92f};
    if (keyDown(window_, GLFW_KEY_7)) state_.paintColor = {0.98f, 0.62f, 0.025f};
    if (keyDown(window_, GLFW_KEY_8)) state_.paintColor = {0.045f, 0.46f, 0.19f};

    const bool matteKeyDown = keyDown(window_, GLFW_KEY_M);
    if (matteKeyDown && !matteKeyWasDown_) setFinish(!state_.glossy);
    matteKeyWasDown_ = matteKeyDown;

    const bool pauseKeyDown = keyDown(window_, GLFW_KEY_SPACE);
    if (pauseKeyDown && !pauseKeyWasDown_) state_.turntablePaused = !state_.turntablePaused;
    pauseKeyWasDown_ = pauseKeyDown;
}

#include "CameraControls.h"

#include <algorithm>
#include <cmath>

CameraControls::CameraControls(GLFWwindow* window, OrbitCamera& camera)
    : window_(window), camera_(&camera) {
    glfwSetWindowUserPointer(window_, this);
    glfwSetMouseButtonCallback(window_, mouseButtonCallback);
    glfwSetCursorPosCallback(window_, cursorPositionCallback);
    glfwSetScrollCallback(window_, scrollCallback);
}

void CameraControls::update(float deltaTimeSeconds) {
    if (window_ == nullptr || camera_ == nullptr || !std::isfinite(deltaTimeSeconds)) return;

    const float deltaTime = std::clamp(deltaTimeSeconds, 0.0f, 0.05f);
    const bool left = glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS ||
                      glfwGetKey(window_, GLFW_KEY_LEFT) == GLFW_PRESS;
    const bool right = glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS ||
                       glfwGetKey(window_, GLFW_KEY_RIGHT) == GLFW_PRESS;
    const bool up = glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS ||
                    glfwGetKey(window_, GLFW_KEY_UP) == GLFW_PRESS;
    const bool down = glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS ||
                      glfwGetKey(window_, GLFW_KEY_DOWN) == GLFW_PRESS;
    const bool zoomIn = glfwGetKey(window_, GLFW_KEY_Q) == GLFW_PRESS;
    const bool zoomOut = glfwGetKey(window_, GLFW_KEY_E) == GLFW_PRESS;

    constexpr float radiansPerSecond = 1.4f;
    constexpr float zoomStepsPerSecond = 4.0f;
    const float yawDirection = static_cast<float>(right) - static_cast<float>(left);
    const float pitchDirection = static_cast<float>(up) - static_cast<float>(down);
    camera_->orbit(yawDirection * radiansPerSecond * deltaTime,
                   pitchDirection * radiansPerSecond * deltaTime);
    camera_->zoom((static_cast<float>(zoomIn) - static_cast<float>(zoomOut)) *
                  zoomStepsPerSecond * deltaTime);
}

void CameraControls::detach() {
    if (window_ == nullptr) return;

    glfwSetMouseButtonCallback(window_, nullptr);
    glfwSetCursorPosCallback(window_, nullptr);
    glfwSetScrollCallback(window_, nullptr);
    if (glfwGetWindowUserPointer(window_) == this) {
        glfwSetWindowUserPointer(window_, nullptr);
    }
    window_ = nullptr;
}

void CameraControls::mouseButtonCallback(GLFWwindow* window, int button,
                                         int action, int) {
    if (button != GLFW_MOUSE_BUTTON_LEFT) return;
    auto* controls = static_cast<CameraControls*>(glfwGetWindowUserPointer(window));
    if (controls == nullptr) return;

    if (action == GLFW_PRESS) {
        controls->dragging_ = true;
        glfwGetCursorPos(window, &controls->lastX_, &controls->lastY_);
        controls->hasLastPosition_ = true;
    } else if (action == GLFW_RELEASE) {
        controls->dragging_ = false;
        controls->hasLastPosition_ = false;
    }
}

void CameraControls::cursorPositionCallback(GLFWwindow* window, double x, double y) {
    auto* controls = static_cast<CameraControls*>(glfwGetWindowUserPointer(window));
    if (controls == nullptr || !controls->dragging_ || controls->camera_ == nullptr) return;

    if (!controls->hasLastPosition_) {
        controls->lastX_ = x;
        controls->lastY_ = y;
        controls->hasLastPosition_ = true;
        return;
    }

    constexpr float radiansPerPixel = 0.005f;
    const double deltaX = x - controls->lastX_;
    const double deltaY = y - controls->lastY_;
    controls->lastX_ = x;
    controls->lastY_ = y;

    controls->camera_->orbit(static_cast<float>(deltaX) * radiansPerPixel,
                             static_cast<float>(deltaY) * radiansPerPixel);
}

void CameraControls::scrollCallback(GLFWwindow* window, double, double yOffset) {
    auto* controls = static_cast<CameraControls*>(glfwGetWindowUserPointer(window));
    if (controls != nullptr && controls->camera_ != nullptr) {
        controls->camera_->zoom(static_cast<float>(yOffset));
    }
}

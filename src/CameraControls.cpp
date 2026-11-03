#include "CameraControls.h"

CameraControls::CameraControls(GLFWwindow* window, OrbitCamera& camera)
    : window_(window), camera_(&camera) {
    glfwSetWindowUserPointer(window_, this);
    glfwSetMouseButtonCallback(window_, mouseButtonCallback);
    glfwSetCursorPosCallback(window_, cursorPositionCallback);
    glfwSetScrollCallback(window_, scrollCallback);
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

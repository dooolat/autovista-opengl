#pragma once

#include "OrbitCamera.h"

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>

class CameraControls {
public:
    CameraControls(GLFWwindow* window, OrbitCamera& camera);
    CameraControls(const CameraControls&) = delete;
    CameraControls& operator=(const CameraControls&) = delete;

    void update(float deltaTimeSeconds);
    void detach();

private:
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int modifiers);
    static void cursorPositionCallback(GLFWwindow* window, double x, double y);
    static void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);

    GLFWwindow* window_ = nullptr;
    OrbitCamera* camera_ = nullptr;
    bool dragging_ = false;
    bool hasLastPosition_ = false;
    double lastX_ = 0.0;
    double lastY_ = 0.0;
};

#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

class OrbitCamera {
public:
    explicit OrbitCamera(glm::vec3 target = glm::vec3(0.0f),
                         float distance = 8.0f,
                         float yawRadians = 0.785398163f,
                         float pitchRadians = 0.34906585f);

    void orbit(float deltaYawRadians, float deltaPitchRadians);
    void zoom(float scrollSteps);

    glm::vec3 position() const;
    glm::mat4 viewMatrix() const;

    const glm::vec3& target() const noexcept { return target_; }
    float distance() const noexcept { return distance_; }
    float yaw() const noexcept { return yaw_; }
    float pitch() const noexcept { return pitch_; }

private:
    glm::vec3 target_;
    float distance_;
    float yaw_;
    float pitch_;
};

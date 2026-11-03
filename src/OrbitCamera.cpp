#include "OrbitCamera.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace {

constexpr float kMaxPitch = 1.396263402f; // 80 degrees in radians
constexpr float kMinDistance = 4.5f;
constexpr float kMaxDistance = 18.0f;
constexpr float kZoomSensitivity = 0.12f;

} // namespace

OrbitCamera::OrbitCamera(glm::vec3 target, float distance, float yawRadians, float pitchRadians)
    : target_(target), distance_(distance), yaw_(yawRadians), pitch_(pitchRadians) {
    if (!std::isfinite(distance_) || distance_ <= 0.0f) {
        throw std::invalid_argument("OrbitCamera distance must be positive and finite.");
    }
    if (!std::isfinite(yaw_) || !std::isfinite(pitch_)) {
        throw std::invalid_argument("OrbitCamera angles must be finite.");
    }

    distance_ = std::clamp(distance_, kMinDistance, kMaxDistance);
    yaw_ = std::remainder(yaw_, glm::two_pi<float>());
    pitch_ = std::clamp(pitch_, -kMaxPitch, kMaxPitch);
}

void OrbitCamera::orbit(float deltaYawRadians, float deltaPitchRadians) {
    if (!std::isfinite(deltaYawRadians) || !std::isfinite(deltaPitchRadians)) {
        return;
    }

    yaw_ = std::remainder(yaw_ + deltaYawRadians, glm::two_pi<float>());
    pitch_ = std::clamp(pitch_ + deltaPitchRadians, -kMaxPitch, kMaxPitch);
}

void OrbitCamera::zoom(float scrollSteps) {
    if (!std::isfinite(scrollSteps)) {
        return;
    }

    distance_ = std::clamp(
        distance_ * std::exp(-scrollSteps * kZoomSensitivity),
        kMinDistance,
        kMaxDistance);
}

glm::vec3 OrbitCamera::position() const {
    const float horizontalDistance = distance_ * std::cos(pitch_);
    return target_ + glm::vec3(
        horizontalDistance * std::cos(yaw_),
        distance_ * std::sin(pitch_),
        horizontalDistance * std::sin(yaw_));
}

glm::mat4 OrbitCamera::viewMatrix() const {
    return glm::lookAt(position(), target_, glm::vec3(0.0f, 1.0f, 0.0f));
}

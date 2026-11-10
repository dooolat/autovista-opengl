#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <array>
#include <string>

// Studio lights. Keys 1/2/3 toggle them (see main.cpp).
struct Light {
    glm::vec3 position;
    glm::vec3 color;
    float intensity;
    bool enabled;
};

constexpr int kLightCount = 3;

inline std::array<Light, kLightCount> makeStudioLights() {
    return {{
        {{ 4.0f, 5.0f,  4.0f}, {1.00f, 1.00f, 1.00f}, 1.6f, true}, // key
        {{-5.0f, 3.0f,  2.0f}, {0.55f, 0.70f, 1.00f}, 1.0f, true}, // fill (cool)
        {{ 0.0f, 4.0f, -5.0f}, {1.00f, 0.80f, 0.60f}, 1.2f, true}, // rim (warm)
    }};
}

inline void uploadLights(GLuint program, const std::array<Light, kLightCount>& lights) {
    for (int i = 0; i < kLightCount; ++i) {
        const std::string base = "uLights[" + std::to_string(i) + "].";
        glUniform3fv(glGetUniformLocation(program, (base + "position").c_str()), 1,
                     glm::value_ptr(lights[i].position));
        glUniform3fv(glGetUniformLocation(program, (base + "color").c_str()), 1,
                     glm::value_ptr(lights[i].color));
        glUniform1f(glGetUniformLocation(program, (base + "intensity").c_str()),
                    lights[i].intensity);
        glUniform1i(glGetUniformLocation(program, (base + "enabled").c_str()),
                    lights[i].enabled ? 1 : 0);
    }
}

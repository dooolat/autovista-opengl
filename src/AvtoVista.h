#pragma once

#include <glad/gl.h>
#include <glm/vec3.hpp>

#include "Texture2D.h"

#include <cstdint>
#include <vector>

class AvtoVistaScene {
public:
    AvtoVistaScene() = default;
    AvtoVistaScene(const AvtoVistaScene&) = delete;
    AvtoVistaScene& operator=(const AvtoVistaScene&) = delete;

    bool initialize();
    void update(float deltaTimeSeconds);
    void render(GLint modelLocation, GLint colorLocation, GLint texturedLocation,
                const glm::vec3& paintColor) const;
    void destroy();

private:
    struct Mesh {
        GLuint vao = 0;
        GLuint vbo = 0;
        GLuint ebo = 0;
        GLsizei indexCount = 0;

        bool create(const std::vector<float>& vertices,
                    const std::vector<std::uint32_t>& indices,
                    unsigned int vertexStride = 3);
        void draw() const;
        void destroy();
    };

    Mesh cylinder_;
    Mesh cube_;
    Mesh floor_;
    Texture2D floorTexture_;
    float angle_ = 0.0f;
};

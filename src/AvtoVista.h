#pragma once

#include <glad/gl.h>
#include <glm/vec3.hpp>

#include "Texture2D.h"
#include "CubeMapTexture.h"

#include <cstdint>
#include <vector>

class AvtoVistaScene {
public:
    AvtoVistaScene() = default;
    AvtoVistaScene(const AvtoVistaScene&) = delete;
    AvtoVistaScene& operator=(const AvtoVistaScene&) = delete;

    bool initialize();
    void update(float deltaTimeSeconds);
    void render(GLint modelLocation, GLint normalLocation, GLint colorLocation,
                GLint texturedLocation, GLint paintMaterialLocation,
                const HeadlightUniforms& headlights, const glm::vec3& paintColor,
                bool lampsEnabled, int carVariant = 0) const;
    void destroy();

private:
    struct Mesh {
        GLuint vao = 0;
        GLuint vbo = 0;
        GLuint ebo = 0;
        GLsizei indexCount = 0;

        bool create(const std::vector<float>& vertices,
                    const std::vector<std::uint32_t>& indices);
        void draw() const;
        void destroy();
    };

    Mesh cylinder_;
    Mesh cube_;
    Mesh floor_;
    Texture2D floorTexture_;
    CubeMapTexture environmentMap_;
    float angle_ = 0.0f;
};

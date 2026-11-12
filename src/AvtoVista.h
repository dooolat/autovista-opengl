#pragma once

#include <glad/gl.h>

#include <cstdint>
#include <vector>

class AvtoVistaScene {
public:
    AvtoVistaScene() = default;
    AvtoVistaScene(const AvtoVistaScene&) = delete;
    AvtoVistaScene& operator=(const AvtoVistaScene&) = delete;

    bool initialize();
    void update(float deltaTimeSeconds);
    void render(GLint modelLocation, GLint colorLocation) const;
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
    float angle_ = 0.0f;
};

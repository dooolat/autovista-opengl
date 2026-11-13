#pragma once

#include <glad/gl.h>

class Texture2D {
public:
    Texture2D() = default;
    Texture2D(const Texture2D&) = delete;
    Texture2D& operator=(const Texture2D&) = delete;

    bool initialize();
    void bind(GLuint textureUnit) const;
    void destroy();

private:
    GLuint texture_ = 0;
};

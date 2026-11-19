#pragma once

#include <glad/gl.h>

class CubeMapTexture {
public:
    CubeMapTexture() = default;
    CubeMapTexture(const CubeMapTexture&) = delete;
    CubeMapTexture& operator=(const CubeMapTexture&) = delete;

    bool initialize();
    void bind(GLuint textureUnit) const;
    void destroy();

private:
    GLuint texture_ = 0;
};
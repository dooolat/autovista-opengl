#include "Texture2D.h"

#include <algorithm>
#include <cstdint>
#include <vector>

bool Texture2D::initialize() {
    constexpr int size = 256;
    constexpr int tileSize = 64;
    constexpr int groutSize = 4;
    std::vector<std::uint8_t> pixels(size * size * 4);

    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            const int tileX = x / tileSize;
            const int tileY = y / tileSize;
            const unsigned int seed = static_cast<unsigned int>(tileX * 37 + tileY * 61 + 19);
            const int variation = static_cast<int>((seed * 17u + 23u) % 13u) - 6;
            const int grain = ((x * 13 + y * 7 + tileX * 11 + tileY * 3) % 7) - 3;
            const bool grout = x % tileSize < groutSize || y % tileSize < groutSize;
            const int base = grout ? 54 : 112 + variation + grain;
            const std::size_t offset = static_cast<std::size_t>((y * size + x) * 4);
            pixels[offset] = static_cast<std::uint8_t>(std::clamp(base + 1, 0, 255));
            pixels[offset + 1] = static_cast<std::uint8_t>(std::clamp(base + 4, 0, 255));
            pixels[offset + 2] = static_cast<std::uint8_t>(std::clamp(base + 9, 0, 255));
            pixels[offset + 3] = 255;
        }
    }

    glGenTextures(1, &texture_);
    if (texture_ == 0) return false;

    glBindTexture(GL_TEXTURE_2D, texture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size, size, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, pixels.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    return true;
}

void Texture2D::bind(GLuint textureUnit) const {
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, texture_);
}

void Texture2D::destroy() {
    if (texture_ != 0) glDeleteTextures(1, &texture_);
    texture_ = 0;
}

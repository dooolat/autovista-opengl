#include "CubeMapTexture.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <cmath>
#include <vector>

namespace {

using Color = std::array<std::uint8_t, 3>;

struct FacePalette {
    Color sky;
    Color horizon;
    Color ground;
};

Color blend(const Color& from, const Color& to, float amount) {
    Color result{};
    for (std::size_t channel = 0; channel < result.size(); ++channel) {
        const float value = static_cast<float>(from[channel]) +
            (static_cast<float>(to[channel]) - static_cast<float>(from[channel])) * amount;
        result[channel] = static_cast<std::uint8_t>(
            std::clamp(static_cast<int>(value), 0, 255));
    }
    return result;
}

} // namespace

bool CubeMapTexture::initialize() {
    constexpr int size = 64;
    const std::array<FacePalette, 6> faces = {{
        {{105, 157, 211}, {232, 196, 150}, {67, 79, 93}},
        {{92, 145, 204}, {222, 185, 142}, {57, 70, 86}},
        {{120, 171, 220}, {221, 221, 211}, {112, 132, 150}},
        {{57, 67, 78}, {75, 83, 90}, {43, 49, 56}},
        {{101, 153, 205}, {232, 196, 150}, {68, 81, 96}},
        {{89, 143, 201}, {221, 184, 140}, {59, 71, 85}}
    }};
    std::vector<std::uint8_t> pixels(static_cast<std::size_t>(size * size * 3));

    glGenTextures(1, &texture_);
    if (texture_ == 0) return false;
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_);

    for (std::size_t face = 0; face < faces.size(); ++face) {
        for (int y = 0; y < size; ++y) {
            const float vertical = static_cast<float>(y) / static_cast<float>(size - 1);
            const Color rowColor = vertical < 0.58f
                ? blend(faces[face].sky, faces[face].horizon, vertical / 0.58f)
                : blend(faces[face].horizon, faces[face].ground,
                        (vertical - 0.58f) / 0.42f);
            for (int x = 0; x < size; ++x) {
                const float edgeShade = 1.0f - 0.08f *
                    std::abs(static_cast<float>(x) / static_cast<float>(size - 1) - 0.5f);
                const std::size_t offset =
                    static_cast<std::size_t>((y * size + x) * 3);
                for (std::size_t channel = 0; channel < rowColor.size(); ++channel) {
                    pixels[offset + channel] = static_cast<std::uint8_t>(
                        static_cast<float>(rowColor[channel]) * edgeShade);
                }
            }
        }

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<GLenum>(face),
                     0, GL_RGB8, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE,
                     pixels.data());
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    return true;
}

void CubeMapTexture::bind(GLuint textureUnit) const {
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_);
}

void CubeMapTexture::destroy() {
    if (texture_ != 0) glDeleteTextures(1, &texture_);
    texture_ = 0;
}
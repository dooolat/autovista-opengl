#include "AvtoVista.h"

#include <cmath>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace {

constexpr float kPi = 3.14159265359f;
constexpr float kTau = 6.28318530718f;

struct MeshData {
    std::vector<float> vertices;
    std::vector<std::uint32_t> indices;
};

MeshData cylinderData(unsigned int segments = 64) {
    MeshData mesh;
    const float step = kTau / static_cast<float>(segments);

    for (unsigned int segment = 0; segment < segments; ++segment) {
        const float a = static_cast<float>(segment) * step;
        const float b = static_cast<float>(segment + 1) * step;
        const float x0 = std::cos(a);
        const float z0 = std::sin(a);
        const float x1 = std::cos(b);
        const float z1 = std::sin(b);
        const auto first = static_cast<std::uint32_t>(mesh.vertices.size() / 3);

        mesh.vertices.insert(mesh.vertices.end(), {
            x0, -0.5f, z0, x0, 0.5f, z0,
            x1, 0.5f, z1, x1, -0.5f, z1
        });
        mesh.indices.insert(mesh.indices.end(), {
            first, first + 1, first + 2,
            first, first + 2, first + 3
        });
    }

    const auto center = static_cast<std::uint32_t>(mesh.vertices.size() / 3);
    mesh.vertices.insert(mesh.vertices.end(), {0.0f, 0.5f, 0.0f, 0.0f, -0.5f, 0.0f});
    const auto bottomCenter = center + 1;

    for (unsigned int segment = 0; segment < segments; ++segment) {
        const float a = static_cast<float>(segment) * step;
        const float b = static_cast<float>(segment + 1) * step;
        const auto first = static_cast<std::uint32_t>(mesh.vertices.size() / 3);
        mesh.vertices.insert(mesh.vertices.end(), {
            std::cos(a), 0.5f, std::sin(a),
            std::cos(b), 0.5f, std::sin(b),
            std::cos(a), -0.5f, std::sin(a),
            std::cos(b), -0.5f, std::sin(b)
        });
        mesh.indices.insert(mesh.indices.end(), {
            center, first + 1, first,
            bottomCenter, first + 2, first + 3
        });
    }

    return mesh;
}

MeshData floorData() {
    MeshData mesh;
    constexpr float extent = 5.0f;
    constexpr float textureRepeat = 8.0f;
    constexpr float y = -0.18f;
    mesh.vertices = {
        -extent, y, -extent, 0.0f, 0.0f,
         extent, y, -extent, textureRepeat, 0.0f,
         extent, y,  extent, textureRepeat, textureRepeat,
        -extent, y,  extent, 0.0f, textureRepeat
    };
    mesh.indices = {0, 2, 1, 0, 3, 2};
    return mesh;
}

MeshData cubeData() {
    MeshData mesh;
    mesh.vertices = {
        -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f
    };
    mesh.indices = {
        0, 2, 1, 0, 3, 2, 4, 5, 6, 4, 6, 7,
        0, 1, 5, 0, 5, 4, 3, 7, 6, 3, 6, 2,
        0, 4, 7, 0, 7, 3, 1, 2, 6, 1, 6, 5
    };
    return mesh;
}

glm::mat4 boxTransform(const glm::vec3& position, const glm::vec3& size,
                       float zRotation = 0.0f) {
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
    if (zRotation != 0.0f) {
        transform = glm::rotate(transform, zRotation, glm::vec3(0.0f, 0.0f, 1.0f));
    }
    return glm::scale(transform, size);
}

} // namespace

bool AvtoVistaScene::Mesh::create(const std::vector<float>& vertices,
                                  const std::vector<std::uint32_t>& indices,
                                  unsigned int vertexStride) {
    if (vertices.empty() || indices.empty() || vertexStride < 3 ||
        vertices.size() % vertexStride != 0) return false;

    indexCount = static_cast<GLsizei>(indices.size());
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(vertices.size() * sizeof(float)),
                 vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(indices.size() * sizeof(std::uint32_t)),
                 indices.data(), GL_STATIC_DRAW);
    const GLsizei stride = static_cast<GLsizei>(vertexStride * sizeof(float));
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, nullptr);
    glEnableVertexAttribArray(0);
    if (vertexStride >= 5) {
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride,
                              reinterpret_cast<void*>(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }
    glBindVertexArray(0);
    return vao != 0 && vbo != 0 && ebo != 0;
}

void AvtoVistaScene::Mesh::draw() const {
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
}

void AvtoVistaScene::Mesh::destroy() {
    if (ebo != 0) glDeleteBuffers(1, &ebo);
    if (vbo != 0) glDeleteBuffers(1, &vbo);
    if (vao != 0) glDeleteVertexArrays(1, &vao);
    vao = vbo = ebo = 0;
    indexCount = 0;
}

bool AvtoVistaScene::initialize() {
    const MeshData cylinder = cylinderData();
    const MeshData cube = cubeData();
    const MeshData floor = floorData();
    if (!cylinder_.create(cylinder.vertices, cylinder.indices) ||
        !cube_.create(cube.vertices, cube.indices) ||
        !floor_.create(floor.vertices, floor.indices, 5) ||
        !floorTexture_.initialize()) {
        destroy();
        return false;
    }
    return true;
}

void AvtoVistaScene::update(float deltaTimeSeconds) {
    if (std::isfinite(deltaTimeSeconds) && deltaTimeSeconds > 0.0f) {
        angle_ = std::fmod(angle_ + deltaTimeSeconds * 0.35f, kTau);
    }
}

void AvtoVistaScene::render(GLint modelLocation, GLint colorLocation,
                            GLint texturedLocation,
                            const glm::vec3& paintColor) const {
    const glm::mat4 identity(1.0f);
    const glm::mat4 turntable = glm::rotate(identity, angle_, glm::vec3(0.0f, 1.0f, 0.0f));
    const auto draw = [&](const Mesh& mesh, const glm::mat4& model, const glm::vec3& color) {
        glUniform1i(texturedLocation, GL_FALSE);
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3fv(colorLocation, 1, glm::value_ptr(color));
        mesh.draw();
    };
    const auto drawCarBox = [&](const glm::vec3& position, const glm::vec3& size,
                                const glm::vec3& color, float zRotation = 0.0f) {
        draw(cube_, turntable * boxTransform(position, size, zRotation), color);
    };
    const auto drawPaintedCarBox = [&](const glm::vec3& position, const glm::vec3& size) {
        draw(cube_, turntable * boxTransform(position, size), paintColor);
    };

    // Textured showroom floor. Keep it separate so the car meshes stay unchanged.
    floorTexture_.bind(0);
    glUniform1i(texturedLocation, GL_TRUE);
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(identity));
    glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(1.0f)));
    floor_.draw();

    // Rotating display platform.
    draw(cylinder_, turntable * boxTransform({0.0f, -0.06f, 0.0f}, {4.5f, 0.18f, 4.5f}),
         {0.055f, 0.065f, 0.08f});
    draw(cylinder_, turntable * boxTransform({0.0f, -0.06f, 0.0f}, {4.5f, 0.18f, 4.5f}),
         {0.055f, 0.065f, 0.08f});
    draw(cylinder_, turntable * boxTransform({0.0f, 0.035f, 0.0f}, {4.28f, 0.035f, 4.28f}),
         {0.27f, 0.31f, 0.35f});

    // Main body and cabin.
    drawPaintedCarBox({0.0f, 0.43f, 0.0f}, {2.55f, 0.43f, 1.12f});
    drawPaintedCarBox({-0.18f, 0.77f, 0.0f}, {1.35f, 0.34f, 0.91f});

    // Dark windows and sloped front and rear glass.
    for (const float side : {-1.0f, 1.0f}) {
        const float z = side * 0.468f;
        drawCarBox({-0.23f, 0.78f, z}, {0.52f, 0.20f, 0.035f},
                   {0.035f, 0.075f, 0.095f});
        drawCarBox({0.29f, 0.78f, z}, {0.40f, 0.20f, 0.035f},
                   {0.035f, 0.075f, 0.095f});
        drawCarBox({0.28f, 0.77f, side * 0.60f}, {0.16f, 0.08f, 0.12f},
                   {0.06f, 0.07f, 0.08f});
    }
    drawCarBox({0.48f, 0.77f, 0.0f}, {0.045f, 0.24f, 0.72f},
               {0.035f, 0.075f, 0.095f}, -0.28f);
    drawCarBox({-0.76f, 0.76f, 0.0f}, {0.045f, 0.22f, 0.72f},
               {0.035f, 0.075f, 0.095f}, 0.30f);

    // Four tires with metallic hubs.
    for (const float side : {-1.0f, 1.0f}) {
        for (const float x : {-0.78f, 0.78f}) {
            const glm::mat4 tire = turntable *
                glm::translate(identity, {x, 0.34f, side * 0.565f}) *
                glm::rotate(identity, 0.5f * kPi, {1.0f, 0.0f, 0.0f}) *
                glm::scale(identity, {0.30f, 0.16f, 0.30f});
            draw(cylinder_, tire, {0.035f, 0.04f, 0.047f});

            const glm::mat4 hub = turntable *
                glm::translate(identity, {x, 0.34f, side * 0.66f}) *
                glm::rotate(identity, 0.5f * kPi, {1.0f, 0.0f, 0.0f}) *
                glm::scale(identity, {0.15f, 0.025f, 0.15f});
            draw(cylinder_, hub, {0.52f, 0.58f, 0.63f});
        }
    }

    // Headlights, tail lights, grille, and bumpers.
    for (const float side : {-1.0f, 1.0f}) {
        drawCarBox({1.285f, 0.49f, side * 0.34f}, {0.04f, 0.105f, 0.19f},
                   {0.98f, 0.82f, 0.48f});
        drawCarBox({-1.285f, 0.47f, side * 0.35f}, {0.04f, 0.11f, 0.18f},
                   {0.72f, 0.035f, 0.025f});
    }
    drawCarBox({1.30f, 0.34f, 0.0f}, {0.06f, 0.15f, 0.48f},
               {0.045f, 0.052f, 0.06f});
    drawCarBox({-1.30f, 0.32f, 0.0f}, {0.06f, 0.13f, 0.55f},
               {0.045f, 0.052f, 0.06f});
}

void AvtoVistaScene::destroy() {
    floorTexture_.destroy();
    floor_.destroy();
    cube_.destroy();
    cylinder_.destroy();
}

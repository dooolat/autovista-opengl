#include "AvtoVista.h"

#include <cmath>

#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace {

constexpr float kPi = 3.14159265359f;
constexpr float kTau = 6.28318530718f;

struct MeshData {
    std::vector<float> vertices;
    std::vector<std::uint32_t> indices;
};

constexpr unsigned int kVertexStride = 8;

std::uint32_t appendVertex(MeshData& mesh, const glm::vec3& position,
                           const glm::vec3& normal, float u = 0.0f, float v = 0.0f) {
    const auto index = static_cast<std::uint32_t>(mesh.vertices.size() / kVertexStride);
    mesh.vertices.insert(mesh.vertices.end(), {
        position.x, position.y, position.z,
        normal.x, normal.y, normal.z,
        u, v
    });
    return index;
}

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
        const auto first = appendVertex(mesh, {x0, -0.5f, z0}, {x0, 0.0f, z0});
        appendVertex(mesh, {x0, 0.5f, z0}, {x0, 0.0f, z0});
        appendVertex(mesh, {x1, 0.5f, z1}, {x1, 0.0f, z1});
        appendVertex(mesh, {x1, -0.5f, z1}, {x1, 0.0f, z1});
        mesh.indices.insert(mesh.indices.end(), {
            first, first + 1, first + 2,
            first, first + 2, first + 3
        });
    }

    for (unsigned int segment = 0; segment < segments; ++segment) {
        const float a = static_cast<float>(segment) * step;
        const float b = static_cast<float>(segment + 1) * step;
        const auto topCenter = appendVertex(mesh, {0.0f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f});
        const auto topA = appendVertex(mesh, {std::cos(a), 0.5f, std::sin(a)}, {0.0f, 1.0f, 0.0f});
        const auto topB = appendVertex(mesh, {std::cos(b), 0.5f, std::sin(b)}, {0.0f, 1.0f, 0.0f});
        const auto bottomCenter = appendVertex(mesh, {0.0f, -0.5f, 0.0f}, {0.0f, -1.0f, 0.0f});
        const auto bottomA = appendVertex(mesh, {std::cos(a), -0.5f, std::sin(a)}, {0.0f, -1.0f, 0.0f});
        const auto bottomB = appendVertex(mesh, {std::cos(b), -0.5f, std::sin(b)}, {0.0f, -1.0f, 0.0f});
        mesh.indices.insert(mesh.indices.end(), {
            topCenter, topB, topA,
            bottomCenter, bottomA, bottomB
        });
    }

    return mesh;
}

MeshData floorData() {
    MeshData mesh;
    constexpr float extent = 5.0f;
    constexpr float textureRepeat = 8.0f;
    constexpr float y = -0.18f;
    appendVertex(mesh, {-extent, y, -extent}, {0.0f, 1.0f, 0.0f}, 0.0f, 0.0f);
    appendVertex(mesh, { extent, y, -extent}, {0.0f, 1.0f, 0.0f}, textureRepeat, 0.0f);
    appendVertex(mesh, { extent, y,  extent}, {0.0f, 1.0f, 0.0f}, textureRepeat, textureRepeat);
    appendVertex(mesh, {-extent, y,  extent}, {0.0f, 1.0f, 0.0f}, 0.0f, textureRepeat);
    mesh.indices = {0, 2, 1, 0, 3, 2};
    return mesh;
}

MeshData cubeData() {
    MeshData mesh;
    const auto addFace = [&](const glm::vec3& normal, const glm::vec3& a,
                             const glm::vec3& b, const glm::vec3& c,
                             const glm::vec3& d) {
        const auto first = appendVertex(mesh, a, normal);
        appendVertex(mesh, b, normal);
        appendVertex(mesh, c, normal);
        appendVertex(mesh, d, normal);
        mesh.indices.insert(mesh.indices.end(), {
            first, first + 1, first + 2,
            first, first + 2, first + 3
        });
    };
    addFace({0.0f, 0.0f, 1.0f}, {-0.5f,-0.5f, 0.5f}, {0.5f,-0.5f, 0.5f},
            {0.5f,0.5f,0.5f}, {-0.5f,0.5f,0.5f});
    addFace({0.0f, 0.0f,-1.0f}, {0.5f,-0.5f,-0.5f}, {-0.5f,-0.5f,-0.5f},
            {-0.5f,0.5f,-0.5f}, {0.5f,0.5f,-0.5f});
    addFace({1.0f, 0.0f, 0.0f}, {0.5f,-0.5f,0.5f}, {0.5f,-0.5f,-0.5f},
            {0.5f,0.5f,-0.5f}, {0.5f,0.5f,0.5f});
    addFace({-1.0f,0.0f,0.0f}, {-0.5f,-0.5f,-0.5f}, {-0.5f,-0.5f,0.5f},
            {-0.5f,0.5f,0.5f}, {-0.5f,0.5f,-0.5f});
    addFace({0.0f, 1.0f, 0.0f}, {-0.5f,0.5f,0.5f}, {0.5f,0.5f,0.5f},
            {0.5f,0.5f,-0.5f}, {-0.5f,0.5f,-0.5f});
    addFace({0.0f,-1.0f,0.0f}, {-0.5f,-0.5f,-0.5f}, {0.5f,-0.5f,-0.5f},
            {0.5f,-0.5f,0.5f}, {-0.5f,-0.5f,0.5f});
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
                                  const std::vector<std::uint32_t>& indices) {
    if (vertices.empty() || indices.empty() || vertices.size() % kVertexStride != 0) return false;

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
    const GLsizei stride = static_cast<GLsizei>(kVertexStride * sizeof(float));
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
                          reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride,
                          reinterpret_cast<void*>(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
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
        !floor_.create(floor.vertices, floor.indices) ||
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

void AvtoVistaScene::render(GLint modelLocation, GLint normalLocation,
                            GLint colorLocation, GLint texturedLocation, GLint paintMaterialLocation,
                const HeadlightUniforms& headlights,
                            const glm::vec3& paintColor, bool lampsEnabled,
                            int carVariant) const {
    const glm::mat4 identity(1.0f);
    const glm::mat4 turntable = glm::rotate(identity, angle_, glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::vec3 lampCenters[] = {{1.285f, 0.49f, -0.34f}, {1.285f, 0.49f, 0.34f}};
    glUniform1i(headlights.enabled, lampsEnabled);
    if (lampsEnabled) {
        float positions[6];
        for (int i = 0; i < 2; ++i) {
            const glm::vec3 lensFront = lampCenters[i] + glm::vec3(0.025f, 0.0f, 0.0f);
            const glm::vec4 world = turntable * glm::vec4(lensFront, 1.0f);
            positions[3 * i] = world.x;
            positions[3 * i + 1] = world.y;
            positions[3 * i + 2] = world.z;
        }
        const glm::vec3 direction = glm::normalize(glm::mat3(turntable) * glm::vec3(1.0f, -0.18f, 0.0f));
        glUniform3fv(headlights.positions, 2, positions);
        glUniform3fv(headlights.direction, 1, glm::value_ptr(direction));
    }

    const auto draw = [&](const Mesh& mesh, const glm::mat4& model, const glm::vec3& color) {
        glUniform1i(texturedLocation, GL_FALSE);
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
        const glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(model));
        glUniformMatrix3fv(normalLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));
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
    const bool alternateCar = carVariant == 1;

    // Textured showroom floor. Keep it separate so the car meshes stay unchanged.
    glUniform1i(paintMaterialLocation, GL_FALSE);
    floorTexture_.bind(0);
    glUniform1i(texturedLocation, GL_TRUE);
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(identity));
    const glm::mat3 floorNormalMatrix(1.0f);
    glUniformMatrix3fv(normalLocation, 1, GL_FALSE, glm::value_ptr(floorNormalMatrix));
    glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(1.0f)));
    floor_.draw();

    // Two simple walls form a backdrop along the rear and left edges.
    draw(cube_, boxTransform({0.0f, 1.32f, -4.86f}, {9.8f, 3.0f, 0.18f}),
         {0.20f, 0.25f, 0.31f});
    draw(cube_, boxTransform({-4.86f, 1.32f, 0.0f}, {0.18f, 3.0f, 9.8f}),
         {0.20f, 0.25f, 0.31f});

    // Three cube-built columns mark the rear and side-wall corners.
    const glm::vec3 columnPositions[] = {
        {-4.25f, 1.32f, -4.65f},
        { 4.25f, 1.32f, -4.65f},
        {-4.65f, 1.32f,  4.25f}
    };
    for (const glm::vec3& columnPosition : columnPositions) {
        draw(cube_, boxTransform(columnPosition, {0.34f, 3.0f, 0.34f}),
             {0.34f, 0.39f, 0.45f});
        draw(cube_, boxTransform({columnPosition.x, -0.08f, columnPosition.z},
                                 {0.54f, 0.20f, 0.54f}),
             {0.22f, 0.28f, 0.34f});
        draw(cube_, boxTransform({columnPosition.x, 2.72f, columnPosition.z},
                                 {0.50f, 0.20f, 0.50f}),
             {0.22f, 0.28f, 0.34f});
    }

    // Compact rotating platform sized to the car footprint.
    draw(cylinder_, turntable * boxTransform({0.0f, -0.06f, 0.0f}, {1.72f, 0.18f, 1.72f}),
         {0.055f, 0.065f, 0.08f});
    draw(cylinder_, turntable * boxTransform({0.0f, 0.035f, 0.0f}, {1.62f, 0.035f, 1.62f}),
         {0.27f, 0.31f, 0.35f});

    // Only the painted body and cabin use the selected finish.
    glUniform1i(paintMaterialLocation, GL_TRUE);
    drawPaintedCarBox({0.0f, alternateCar ? 0.48f : 0.43f, 0.0f},
                      alternateCar ? glm::vec3(2.30f, 0.52f, 1.28f)
                                   : glm::vec3(2.55f, 0.43f, 1.12f));
    drawPaintedCarBox({alternateCar ? -0.10f : -0.18f,
                       alternateCar ? 0.91f : 0.77f, 0.0f},
                      alternateCar ? glm::vec3(1.48f, 0.48f, 1.04f)
                                   : glm::vec3(1.35f, 0.34f, 0.91f));
    glUniform1i(paintMaterialLocation, GL_FALSE);
    if (alternateCar) {
        drawCarBox({-0.10f, 1.18f, -0.45f}, {1.40f, 0.06f, 0.05f},
                   {0.06f, 0.07f, 0.08f});
        drawCarBox({-0.10f, 1.18f, 0.45f}, {1.40f, 0.06f, 0.05f},
                   {0.06f, 0.07f, 0.08f});
    }

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

    // Small door handles on both sides, built from the existing cube mesh.
    for (const float side : {-1.0f, 1.0f}) {
        drawCarBox({-0.42f, 0.52f, side * 0.575f}, {0.18f, 0.035f, 0.025f},
                   {0.58f, 0.62f, 0.66f});
        drawCarBox({0.40f, 0.52f, side * 0.575f}, {0.18f, 0.035f, 0.025f},
                   {0.58f, 0.62f, 0.66f});
    }

    drawCarBox({0.48f, 0.77f, 0.0f}, {0.045f, 0.24f, 0.72f},
               {0.035f, 0.075f, 0.095f}, -0.28f);
    drawCarBox({-0.76f, 0.76f, 0.0f}, {0.045f, 0.22f, 0.72f},
               {0.035f, 0.075f, 0.095f}, 0.30f);

    // Four tires with metallic hubs.
    for (const float side : {-1.0f, 1.0f}) {
        const float wheelPositions[] = {
            alternateCar ? -0.72f : -0.78f,
            alternateCar ? 0.72f : 0.78f
        };
        for (const float x : wheelPositions) {
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

    // A simple front grille with three horizontal slats.
    drawCarBox({1.294f, 0.43f, 0.0f}, {0.035f, 0.14f, 0.32f},
               {0.035f, 0.045f, 0.055f});
    drawCarBox({1.316f, 0.385f, 0.0f}, {0.012f, 0.012f, 0.27f},
               {0.48f, 0.53f, 0.58f});
    drawCarBox({1.316f, 0.43f, 0.0f}, {0.012f, 0.012f, 0.27f},
               {0.48f, 0.53f, 0.58f});
    drawCarBox({1.316f, 0.475f, 0.0f}, {0.012f, 0.012f, 0.27f},
               {0.48f, 0.53f, 0.58f});

    // Headlights, tail lights, and bumpers.
    const glm::vec3 headlightColor = lampsEnabled
        ? glm::vec3(0.98f, 0.96f, 0.82f)
        : glm::vec3(0.12f, 0.12f, 0.10f);
    const glm::vec3 tailLightColor = lampsEnabled
        ? glm::vec3(0.55f, 0.025f, 0.02f)
        : glm::vec3(0.10f, 0.018f, 0.015f);
    for (const auto& center : lampCenters)
        drawCarBox(center, {0.04f, 0.105f, 0.19f}, headlightColor);
    for (const float side : {-1.0f, 1.0f}) {
        drawCarBox({-1.285f, 0.47f, side * 0.35f}, {0.04f, 0.11f, 0.18f},
                   tailLightColor);
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

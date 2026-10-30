//
// Created by zykov on 3/15/2026.
//

#include "MeshPrimitives.h"
#include "../../math/Vertex.h"
#include "../../di/DIContainer.h"
#include "../../math/Vector3.h"

Mesh *MeshPrimitives::CreateBox() {
    Vertex vertices[24] = {
        // === Front (Z-)
        {{-1, -1, -1}, {0, 0, -1}, {0, 0}, {1, 0, 0}, {0, 1, 0}}, // 0
        {{-1, 1, -1}, {0, 0, -1}, {0, 1}, {1, 0, 0}, {0, 1, 0}}, // 1
        {{1, 1, -1}, {0, 0, -1}, {1, 1}, {1, 0, 0}, {0, 1, 0}}, // 2
        {{1, -1, -1}, {0, 0, -1}, {1, 0}, {1, 0, 0}, {0, 1, 0}}, // 3

        // === Right (X+)
        {{1, -1, -1}, {1, 0, 0}, {0, 0}, {0, 0, 1}, {0, 1, 0}}, // 4
        {{1, 1, -1}, {1, 0, 0}, {0, 1}, {0, 0, 1}, {0, 1, 0}}, // 5
        {{1, 1, 1}, {1, 0, 0}, {1, 1}, {0, 0, 1}, {0, 1, 0}}, // 6
        {{1, -1, 1}, {1, 0, 0}, {1, 0}, {0, 0, 1}, {0, 1, 0}}, // 7

        // === Back (Z+)
        {{1, -1, 1}, {0, 0, 1}, {0, 0}, {-1, 0, 0}, {0, 1, 0}}, // 8
        {{1, 1, 1}, {0, 0, 1}, {0, 1}, {-1, 0, 0}, {0, 1, 0}}, // 9
        {{-1, 1, 1}, {0, 0, 1}, {1, 1}, {-1, 0, 0}, {0, 1, 0}}, // 10
        {{-1, -1, 1}, {0, 0, 1}, {1, 0}, {-1, 0, 0}, {0, 1, 0}}, // 11

        // === Left (X-)
        {{-1, -1, 1}, {-1, 0, 0}, {0, 0}, {0, 0, -1}, {0, 1, 0}}, // 12
        {{-1, 1, 1}, {-1, 0, 0}, {0, 1}, {0, 0, -1}, {0, 1, 0}}, // 13
        {{-1, 1, -1}, {-1, 0, 0}, {1, 1}, {0, 0, -1}, {0, 1, 0}}, // 14
        {{-1, -1, -1}, {-1, 0, 0}, {1, 0}, {0, 0, -1}, {0, 1, 0}}, // 15

        // === Top (Y+)
        {{-1, 1, -1}, {0, 1, 0}, {0, 0}, {1, 0, 0}, {0, 0, 1}}, // 16
        {{-1, 1, 1}, {0, 1, 0}, {0, 1}, {1, 0, 0}, {0, 0, 1}}, // 17
        {{1, 1, 1}, {0, 1, 0}, {1, 1}, {1, 0, 0}, {0, 0, 1}}, // 18
        {{1, 1, -1}, {0, 1, 0}, {1, 0}, {1, 0, 0}, {0, 0, 1}}, // 19

        // === Bottom (Y-)
        {{1, -1, 1}, {0, -1, 0}, {0, 0}, {0, 0, -1}, {-1, 0, 0}}, // 20
        {{-1, -1, 1}, {0, -1, 0}, {0, 1}, {0, 0, -1}, {-1, 0, 0}}, // 21
        {{-1, -1, -1}, {0, -1, 0}, {1, 1}, {0, 0, -1}, {-1, 0, 0}}, // 22
        {{1, -1, -1}, {0, -1, 0}, {1, 0}, {0, 0, -1}, {-1, 0, 0}} // 23
    };

    // Vertex vertices[24] = {
    //     // === Front (Z-)
    //     {{0.0f, 0.0f, 0.0f}, {0, 0, -1}, {0, 0}, {1, 0, 0}, {0, 1, 0}}, // 0
    //     {{0.0f, 1.0f, 0.0f}, {0, 0, -1}, {0, 1}, {1, 0, 0}, {0, 1, 0}}, // 1
    //     {{1.0f, 1.0f, 0.0f}, {0, 0, -1}, {1, 1}, {1, 0, 0}, {0, 1, 0}}, // 2
    //     {{1.0f, 0.0f, 0.0f}, {0, 0, -1}, {1, 0}, {1, 0, 0}, {0, 1, 0}}, // 3
    //
    //     // === Right (X+)
    //     {{1.0f, 0.0f, 0.0f}, {1, 0, 0}, {0, 0}, {0, 0, 1}, {0, 1, 0}}, // 4
    //     {{1.0f, 1.0f, 0.0f}, {1, 0, 0}, {0, 1}, {0, 0, 1}, {0, 1, 0}}, // 5
    //     {{1.0f, 1.0f, 1.0f}, {1, 0, 0}, {1, 1}, {0, 0, 1}, {0, 1, 0}}, // 6
    //     {{1.0f, 0.0f, 1.0f}, {1, 0, 0}, {1, 0}, {0, 0, 1}, {0, 1, 0}}, // 7
    //
    //     // === Back (Z+)
    //     {{1.0f, 0.0f, 1.0f}, {0, 0, 1}, {0, 0}, {-1, 0, 0}, {0, 1, 0}}, // 8
    //     {{1.0f, 1.0f, 1.0f}, {0, 0, 1}, {0, 1}, {-1, 0, 0}, {0, 1, 0}}, // 9
    //     {{0.0f, 1.0f, 1.0f}, {0, 0, 1}, {1, 1}, {-1, 0, 0}, {0, 1, 0}}, // 10
    //     {{0.0f, 0.0f, 1.0f}, {0, 0, 1}, {1, 0}, {-1, 0, 0}, {0, 1, 0}}, // 11
    //
    //     // === Left (X-)
    //     {{0.0f, 0.0f, 1.0f}, {-1, 0, 0}, {0, 0}, {0, 0, -1}, {0, 1, 0}}, // 12
    //     {{0.0f, 1.0f, 1.0f}, {-1, 0, 0}, {0, 1}, {0, 0, -1}, {0, 1, 0}}, // 13
    //     {{0.0f, 1.0f, 0.0f}, {-1, 0, 0}, {1, 1}, {0, 0, -1}, {0, 1, 0}}, // 14
    //     {{0.0f, 0.0f, 0.0f}, {-1, 0, 0}, {1, 0}, {0, 0, -1}, {0, 1, 0}}, // 15
    //
    //     // === Top (Y+)
    //     {{0.0f, 1.0f, 0.0f}, {0, 1, 0}, {0, 0}, {1, 0, 0}, {0, 0, 1}}, // 16
    //     {{0.0f, 1.0f, 1.0f}, {0, 1, 0}, {0, 1}, {1, 0, 0}, {0, 0, 1}}, // 17
    //     {{1.0f, 1.0f, 1.0f}, {0, 1, 0}, {1, 1}, {1, 0, 0}, {0, 0, 1}}, // 18
    //     {{1.0f, 1.0f, 0.0f}, {0, 1, 0}, {1, 0}, {1, 0, 0}, {0, 0, 1}}, // 19
    //
    //     // === Bottom (Y-)
    //     {{1.0f, 0.0f, 1.0f}, {0, -1, 0}, {0, 0}, {0, 0, -1}, {-1, 0, 0}}, // 20
    //     {{0.0f, 0.0f, 1.0f}, {0, -1, 0}, {0, 1}, {0, 0, -1}, {-1, 0, 0}}, // 21
    //     {{0.0f, 0.0f, 0.0f}, {0, -1, 0}, {1, 1}, {0, 0, -1}, {-1, 0, 0}}, // 22
    //     {{1.0f, 0.0f, 0.0f}, {0, -1, 0}, {1, 0}, {0, 0, -1}, {-1, 0, 0}}  // 23
    // };


    // Vertex vertices[24] = {
    //     // === Front (Z-)
    //     {{-0.5f, -0.5f, -0.5f}, {0, 0, -1}, {0, 0}, {1, 0, 0}, {0, 1, 0}}, // 0
    //     {{-0.5f, 0.5f, -0.5f}, {0, 0, -1}, {0, 1}, {1, 0, 0}, {0, 1, 0}}, // 1
    //     {{0.5f, 0.5f, -0.5f}, {0, 0, -1}, {1, 1}, {1, 0, 0}, {0, 1, 0}}, // 2
    //     {{0.5f, -0.5f, -0.5f}, {0, 0, -1}, {1, 0}, {1, 0, 0}, {0, 1, 0}}, // 3
    //
    //     // === Right (X+)
    //     {{0.5f, -0.5f, -0.5f}, {1, 0, 0}, {0, 0}, {0, 0, 1}, {0, 1, 0}}, // 4
    //     {{0.5f, 0.5f, -0.5f}, {1, 0, 0}, {0, 1}, {0, 0, 1}, {0, 1, 0}}, // 5
    //     {{0.5f, 0.5f, 0.5f}, {1, 0, 0}, {1, 1}, {0, 0, 1}, {0, 1, 0}}, // 6
    //     {{0.5f, -0.5f, 0.5f}, {1, 0, 0}, {1, 0}, {0, 0, 1}, {0, 1, 0}}, // 7
    //
    //     // === Back (Z+)
    //     {{0.5f, -0.5f, 0.5f}, {0, 0, 1}, {0, 0}, {-1, 0, 0}, {0, 1, 0}}, // 8
    //     {{0.5f, 0.5f, 0.5f}, {0, 0, 1}, {0, 1}, {-1, 0, 0}, {0, 1, 0}}, // 9
    //     {{-0.5f, 0.5f, 0.5f}, {0, 0, 1}, {1, 1}, {-1, 0, 0}, {0, 1, 0}}, // 10
    //     {{-0.5f, -0.5f, 0.5f}, {0, 0, 1}, {1, 0}, {-1, 0, 0}, {0, 1, 0}}, // 11
    //
    //     // === Left (X-)
    //     {{-0.5f, -0.5f, 0.5f}, {-1, 0, 0}, {0, 0}, {0, 0, -1}, {0, 1, 0}}, // 12
    //     {{-0.5f, 0.5f, 0.5f}, {-1, 0, 0}, {0, 1}, {0, 0, -1}, {0, 1, 0}}, // 13
    //     {{-0.5f, 0.5f, -0.5f}, {-1, 0, 0}, {1, 1}, {0, 0, -1}, {0, 1, 0}}, // 14
    //     {{-0.5f, -0.5f, -0.5f}, {-1, 0, 0}, {1, 0}, {0, 0, -1}, {0, 1, 0}}, // 15
    //
    //     // === Top (Y+)
    //     {{-0.5f, 0.5f, -0.5f}, {0, 1, 0}, {0, 0}, {1, 0, 0}, {0, 0, 1}}, // 16
    //     {{-0.5f, 0.5f, 0.5f}, {0, 1, 0}, {0, 1}, {1, 0, 0}, {0, 0, 1}}, // 17
    //     {{0.5f, 0.5f, 0.5f}, {0, 1, 0}, {1, 1}, {1, 0, 0}, {0, 0, 1}}, // 18
    //     {{0.5f, 0.5f, -0.5f}, {0, 1, 0}, {1, 0}, {1, 0, 0}, {0, 0, 1}}, // 19
    //
    //     // === Bottom (Y-)
    //     {{0.5f, -0.5f, 0.5f}, {0, -1, 0}, {0, 0}, {0, 0, -1}, {-1, 0, 0}}, // 20
    //     {{-0.5f, -0.5f, 0.5f}, {0, -1, 0}, {0, 1}, {0, 0, -1}, {-1, 0, 0}}, // 21
    //     {{-0.5f, -0.5f, -0.5f}, {0, -1, 0}, {1, 1}, {0, 0, -1}, {-1, 0, 0}}, // 22
    //     {{0.5f, -0.5f, -0.5f}, {0, -1, 0}, {1, 0}, {0, 0, -1}, {-1, 0, 0}} // 23
    // };

    uint16_t indices[36] = {
        0, 1, 2, 0, 2, 3, // Front
        4, 5, 6, 4, 6, 7, // Right
        8, 9, 10, 8, 10, 11, // Back
        12, 13, 14, 12, 14, 15, // Left
        16, 17, 18, 16, 18, 19, // Top
        20, 21, 22, 20, 22, 23 // Bottom
    };

    const std::vector ver(vertices, vertices + std::size(vertices));
    const std::vector ind(indices, indices + std::size(indices));

    return DIContainer::GetInstance().Get<Rendering::IRenderer>()->CreateMesh(ver, ind);
}

Mesh *MeshPrimitives::CreatePlane() {
    const Vector3 pos1 = {-1.0f, -1.0f, 0.0f};
    const Vector3 pos2 = {-1.0f, 1.0f, 0.0f};
    const Vector3 pos3 = {1.0f, 1.0f, 0.0f};
    const Vector3 pos4 = {1.0f, -1.0f, 0.0f};

    constexpr Vector2 uv1 = {0.0f, 1.0f};
    constexpr Vector2 uv2 = {0.0f, 0.0f};
    constexpr Vector2 uv3 = {1.0f, 0.0f};
    constexpr Vector2 uv4 = {1.0f, 1.0f};

    const Vector3 edge1 = pos2 - pos1;
    const Vector3 edge2 = pos3 - pos1;
    Vector2 deltaUV1 = uv2 - uv1;
    Vector2 deltaUV2 = uv3 - uv1;

    const float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    Vector3 tangent1;
    tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

    Vector3 bitangent1;
    bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

    Vertex vertices[4] = {
        {pos1, {0.0f, 0.0f, -1.0f}, uv1, tangent1, bitangent1},
        {pos2, {0.0f, 0.0f, -1.0f}, uv2, tangent1, bitangent1},
        {pos3, {0.0f, 0.0f, -1.0f}, uv3, tangent1, bitangent1},
        {pos4, {0.0f, 0.0f, -1.0f}, uv4, tangent1, bitangent1}
    };

    uint16_t indices[6] = {
        0, 1, 2,
        0, 2, 3
    };

    const std::vector ver(vertices, vertices + std::size(vertices));
    const std::vector ind(indices, indices + std::size(indices));

    return DIContainer::GetInstance().Get<Rendering::IRenderer>()->CreateMesh(ver, ind);
}

Mesh * MeshPrimitives::CreateSphere(const float radius) {
    std::vector<Vertex> vertices;

    const float PI = std::acos(-1.0f);
    constexpr uint32_t sectors = 32;
    constexpr uint32_t rings = 32;

    constexpr float R = 1.0f / static_cast<float>(sectors);
    constexpr float S = 1.0f / static_cast<float>(rings);

    for (unsigned int r = 0; r <= rings; ++r) {
        const float phi = r * PI * S;

        for (unsigned int s = 0; s <= sectors; ++s) {
            const float theta = s * 2.0f * PI * R;

            float x = std::sin(phi) * std::cos(theta);
            float y = std::cos(phi);
            float z = std::sin(phi) * std::sin(theta);

            Vertex vertex;

            vertex.position = { x * radius, y * radius, z * radius };
            vertex.normal   = { x, y, z };

            vertex.texCoords = { static_cast<float>(s) * R, static_cast<float>(r) * S };

            float tx = -std::sin(theta);
            float ty = 0.0f;
            float tz = std::cos(theta);
            vertex.tangent = { tx, ty, tz };

            vertex.bitangent.x = vertex.normal.y * vertex.tangent.z - vertex.normal.z * vertex.tangent.y;
            vertex.bitangent.y = vertex.normal.z * vertex.tangent.x - vertex.normal.x * vertex.tangent.z;
            vertex.bitangent.z = vertex.normal.x * vertex.tangent.y - vertex.normal.y * vertex.tangent.x;

            vertices.push_back(vertex);
        }
    }

    std::vector<uint16_t> indices;
    indices.reserve(24);

    for (unsigned int r = 0; r < rings; ++r) {
        for (unsigned int s = 0; s < sectors; ++s) {
            const int curRow = r * (sectors + 1);
            const int nextRow = (r + 1) * (sectors + 1);

            indices.push_back(curRow + s);
            indices.push_back(nextRow + s);
            indices.push_back(nextRow + s + 1);

            indices.push_back(curRow + s);
            indices.push_back(nextRow + s + 1);
            indices.push_back(curRow + s + 1);
        }
    }

    Mesh *mesh = DIContainer::GetInstance().Get<Rendering::IRenderer>()->CreateMesh(vertices, indices);

    return mesh;
}

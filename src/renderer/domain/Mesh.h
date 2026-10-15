//
// Created by zykov on 3/15/2026.
//

#ifndef MAXENGINE_MESH_H
#define MAXENGINE_MESH_H


#include <string>
#include <../src/utils/Hash.cpp>
#include "../../math/Vertex.h"
#include "../domain/buffer/BufferHandle.h"

struct MeshVertexBuffer {
    uint32_t offset;
    uint32_t size;
};

struct MeshIndexBuffer {
    uint32_t offset;
    uint32_t size;
};

struct MeshBuffer {
    MeshVertexBuffer vertexBuffer{};
    MeshIndexBuffer indexBuffer{};

    void* vertexData;
    void* indexData;
};

struct StaticMeshes {
    Rendering::BufferHandle vertexBuffer;
    Rendering::BufferHandle indexBuffer;

    int32_t currentVertexOffset;
    int32_t currentIndexOffset;
};


struct MeshId {
    uint64_t Id;

    bool operator==(const MeshId &other) const {
        return Id == other.Id;
    }

    bool operator<(const MeshId &other) const {
        return std::tie(Id) < std::tie(other.Id);
    }
};

template<>
struct std::hash<MeshId> {
    size_t operator()(const MeshId &s) const noexcept {
        return Hash::Generate(std::to_string(s.Id));
    }
};


class Mesh {
public:
    explicit Mesh(const std::vector<Vertex>& vertices, const std::vector<uint16_t>& indices);

    ~Mesh() = default;

    size_t Size;
    Vector3 center{};
    Vector3 position{};

    size_t GetIndexCount() const;
    MeshId GetId() const;
    void SetId(MeshId id);
    std::vector<Vertex>& GetVertices();
    std::vector<uint16_t>& GetIndices();

    Rendering::BufferHandle vertexBuffer{};
    Rendering::BufferHandle indexBuffer{};

    uint32_t indexMesh;

private:
    MeshId id{};
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;

};


#endif //MAXENGINE_MESH_H
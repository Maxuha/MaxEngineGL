//
// Created by zykov on 3/15/2026.
//

#ifndef MAXENGINE_MESH_H
#define MAXENGINE_MESH_H
#include <span>
#include <string>
#include <../src/utils/Hash.cpp>
#include "../../math/Vertex.h"
#include "struct/Buffer.h"

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
    explicit Mesh(const std::span<const Vertex>& vertices, const std::span<const uint32_t>& indices);

    ~Mesh() = default;

    size_t Size;
    Vector3 center{};
    Vector3 position{};

    size_t GetIndexCount() const;
    MeshId GetId() const;
    const std::span<const Vertex>& GetVertices() const;
    const std::span<const uint32_t>& GetIndices() const;

    BufferHandle vertexBuffer{};
    BufferHandle indexBuffer{};

private:
    MeshId id{};
    std::span<const Vertex> vertices;
    std::span<const uint32_t> indices;

};


#endif //MAXENGINE_MESH_H
//
// Created by zykov on 3/15/2026.
//

#ifndef MAXENGINE_MESH_H
#define MAXENGINE_MESH_H
#include <span>
#include <string>
#include <../src/utils/Hash.cpp>
#include "../../math/Vertex.h"
#include "../src/renderer/domain/IBufferManager.h"
#include "../src/renderer/IResourceSetManager.h"

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
    std::vector<Vertex>& GetVertices();
    std::vector<uint16_t>& GetIndices();

    Rendering::BufferHandle vertexBuffer{};
    Rendering::BufferHandle indexBuffer{};
    Rendering::BufferHandle modelBuffer{};
    Rendering::ResourceSetHandle modelResourceSet{};

private:
    MeshId id{};
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;

};


#endif //MAXENGINE_MESH_H
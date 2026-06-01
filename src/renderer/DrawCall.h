//
// Created by zykov on 4/30/2026.
//

#ifndef MAXENGINE_DRAWCALL_H
#define MAXENGINE_DRAWCALL_H
#include <memory>

#include "../graphics/Material.h"
#include "../graphics/Mesh.h"
#include "../graphics/Shader.h"
#include "glm/fwd.hpp"


namespace Rendering {
    struct DrawCall {
        glm::uint64_t key{};
        MeshId meshId;
        MaterialId materialId;
        ShaderId shaderId;
        Matrix4x4 model;
        std::shared_ptr<Mesh> mesh;
        Material *material;
        Transform *transform;
        IShader *shader;
    };
} // namespace Rendering


#endif //MAXENGINE_DRAWCALL_H

//
// Created by zykov on 3/15/2026.
//

#ifndef MAXENGINE_MESHRENDERER_H
#define MAXENGINE_MESHRENDERER_H
#include <memory>

#include "Component.h"
#include "../renderer/domain/Mesh.h"

class Material;
class Camera;

class MeshRenderer : public Component {
public:
    explicit MeshRenderer() = default;

    ~MeshRenderer() override = default;

    std::shared_ptr<Mesh> mesh;

    Material* material{};
};


#endif //MAXENGINE_MESHRENDERER_H
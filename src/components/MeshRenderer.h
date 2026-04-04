//
// Created by zykov on 3/15/2026.
//

#ifndef MAXENGINE_MESHRENDERER_H
#define MAXENGINE_MESHRENDERER_H
#include "Component.h"
#include "../graphics/Material.h"
#include "../graphics/Mesh.h"
#include "../graphics/DefaultMaterial.h"

class Camera;

class MeshRenderer : public Component {
public:
    ~MeshRenderer() = default;

    Mesh* mesh;

    DefaultMaterial* material = nullptr;

    void Draw(const RenderContext &context) const;
};


#endif //MAXENGINE_MESHRENDERER_H
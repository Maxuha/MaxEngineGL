//
// Created by zykov on 3/15/2026.
//

#ifndef MAXENGINE_MESHRENDERER_H
#define MAXENGINE_MESHRENDERER_H
#include "Component.h"
#include "../graphics/Mesh.h"
#include "../graphics/DefaultMaterial.h"

class Camera;

class MeshRenderer : public Component {
public:
    explicit MeshRenderer(GameObject* gameObject) : Component(gameObject) {
    }

    ~MeshRenderer() = default;

    Mesh* mesh;

    Material* material;

    void Draw(std::vector<Light*>& lights, Camera& camera) const;
    void Draw2(std::vector<Light*>& lights, Camera& camera) const;
};


#endif //MAXENGINE_MESHRENDERER_H
//
// Created by zykov on 3/15/2026.
//

#ifndef MAXENGINE_MESHRENDERER_H
#define MAXENGINE_MESHRENDERER_H
#include "Component.h"
#include "../gameObject/light/DirectionalLight.h"
#include "../graphics/Material.h"
#include "../graphics/Mesh.h"
#include "../graphics/SimpleMaterial.h"

class Camera;

class MeshRenderer : public Component {
public:
    MeshRenderer() {
    }

    Mesh mesh;

    SimpleMaterial* material = nullptr;

    std::vector<float> Vertecies;

    unsigned int VBO, VAO;

    void Init();

    void Render(const RenderContext &context) const;

    void Clear() const;
};


#endif //MAXENGINE_MESHRENDERER_H
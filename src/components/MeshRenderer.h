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
    MeshRenderer() = default;

    ~MeshRenderer() {
        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);
    }

    Mesh mesh;

    DefaultMaterial* material = nullptr;

    std::vector<float> Vertex;

    unsigned int VBO{}, VAO{};

    void Init();

    void Render(const RenderContext &context) const;
};


#endif //MAXENGINE_MESHRENDERER_H
//
// Created by zykov on 3/15/2026.
//

#ifndef MAXENGINE_MESHRENDERER_H
#define MAXENGINE_MESHRENDERER_H
#include "Component.h"
#include "glad/glad.h"
#include "../graphics/Mesh.h"

class Camera;

class MeshRenderer : public Component {
public:
    MeshRenderer() {
    }

    Mesh mesh;

    unsigned int VBO, VAO;

    GLuint shaderProgram;

    void Init();

    void Render();

    void Clear() const;
};


#endif //MAXENGINE_MESHRENDERER_H
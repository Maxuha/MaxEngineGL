//
// Created by zykov on 3/15/2026.
//

#ifndef MAXENGINE_CAMERA_H
#define MAXENGINE_CAMERA_H

#include "GameObject.h"
#include "../components/Transform.h"
#include "../math/Matrix4x4.h"
#include "glm/fwd.hpp"
#include "glm/ext/matrix_transform.hpp"

class Camera : public GameObject {
public:
    Camera() {
        auto* transform = AddComponent<Transform>();
        //transform->position = Vector3(0.0f, 0.0f, -2.0f);
    }

    float fov{};
    float near{};
    float far{};
    float aspectRatio{};

    Matrix4x4 ViewMatrix();

    Matrix4x4 ProjectionMatrix() const;

    glm::mat4 ViewProjectionMatrix();
};


#endif //MAXENGINE_CAMERA_H
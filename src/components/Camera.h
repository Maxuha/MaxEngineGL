//
// Created by zykov on 3/15/2026.
//

#ifndef MAXENGINE_CAMERA_H
#define MAXENGINE_CAMERA_H

#include "../gameObject/GameObject.h"
#include "glm/fwd.hpp"
#include "glm/ext/matrix_transform.hpp"

class Camera : public GameObject {
public:
    float fov;
    float near;
    float far;
    float aspectRatio;

    glm::mat4 ViewMatrix();

    glm::mat4 ProjectionMatrix() const;

    glm::mat4 ViewProjectionMatrix();
};


#endif //MAXENGINE_CAMERA_H
//
// Created by zykov on 3/15/2026.
//

#ifndef MAXENGINE_CAMERA_H
#define MAXENGINE_CAMERA_H

#include "GameObject.h"
#include "../components/Transform.h"

class Camera : public GameObject {
public:
    Camera(float fov, float near, float far, float aspectRatio);

    Matrix4x4 GetProjection() const;

private:
    float fov;
    float zNear;
    float zFar;
    float aspectRatio;

    Matrix4x4 projection = Matrix4x4(1);
};


#endif //MAXENGINE_CAMERA_H

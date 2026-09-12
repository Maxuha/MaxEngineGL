//
// Created by zykov on 3/15/2026.
//

#ifndef MAXENGINE_CAMERA_H
#define MAXENGINE_CAMERA_H

#include "GameObject.h"
#include "../components/Transform.h"
#include "../renderer/domain/CameraRender.h"


class Camera : public GameObject {
public:
    Camera(float fov, float zNear, float zFar, float aspectRatio);

    Matrix4x4 GetProjection() const;

    Rendering::CameraHandle GetHandle() const;

private:
    Rendering::CameraHandle  handle;

    float fov;
    float zNear;
    float zFar;
    float aspectRatio;

    Matrix4x4 projection = Matrix4x4(1);
};


#endif //MAXENGINE_CAMERA_H

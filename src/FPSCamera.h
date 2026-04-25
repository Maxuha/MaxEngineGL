//
// Created by zykov on 4/6/2026.
//

#ifndef MAXENGINE_FPSCAMERA_H
#define MAXENGINE_FPSCAMERA_H
#include "gameObject/Camera.h"


class FPSCamera : public Camera {
public:
    FPSCamera(float fov, float near, float far, float aspectRatio);

    ~FPSCamera() override;

    void Start() override;

    void Update(float delta_time) override;

private:
    float cameraSpeed = 0, minCameraSpeed = 2, maxCameraSpeed = 4;
    Vector3 directionRot = Vector3::Zero();
};


#endif //MAXENGINE_FPSCAMERA_H

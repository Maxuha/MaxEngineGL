//
// Created by zykov on 4/6/2026.
//

#ifndef MAXENGINE_FPSCAMERA_H
#define MAXENGINE_FPSCAMERA_H
#include "IKeyListener.h"
#include "gameObject/Camera.h"

class FPSCamera : public Camera, public IKeyListener {
public:
    FPSCamera(float fov, float zNear, float zFar, float aspectRatio);

    ~FPSCamera() override;

    void Start() override;

    void Update(float delta_time) override;

    void OnKeyPressed(int key, int scancode, int action, int mods) override;

private:
    float cameraSpeed = 0, minCameraSpeed = 2, maxCameraSpeed = 10;
    Vector3 directionRot = Vector3::Zero();

    float deltaTime = 0;
};


#endif //MAXENGINE_FPSCAMERA_H

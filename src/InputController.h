//
// Created by zykov on 4/6/2026.
//

#ifndef MAXENGINE_INPUTCONTROLLER_H
#define MAXENGINE_INPUTCONTROLLER_H
#include "GLFW/glfw3.h"


class InputController {
public:
    static InputController& GetInstance();

    void Init(GLFWwindow *window);

    bool GetKeyDown(int key) const;

    void GetCursorPos(double *x, double *y) const;

private:
    explicit InputController() = default;

    GLFWwindow *window;

    static InputController* Instance;
};



#endif //MAXENGINE_INPUTCONTROLLER_H

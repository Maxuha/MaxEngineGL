//
// Created by zykov on 3/17/2026.
//

#ifndef MAXENGINE_WINDOW_H
#define MAXENGINE_WINDOW_H

#include "GLFW/glfw3.h"

class Window {
private:
    GLFWwindow *window{};

public:
    Window(const int width, const int height) {
        this->width = width;
        this->height = height;
        aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    }

    int width, height;
    float aspectRatio;

    void MakeWindow();

    void SwapBuffers() const;

    void DestroyWindow() const;

    bool GetInputKey(int key) const;

    void GetCursorPos(double *x, double *y) const;

    int IsClosed() const;
};


#endif //MAXENGINE_WINDOW_H
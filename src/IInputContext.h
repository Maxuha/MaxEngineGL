//
// Created by zykov on 8/11/2026.
//

#ifndef MAXENGINE_IINPUTCONTEXT_H
#define MAXENGINE_IINPUTCONTEXT_H

#include "../src/math/Vector2.h"


class IInputContext {
    public:
    virtual ~IInputContext() = default;

    virtual bool isKeyPressed(int keycode) = 0;

    virtual void AddKeyListener(std::function<void(int, int, int, int)> callback) = 0;

    virtual Vector2 GetMousePosition() = 0;

    virtual bool GetKeyDown(int keycode) = 0;
};


#endif //MAXENGINE_IINPUTCONTEXT_H

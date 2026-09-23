//
// Created by zykov on 4/6/2026.
//

#ifndef MAXENGINE_INPUTCONTROLLER_H
#define MAXENGINE_INPUTCONTROLLER_H
#include "IInputContext.h"
#include "GLFW/glfw3.h"

class IKeyListener;

class InputController {
public:
    InputController(IInputContext* context);

    void AddKeyListener(IKeyListener* listener);

    void RemoveKeyListener(IKeyListener* listener);

    bool GetKeyDown(int keycode) const;

    Vector2 GetCursorPos() const;

    void Update();

    void KeyPressed(int key, int scancode, int action, int mods) const;

private:
    explicit InputController() = default;

    IInputContext* context;

    static InputController* Instance;

    std::vector<IKeyListener*> keyListeners;
};



#endif //MAXENGINE_INPUTCONTROLLER_H

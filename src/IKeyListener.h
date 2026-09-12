//
// Created by zykov on 8/12/2026.
//

#ifndef MAXENGINE_IKEYLISTENER_H
#define MAXENGINE_IKEYLISTENER_H


class IKeyListener {
public:
    virtual void OnKeyPressed(int key, int scancode, int action, int mods) = 0;
};


#endif //MAXENGINE_IKEYLISTENER_H

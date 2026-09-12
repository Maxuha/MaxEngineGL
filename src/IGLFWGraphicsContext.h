//
// Created by zykov on 6/13/2026.
//

#ifndef MAXENGINE_IGLFWGRAPHICSCONTEXT_H
#define MAXENGINE_IGLFWGRAPHICSCONTEXT_H
#include "IWindow.h"


class IGLFWGraphicsContext {
public:
    virtual ~IGLFWGraphicsContext() = default;

    virtual void Init() = 0;

    virtual void SetContext(IWindow* window) = 0;
};


#endif //MAXENGINE_IGLFWGRAPHICSCONTEXT_H

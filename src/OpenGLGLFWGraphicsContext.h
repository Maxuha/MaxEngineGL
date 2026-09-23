//
// Created by zykov on 6/13/2026.
//

#ifndef MAXENGINE_OPENGLGLFWFGRAPHICSCONTEXT_H
#define MAXENGINE_OPENGLGLFWFGRAPHICSCONTEXT_H
#include "IGLFWGraphicsContext.h"


class IWindow;

class OpenGLGLFWGraphicsContext : public IGLFWGraphicsContext {
public:
    ~OpenGLGLFWGraphicsContext() override;

    void Init() override;
    void SetContext(IWindow* window) override;
};


#endif //MAXENGINE_OPENGLGLFWFGRAPHICSCONTEXT_H

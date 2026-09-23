//
// Created by zykov on 6/15/2026.
//

#ifndef MAXENGINE_OGLGRAPHICSCONTEXT_H
#define MAXENGINE_OGLGRAPHICSCONTEXT_H
#include "../IGraphicsContext.h"

namespace Rendering {
    class OGLGraphicsContext : public IGraphicsContext {
    public:
        OGLGraphicsContext(IWindow *window);

        ~OGLGraphicsContext() override = default;

        virtual void Init() override;

    private:
        IWindow *window;
    };
} // Rendering

#endif //MAXENGINE_OGLGRAPHICSCONTEXT_H

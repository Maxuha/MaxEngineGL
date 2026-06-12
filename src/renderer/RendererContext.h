//
// Created by zykov on 6/8/2026.
//

#ifndef MAXENGINE_RENDERERCONTEXT_H
#define MAXENGINE_RENDERERCONTEXT_H
#include "Renderer.h"

namespace Rendering {
    class RendererContext {
    public:
        explicit RendererContext(IRenderer* renderer) {
            currentRenderer = renderer;
        }

        ~RendererContext() = default;

        static IRenderer* GetContext() {
            assert(currentRenderer != nullptr && "Context is not exist yet");
            return currentRenderer;
        }

        static void SetContext(IRenderer* renderer) {
            currentRenderer = renderer;
        }

    private:
        inline static IRenderer* currentRenderer = nullptr;
    };
} // Rendering

#endif //MAXENGINE_RENDERERCONTEXT_H

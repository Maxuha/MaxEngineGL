//
// Created by zykov on 6/15/2026.
//

#ifndef MAXENGINE_IGRAPHICSCONTEXT_H
#define MAXENGINE_IGRAPHICSCONTEXT_H

namespace Rendering {
    class IGraphicsContext {
        public:
        virtual ~IGraphicsContext() = default;
        virtual void Init() = 0;
    };
} // Rendering

#endif //MAXENGINE_IGRAPHICSCONTEXT_H

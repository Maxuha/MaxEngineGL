//
// Created by zykov on 6/26/2026.
//

#ifndef MAXENGINE_OGLINSTANCE_H
#define MAXENGINE_OGLINSTANCE_H

#include "../../IWindow.h"
#include "../IInstance.h"

namespace Rendering {
    class OGLInstance : public IInstance {
        public:
        explicit OGLInstance(IWindow& window);
        ~OGLInstance() override;
    };
} // Rendering

#endif //MAXENGINE_OGLINSTANCE_H

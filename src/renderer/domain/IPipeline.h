//
// Created by zykov on 5/22/2026.
//

#ifndef MAXENGINE_IPIPELINE_H
#define MAXENGINE_IPIPELINE_H
#include <vector>


namespace Rendering {
    struct ShaderInput;
    struct RenderState;
    class ICommandBuffer;

    class IPipeline {
        public:
        virtual ~IPipeline() = default;
        virtual void Compile(const std::vector<ShaderInput> &shaders, RenderState renderState) = 0;
        virtual void Bind(ICommandBuffer* commandBuffer) const = 0;
        virtual void Unbind() const = 0;
    };
} // Rendering

#endif //MAXENGINE_IPIPELINE_H

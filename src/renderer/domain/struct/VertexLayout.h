//
// Created by zykov on 5/21/2026.
//

#ifndef MAXENGINE_VERTEXLAYOUT_H
#define MAXENGINE_VERTEXLAYOUT_H
#include <vector>


namespace Rendering {
    enum class VertexFormat {
        Float,
        Float2,
        Float3,
        Float4,
        UByte4N,
        Int4
    };

    enum class VertexStepRate {
        PerVertex,
        PerInstance
    };

    struct VertexAttribute {
        uint32_t location = 0;
        VertexFormat format = VertexFormat::Float3;
        uint32_t offset = 0;
        uint32_t bufferIndex = 0;
    };

    struct VertexBinding {
        uint32_t bufferIndex = 0;
        uint32_t stride = 0;
        VertexStepRate stepRate = VertexStepRate::PerVertex;
    };

    struct VertexLayout {
        std::vector<VertexBinding> bindings;
        std::vector<VertexAttribute> attributes;
    };
}


#endif //MAXENGINE_VERTEXLAYOUT_H

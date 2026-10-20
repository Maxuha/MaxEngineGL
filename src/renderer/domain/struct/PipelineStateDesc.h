//
// Created by zykov on 5/21/2026.
//

#ifndef MAXENGINE_PIPELINESTATEDESC_H
#define MAXENGINE_PIPELINESTATEDESC_H

#include "ResourceSet.h"
#include "VertexLayout.h"
#include "../../ITextureManager.h"
#include "../../../math/Rect.h"
#include "../../gl/model/TextureData.h"
#include "../src/math/Color.h"

namespace Rendering {

    enum class RenderFace { Front, Back, Both };
    enum class FillMode { Solid, Wireframe };
    enum class FrontFace { Clockwise, CounterClockwise };
    enum class BlendMode { Opaque, AlphaBlend, Additive };

    enum class AttachmentLoadOp { LOAD, CLEAR, DONT_CARE };

    enum class AttachmentStoreOp { STORE, DONT_CARE };

    enum class AttachmentType : uint32_t {
        NONE = 0,
        COLOR = 1 << 0,
        DEPTH = 1 << 1,
        STENCIL = 1 << 2
    };

    enum class DynamicState : uint32_t {
        VIEWPORT = 0,
        SCISSOR,
        LINE_WIDTH,
        DEPTH_BIAS,
        BLEND_CONSTANTS,
        DEPTH_BOUNDS,
        STENCIL_COMPARE_MASK,
        STENCIL_WRITE_MASK,
        STENCIL_REFERENCE,
    };

    struct DynamicStateDesc {
        std::vector<DynamicState> state;
    };

    struct BlendAttachmentDesc {
        std::vector<BlendMode> blendMode = { BlendMode::Opaque };
    };

    struct DepthStencilDesc {
        bool depthTestEnable = true;
        bool depthWriteEnable = true;
    };

    struct AttachmentDescription {
        TextureFormat format = TextureFormat::SRGBA;
        AttachmentType type = AttachmentType::NONE;
        AttachmentLoadOp loadOp = AttachmentLoadOp::DONT_CARE;
        AttachmentStoreOp storeOp = AttachmentStoreOp::DONT_CARE;
        Color clearColor;
        float depthClearValue = 1.0f;
        std::optional<TextureHandle> renderTarget = { };
    };

    struct RasterizerDesc {
        bool cullEnable = true;
        RenderFace renderFace = RenderFace::Front;
        FillMode fillMode = FillMode::Solid;
        FrontFace frontFace = FrontFace::Clockwise;

        float lineWidth = 1.0f;

        bool depthBiasEnable = false;
        float depthBiasConstantFactor = 0.0f;
        float depthBiasClamp = 0.0f;
        float depthBiasSlopeFactor = 0.0f;

        bool depthClampEnable = false;

        bool rasterizerDiscardEnable = false;
    };

    struct PipelineStateDesc {
        VertexLayout VertexLayout;
        BlendAttachmentDesc BlendAttachmentDesc;
        DynamicStateDesc DynamicState;
        DepthStencilDesc DepthStencilState;
        RasterizerDesc RasterizerState;
        Rect Viewport{};
        TextureFormat ColorFormat = TextureFormat::SRGBA;
        TextureFormat DepthFormat = TextureFormat::DEPTH;
    };

    struct MaterialDesc {
        ResourceSet resourceSet;
    };

} // Rendering

#endif //MAXENGINE_PIPELINESTATEDESC_H

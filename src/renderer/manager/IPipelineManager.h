//
// Created by zykov on 5/22/2026.
//

#ifndef MAXENGINE_IPIPELINEMANAGER_H
#define MAXENGINE_IPIPELINEMANAGER_H
#include <cstring>
#include <memory>
#include <unordered_map>
#include <vector>

#include "../domain/GLPipeline.h"

namespace Rendering {

    struct ShaderInput;
    class IPipeline;

    struct PipelineKey {
        std::vector<ShaderInput> shaders;
        RenderState renderState;

        bool operator==(const PipelineKey &other) const {
            if (shaders.size() != other.shaders.size()) return false;

            for (size_t i = 0; i < shaders.size(); ++i) {
                if (shaders[i].Source != other.shaders[i].Source ||
                    shaders[i].Type != other.shaders[i].Type) {
                    return false;
                }
            }

            return std::memcmp(&renderState, &other.renderState, sizeof(RenderState)) == 0;
        }
    };

    struct PipelineKeyHash {
        std::size_t operator()(const PipelineKey &key) const {
            std::size_t res = 0;

            for (const auto &shader: key.shaders) {
                res ^= std::hash<const char *>()(shader.Source) + 0x9e3779b9 + (res << 6) + (res >> 2);
                res ^= std::hash<uint8_t>()(static_cast<uint8_t>(shader.Type)) + 0x9e3779b9 + (res << 6) + (res >> 2);
            }

            const auto pState = reinterpret_cast<const uint8_t *>(&key.renderState);
            for (size_t i = 0; i < sizeof(RenderState); ++i) {
                res ^= std::hash<uint8_t>()(pState[i]) + 0x9e3779b9 + (res << 6) + (res >> 2);
            }

            return res;
        }
    };

    class IPipelineManager {
    public:
        virtual ~IPipelineManager() = default;

        virtual IPipeline *GetOrCreatePipeline(const std::vector<ShaderInput> &shaders, const RenderState &renderState) = 0;

        virtual void Clear() = 0;
    };
} // Rendering

#endif //MAXENGINE_IPIPELINEMANAGER_H

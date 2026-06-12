//
// Created by zykov on 5/27/2026.
//

#ifndef MAXENGINE_BUFFER_H
#define MAXENGINE_BUFFER_H
#include "BufferHandle.h"
#include "IBuffer.h"

namespace Rendering {
    enum class BufferUsage;

    class Buffer : public IBuffer {
    public:
        Buffer() = default;

        Buffer(const BufferHandle &buffer_id, const BufferUsage usage)
            : bufferId(buffer_id),
              usage(usage) {
        }

        ~Buffer() override = default;

        BufferHandle GetId() override {
            return bufferId;
        }

        void UpdateData(void* data) override;

    private:
        BufferHandle bufferId{};
        BufferUsage usage{};
    };
} // Rendering

#endif //MAXENGINE_BUFFER_H

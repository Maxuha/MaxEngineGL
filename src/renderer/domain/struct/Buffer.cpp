//
// Created by zykov on 5/27/2026.
//

#include "Buffer.h"

#include "../../../di/DIContainer.h"

namespace Rendering {
    void Buffer::UpdateData(void* data) {
        DIContainer::GetInstance().Get<IRenderDevice>()->UpdateBuffer(bufferId, data, 0, 0);
    }
} // Rendering
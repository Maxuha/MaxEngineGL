//
// Created by zykov on 5/27/2026.
//

#include "Buffer.h"

#include "../../RendererContext.h"


namespace Rendering {
    void Buffer::UpdateData(void* data) {
        RendererContext::GetContext()->UpdateBufferData(bufferId, 0, 0, data);
    }
} // Rendering
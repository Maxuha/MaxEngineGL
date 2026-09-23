//
// Created by zykov on 6/26/2026.
//

#ifndef MAXENGINE_IPHYSICALDEVICE_H
#define MAXENGINE_IPHYSICALDEVICE_H

namespace Rendering {
    struct QueueFamilyIndices;

    class IPhysicalDevice {
    public:
        virtual ~IPhysicalDevice() = default;

        virtual QueueFamilyIndices GetQueueFamilyIndices() = 0;
    };
} // Rendering

#endif //MAXENGINE_IPHYSICALDEVICE_H

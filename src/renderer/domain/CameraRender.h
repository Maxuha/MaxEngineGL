//
// Created by zykov on 7/25/2026.
//

#ifndef MAXENGINE_CAMERARENDER_H
#define MAXENGINE_CAMERARENDER_H

namespace Rendering {
    struct CameraHandle {
        uint32_t Id;
    };

    class CameraRender {
    public:
        CameraRender();
        ~CameraRender();

        CameraHandle handle ;
    };
} // Rendering

#endif //MAXENGINE_CAMERARENDER_H

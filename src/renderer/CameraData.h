//
// Created by zykov on 5/2/2026.
//

#ifndef MAXENGINE_CAMERADATA_H
#define MAXENGINE_CAMERADATA_H
#include "../math/Matrix4x4.h"


namespace Rendering {

struct CameraData {
    Matrix4x4 view;
    Matrix4x4 projection;
    Vector4 position;
};

} // namespace Rendering


#endif //MAXENGINE_CAMERADATA_H

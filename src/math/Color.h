//
// Created by zykov on 3/22/2026.
//

#ifndef MAXENGINE_COLOR_H
#define MAXENGINE_COLOR_H
#include "Vector3.h"


class Color {
public:
    Color();

    Color(float r, float g, float b, float a);

    float r, g, b, a;

    Vector3 rgb() const;
};


#endif //MAXENGINE_COLOR_H

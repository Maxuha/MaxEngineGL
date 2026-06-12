//
// Created by zykov on 3/22/2026.
//

#include "Color.h"
#include "Vector3.h"

Color::Color() {
    r = 1.0f;
    g = 1.0f;
    b = 1.0f;
    a = 1.0f;
}

Color::Color(const float r, const float g, const float b, const float a = 1.0f) : r(r), g(g), b(b), a(a) {
}

Vector3 Color::rgb() const {
    return {r, g, b};
}

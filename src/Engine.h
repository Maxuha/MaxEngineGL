//
// Created by zykov on 4/4/2026.
//

#ifndef MAXENGINE_ENGINE_H
#define MAXENGINE_ENGINE_H
#include <string>

#include "glad/glad.h"


class Engine {
public:
    void Start();

    static void SaveDepthTextureToFile(GLuint textureId, int width, int height, const std::string& filename);
};

#endif //MAXENGINE_ENGINE_H

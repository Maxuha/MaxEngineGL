//
// Created by zykov on 4/8/2026.
//

#ifndef MAXENGINE_SHADERIMPORTER_H
#define MAXENGINE_SHADERIMPORTER_H
#include <string>

#include "IShaderImporter.h"


class Shader;

class ShaderImporter : public IShaderImporter {
public:
    ~ShaderImporter() override;
    Shader *Import(const std::string &shaderName) override;

    Shader * Import(const Vert &vert, const Frag &frag) override;
};


#endif //MAXENGINE_SHADERIMPORTER_H

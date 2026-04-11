//
// Created by zykov on 4/8/2026.
//

#ifndef MAXENGINE_SHADERIMPORTER_H
#define MAXENGINE_SHADERIMPORTER_H
#include <string>


class Shader;

class ShaderImporter {
    public:
    Shader* Import(std::string shaderName);

};



#endif //MAXENGINE_SHADERIMPORTER_H

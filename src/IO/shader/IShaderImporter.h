//
// Created by zykov on 6/2/2026.
//

#ifndef MAXENGINE_ISHADERIMPORTER_H
#define MAXENGINE_ISHADERIMPORTER_H
#include <string>


class Shader;

using Vert = std::string;
using Frag = std::string;

class IShaderImporter {
public:
    virtual ~IShaderImporter() = default;

    virtual Shader* Import(const Vert &vert, const Frag &frag) = 0;

    virtual Shader* Import(const std::string &shaderName) = 0;
};


#endif //MAXENGINE_ISHADERIMPORTER_H

//
// Created by zykov on 3/20/2026.
//

#ifndef MAXENGINE_SIMPLEMATERIAL_H
#define MAXENGINE_SIMPLEMATERIAL_H
#include "Material.h"
#include "../math/Color.h"

class DefaultMaterial: public Material {

public:
    explicit DefaultMaterial(Shader* shader);

    Color color{};

    void Apply() override;

};


#endif //MAXENGINE_SIMPLEMATERIAL_H
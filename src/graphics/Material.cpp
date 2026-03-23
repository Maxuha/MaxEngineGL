//
// Created by zykov on 3/19/2026.
//

#include "Material.h"

#include "RenderContext.h"
#include "glm/gtc/type_ptr.hpp"

void Material::Apply() {

}

void Material::compileShader() const {
    shader->compileShader();
}

//
// Created by zykov on 4/26/2026.
//

#ifndef MAXENGINE_ASSIMPIMPORTER_H
#define MAXENGINE_ASSIMPIMPORTER_H
#include "ModelImporter.h"
#include "../graphics/Texture.h"
#include "assimp/mesh.h"
#include "assimp/scene.h"
#include "assimp/matrix4x4.h"


namespace Assimp::OpenGEX {
    struct Texture;
}

class AssimpImporter : public ModelImporter {
public:
    Model* Import(const char* fileName) override;

private:
    std::vector<Texture> textures_loaded;
    std::vector<MeshEntry> meshes;
    std::string directory;
    bool gammaCorrection;

    void ProcessNode(aiNode* node, const aiScene* scene, aiMatrix4x4 parentTransform);
    Mesh* ProcessMesh(aiMesh* mesh, const aiScene* scene);
};


#endif //MAXENGINE_ASSIMPIMPORTER_H

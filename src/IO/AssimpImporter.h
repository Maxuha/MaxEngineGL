//
// Created by zykov on 4/26/2026.
//

#ifndef MAXENGINE_ASSIMPIMPORTER_H
#define MAXENGINE_ASSIMPIMPORTER_H
#include "ModelImporter.h"
#include "../renderer/domain/Material.h"

// Forward declare Assimp types to avoid pulling in heavy headers
struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;
namespace Assimp { class Importer; }

class AssimpImporter : public ModelImporter {
public:
    AssimpImporter();
    ~AssimpImporter() override;

    Model* Import(const char* fileName) override;

private:
    std::unique_ptr<Assimp::Importer> importer;

    enum class ValueType { Float, Vector4 };

    struct AssimpKey {
        const char* name;
        unsigned int type;
        unsigned int index;
        ValueType valueType;
    };

    std::vector<Rendering::IGLTexture*> textures_loaded;
    std::vector<MeshEntry> meshes;
    std::string directory;

    std::unordered_map<ShaderProperty, AssimpKey> materialProperties;

    void ProcessNode(const aiNode* node, const aiScene* scene, GameObject* parent, Model* model);
    Mesh* ProcessMesh(const aiMesh* mesh, const aiScene* scene);
    void ProcessMaterial(const aiMaterial* mat, Model* model);
    Material *ProcessMaterialTexture(const aiMaterial* mat);
};


#endif //MAXENGINE_ASSIMPIMPORTER_H

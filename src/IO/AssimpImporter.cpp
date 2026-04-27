//
// Created by zykov on 4/26/2026.
//


#define GLM_ENABLE_EXPERIMENTAL

#include "AssimpImporter.h"
#include <iostream>
#include "../graphics/Texture.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtc/quaternion.hpp"

namespace Assimp::OpenGEX {
    struct Texture;
}

Model* AssimpImporter::Import(const char *fileName) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return {};
    }

    directory = std::string(fileName).substr(0, std::string(fileName).find_last_of('/'));
    ProcessNode(scene->mRootNode, scene, aiMatrix4x4());

    auto* model = new Model;
    model->meshes = std::move(meshes);
    return model;
}

void AssimpImporter::ProcessNode(aiNode *node, const aiScene *scene, aiMatrix4x4 parentTransform) {
    aiMatrix4x4 globalTransform = parentTransform * node->mTransformation;

    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];

        aiVector3D aiScale, aiPosition;
        aiQuaternion aiRotation;
        globalTransform.Decompose(aiScale, aiRotation, aiPosition);

        glm::quat q(aiRotation.w, aiRotation.x, aiRotation.y, aiRotation.z);
        glm::vec3 euler = glm::degrees(glm::eulerAngles(q));

        MeshEntry entry;
        entry.mesh = ProcessMesh(mesh, scene);
        entry.position = Vector3(aiPosition.x, aiPosition.y, aiPosition.z);
        entry.rotation = Vector3(euler.x, euler.y, euler.z);
        entry.scale    = Vector3(aiScale.x, aiScale.y, aiScale.z);
        meshes.push_back(entry);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        ProcessNode(node->mChildren[i], scene, globalTransform);
    }
}

Mesh* AssimpImporter::ProcessMesh(aiMesh *mesh, const aiScene *scene) {
    // data to fill
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // walk through each of the mesh's vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        Vector3 vector;
        // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.position = vector;
        // normals
        if (mesh->HasNormals()) {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.normal = vector;
        }
        // texture coordinates
        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            Vector2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.texCoords = vec;
            // tangent
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.tangent = vector;
            // bitangent
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.bitangent = vector;
        } else
            vertex.texCoords = Vector2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }
    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // return a mesh object created from the extracted mesh data
    return new Mesh(vertices, indices);
}



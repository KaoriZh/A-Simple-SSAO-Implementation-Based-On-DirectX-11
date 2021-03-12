#include "ModelImporter.h"

bool ModelImporter::Import(const std::string& path, Model& model) {
    auto scene = importer.ReadFile(path, 
        aiProcess_ConvertToLeftHanded | 
        aiProcess_Triangulate | 
        aiProcess_GenSmoothNormals | 
        aiProcess_GenUVCoords);
    if (!scene) return false;
    scene = importer.GetOrphanedScene();
    LoadMesh(scene, scene->mRootNode, model);
    return true;
}

void ModelImporter::LoadMesh(const aiScene* scene, const aiNode* node, Model& model) {
    for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
        auto mesh = scene->mMeshes[node->mMeshes[i]];
        LoadMesh(mesh, model);
    }
    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        LoadMesh(scene, node->mChildren[i], model);
    }
}

void ModelImporter::LoadMesh(const aiMesh* mesh, Model& model) {
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        // load vertices

    }
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        // load indices
    }
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        // load normals
    }
}

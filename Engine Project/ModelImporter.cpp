#include "ModelImporter.h"

using namespace DirectX;

bool ModelImporter::Import(const std::string& path) {
    auto scene = importer.ReadFile(path,
        aiProcess_ConvertToLeftHanded |
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        aiProcess_GenUVCoords);
    if (!scene) return false;
    scene = importer.GetOrphanedScene();
    objParts.clear();
    LoadMesh(scene, scene->mRootNode);
    LoadTexture(scene);
    return true;
}

void ModelImporter::LoadMesh(const aiScene* scene, const aiNode* node) {
    // load submesh
    for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
        auto aimesh = scene->mMeshes[node->mMeshes[i]];
        LoadMesh(aimesh);
    }
    // load recursively
    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        LoadMesh(scene, node->mChildren[i]);
    }
}

void ModelImporter::LoadMesh(const aiMesh* aimesh) {
    
    ObjPart objPart;
    // load vertices
    for (unsigned int i = 0; i < aimesh->mNumVertices; ++i) {
        VertexPosNormalTex vertex;
        auto v = aimesh->mVertices[i];
        vertex.pos = XMFLOAT3(v.x, v.y, v.z);
        vMin.x = std::min(vMin.x, v.x);
        vMin.y = std::min(vMin.y, v.y);
        vMin.z = std::min(vMin.z, v.z);
        vMax.x = std::max(vMax.x, v.x);
        vMax.y = std::max(vMax.y, v.y);
        vMax.z = std::max(vMax.z, v.z);
        auto n = aimesh->mNormals[i];
        vertex.normal = XMFLOAT3(n.x, n.y, n.z);
        if (aimesh->mTextureCoords[0]) {
            auto texcoord = aimesh->mTextureCoords[0][i];
            vertex.tex = XMFLOAT2((float)texcoord.x, (float)texcoord.y);
        } else {
            vertex.tex = XMFLOAT2(0, 0);
        }
        objPart.vertices.emplace_back(vertex);
    }
    // load indices
    for (unsigned int i = 0; i < aimesh->mNumFaces; ++i) {
        auto& face = aimesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j) {
            objPart.indices.push_back((DWORD)face.mIndices[j]);
        }
    }

    // set default material
    objPart.material.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
    objPart.material.diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
    objPart.material.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

    objParts.emplace_back(objPart);
}

void ModelImporter::LoadTexture(const aiScene* scene) {

    for (auto objPart : objParts) {
        objPart.texStrDiffuse = L"..\\Model\\house.png";
    }
    /*
    if (scene->HasTextures()) {
        for (unsigned int i = 0; i < scene->mNumTextures; ++i) {
            // load textures
        }
    } else {
        // load default texture
    }
    */
}

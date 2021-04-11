#include "ModelImporter.h"

using namespace DirectX;

bool ModelImporter::Import(const std::string& path, bool importAllInOnePart) {
    auto scene = importer.ReadFile(path,
        aiProcess_ConvertToLeftHanded |
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        aiProcess_GenUVCoords);
    if (!scene) return false;
    scene = importer.GetOrphanedScene();
    objParts.clear();
    vMin = vMax = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
    m_ImportAllInOnePart = importAllInOnePart;
    if (importAllInOnePart) {
        objParts.push_back({});
    }
    LoadMesh(scene);
    if (importAllInOnePart) {
        LoadDefaultTexture(scene);
    }
    return true;
}

void ModelImporter::LoadMesh(const aiScene* scene) {
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        LoadMesh(scene->mMeshes[i]);
    }
}

void ModelImporter::LoadMesh(const aiMesh* aimesh) {
    
    if (!m_ImportAllInOnePart) {
        objParts.push_back({});
    }
    auto& objPart = objParts.back();
    // load vertices
    auto vertexCount = objPart.vertices.size();
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
            objPart.indices.push_back((DWORD)(face.mIndices[j] + vertexCount));
        }
    }
    // set default material
    objPart.material = DefaultMaterial;
}

void ModelImporter::LoadDefaultTexture(const aiScene* scene) {

    for (auto objPart : objParts) {
        objPart.texStrDiffuse = L"Asset\\Model\\default.png";
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

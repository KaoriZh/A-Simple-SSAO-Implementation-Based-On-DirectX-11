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
    // load submesh
    for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
        auto aimesh = scene->mMeshes[node->mMeshes[i]];
        LoadMesh(aimesh, model);
    }
    // load recursively
    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        LoadMesh(scene, node->mChildren[i], model);
    }
}

void ModelImporter::LoadMesh(const aiMesh* aimesh, Model& model) {
    Mesh mesh;
    // load vertices
    for (unsigned int i = 0; i < aimesh->mNumVertices; ++i) {
        Vertex vertex;
        vertex.pos = XMFLOAT3(aimesh->mVertices[i].x,
                              aimesh->mVertices[i].y,
                              aimesh->mVertices[i].z);
        vertex.normal = XMFLOAT3(aimesh->mNormals[i].x,
                                 aimesh->mNormals[i].y,
                                 aimesh->mNormals[i].z);
        if (aimesh->HasVertexColors(0)) {
            vertex.color = XMFLOAT4(aimesh->mColors[0][i].r,
                                    aimesh->mColors[0][i].g,
                                    aimesh->mColors[0][i].b,
                                    aimesh->mColors[0][i].a);
        } else {
            vertex.color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
        }
        mesh.vertices.emplace_back(vertex);
    }
    // load indices
    for (unsigned int i = 0; i < aimesh->mNumFaces; ++i) {
        auto& face = aimesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j) {
            mesh.indices.push_back((DWORD)face.mIndices[j]);
        }
    }
    model.meshes.emplace_back(mesh);
}

void ModelImporter::LoadTexture(const aiScene* scene, Model& model) {
    if (scene->HasTextures()) {
        for (unsigned int i = 0; i < scene->mNumTextures; ++i) {
            // load textures
        }
    } else {
        // load default texture
    }
}

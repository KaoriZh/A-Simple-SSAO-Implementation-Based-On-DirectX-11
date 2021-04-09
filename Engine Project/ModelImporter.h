#pragma once

#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Vertex.h"
#include "LightHelper.h"

class ModelImporter {
public:
	struct ObjPart {
		ObjPart() : material() {}
		~ObjPart() = default;

		Material material;
		std::vector<VertexPosNormalTex> vertices;
		std::vector<DWORD> indices;
		std::wstring texStrDiffuse;
	};

	ModelImporter() : importer(), vMin(), vMax() {}
	ModelImporter(const ModelImporter&) = delete;
	ModelImporter& operator=(const ModelImporter&) = delete;
	ModelImporter(ModelImporter&&) = delete;
	ModelImporter& operator=(ModelImporter&&) = delete;
	~ModelImporter() {}


	// 根据path导入模型，若导入成功则返回true
	bool Import(const std::string& path);

public:
	std::vector<ObjPart> objParts;
	DirectX::XMFLOAT3 vMin, vMax;

private:
	void LoadMesh(const aiScene* scene, const aiNode* node);
	void LoadMesh(const aiMesh* mesh);
	void LoadTexture(const aiScene* scene);

private:
	Assimp::Importer importer;
};
#pragma once

#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Model.h"

class ModelImporter {
public:
	ModelImporter(): importer() {}
	ModelImporter(const ModelImporter&) = delete;
	ModelImporter& operator=(const ModelImporter&) = delete;
	ModelImporter(ModelImporter&&) = delete;
	ModelImporter& operator=(ModelImporter&&) = delete;
	~ModelImporter() {}


	// 根据path导入模型，若导入成功则返回true，以及模型model
	bool Import(const std::string& path, Model& model);

private:
	void LoadMesh(const aiScene* scene, const aiNode* node, Model& model);
	void LoadMesh(const aiMesh* mesh, Model& model);
	void LoadTexture(const aiScene* scene, Model& model);

private:
	Assimp::Importer importer;
};
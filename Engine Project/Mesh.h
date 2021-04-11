#pragma once
#include <DirectXCollision.h>
#include "Effects.h"
#include "ModelImporter.h"
#include "Geometry.h"

struct Mesh {
	// 使用模板别名(C++11)简化类型名
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	Mesh() = default;

	Mesh(const Mesh&) = default;
	Mesh& operator=(const Mesh&) = default;

	Mesh(Mesh&&) = default;
	Mesh& operator=(Mesh&&) = default;

	Material material;
	ComPtr<ID3D11ShaderResourceView> texDiffuse;
	ComPtr<ID3D11ShaderResourceView> texNormalMap;
	ComPtr<ID3D11Buffer> vertexBuffer;
	ComPtr<ID3D11Buffer> indexBuffer;
	UINT vertexCount;
	UINT indexCount;
	DXGI_FORMAT indexFormat;
};
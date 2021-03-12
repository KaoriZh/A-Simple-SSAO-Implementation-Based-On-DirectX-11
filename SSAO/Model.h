#pragma once

#include <wrl/client.h>
#include <d3d11_1.h>
#include <DirectXMath.h>
#include <string>
#include "ConstantBuffer.h"
#include "Mesh.h"

using namespace DirectX;

struct Model {
	void SetDebugObjectName(const std::string& name);

	ConstantBuffer<ConstantBufferContent>* constantBuffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture = nullptr;

	std::vector<Mesh> meshes;
	XMFLOAT3 position;
	XMFLOAT3 rotation;
	XMFLOAT3 scale;
};
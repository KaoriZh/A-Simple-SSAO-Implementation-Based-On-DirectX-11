#pragma once

#include <d3d11_1.h>
#include <DirectXMath.h>
#include "Light.h"
#include "Material.h"

struct VSConstantBuffer {
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX proj;
	DirectX::XMMATRIX worldInvTranspose;

};

struct PSConstantBuffer {
	DirectionalLight dirLight;
	PointLight pointLight;
	SpotLight spotLight;
	Material material;
	DirectX::XMFLOAT4 eyePos;
};
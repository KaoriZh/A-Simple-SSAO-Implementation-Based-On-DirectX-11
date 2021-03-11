#pragma once

#include <d3d11_1.h>
#include <DirectXMath.h>

struct ConstantBuffer {
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX proj;
};
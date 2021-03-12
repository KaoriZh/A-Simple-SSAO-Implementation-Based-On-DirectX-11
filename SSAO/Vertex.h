#pragma once

#include <d3d11_1.h>
#include <DirectXMath.h>

struct VertexPosNormalColor {
	VertexPosNormalColor() = default;

	VertexPosNormalColor(const VertexPosNormalColor&) = default;
	VertexPosNormalColor& operator=(const VertexPosNormalColor&) = default;

	VertexPosNormalColor(VertexPosNormalColor&&) = default;
	VertexPosNormalColor& operator=(VertexPosNormalColor&&) = default;

	constexpr VertexPosNormalColor(const DirectX::XMFLOAT3& _pos, const DirectX::XMFLOAT3& _normal,
		const DirectX::XMFLOAT4& _color) :
		pos(_pos), normal(_normal), color(_color) {}

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT4 color;
	static const D3D11_INPUT_ELEMENT_DESC inputLayout[3];
};

struct Vertex {
	Vertex() {}
	Vertex(float x, float y, float z, float u, float v)
		: pos(x, y, z), texCoord(u, v) {}

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 texCoord;
};
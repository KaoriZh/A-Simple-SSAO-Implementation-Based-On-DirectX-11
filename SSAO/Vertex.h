#pragma once

#include <d3d11_1.h>
#include <DirectXMath.h>

struct Vertex {
	Vertex() = default;

	Vertex(const Vertex&) = default;
	Vertex& operator=(const Vertex&) = default;

	Vertex(Vertex&&) = default;
	Vertex& operator=(Vertex&&) = default;

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT4 color;
	static const D3D11_INPUT_ELEMENT_DESC inputLayout[3];
};

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

struct VertexPosNormalTex {
	VertexPosNormalTex() = default;

	VertexPosNormalTex(const VertexPosNormalTex&) = default;
	VertexPosNormalTex& operator=(const VertexPosNormalTex&) = default;

	VertexPosNormalTex(VertexPosNormalTex&&) = default;
	VertexPosNormalTex& operator=(VertexPosNormalTex&&) = default;

	constexpr VertexPosNormalTex(const DirectX::XMFLOAT3& _pos, const DirectX::XMFLOAT3& _normal,
		const DirectX::XMFLOAT2& _tex) :
		pos(_pos), normal(_normal), tex(_tex) {}

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 tex;
	static const D3D11_INPUT_ELEMENT_DESC inputLayout[3];
};
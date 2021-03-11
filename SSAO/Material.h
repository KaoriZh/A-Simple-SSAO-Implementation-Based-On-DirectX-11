#pragma once

// 虽然是CPU端的渲染数据抽象，但同时也是需要被打包到GPU的内容，因此要注意打包规则
#include <cstring>
#include <DirectXMath.h>

struct Material {
	Material() = default;

	Material(const Material&) = default;
	Material& operator=(const Material&) = default;

	Material(Material&&) = default;
	Material& operator=(Material&&) = default;

	Material(const DirectX::XMFLOAT4& _ambient, const DirectX::XMFLOAT4& _diffuse, const DirectX::XMFLOAT4& _specular,
		const DirectX::XMFLOAT4& _reflect) :
		ambient(_ambient), diffuse(_diffuse), specular(_specular), reflect(_reflect) {}

	DirectX::XMFLOAT4 ambient;
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 specular; // w = 镜面反射强度
	DirectX::XMFLOAT4 reflect;
};
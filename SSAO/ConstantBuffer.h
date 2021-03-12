#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include "DXTrace.h"

template<class T>
class ConstantBuffer {
public:
	ConstantBuffer() {}
	ConstantBuffer(const ConstantBuffer<T>& rhs) = delete;

	ID3D11Buffer* Get() const;

	ID3D11Buffer* const* GetAddressOf() const;

	HRESULT Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	bool Apply();

public:
	T data;

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	ID3D11DeviceContext* deviceContext = nullptr;
};

struct ConstantBufferContent {

};
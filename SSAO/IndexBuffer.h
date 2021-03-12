#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <vector>

class IndexBuffer {
public:
	IndexBuffer() {}
	IndexBuffer(const IndexBuffer&) = delete;

	ID3D11Buffer* Get() const;

	ID3D11Buffer* const* GetAddressOf() const;

	UINT BufferSize() const;

	HRESULT Initialize(ID3D11Device* device, DWORD* data, UINT numIndices);

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	UINT bufferSize = 0;
};
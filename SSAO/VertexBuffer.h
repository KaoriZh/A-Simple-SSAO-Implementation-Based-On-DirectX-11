#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <memory>

template<class T>
class VertexBuffer {

public:
	VertexBuffer() {}
	VertexBuffer(const VertexBuffer<T>& rhs);
	VertexBuffer<T>& operator=(const VertexBuffer<T>& a);

	ID3D11Buffer* Get() const;
	ID3D11Buffer* const* GetAddressOf() const;
	UINT BufferSize() const;

	const UINT Stride() const;
	const UINT* StridePtr() const;

	HRESULT Initialize(ID3D11Device* device, T* data, UINT numVertices);

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	std::shared_ptr<UINT> stride;
	UINT bufferSize = 0;
};
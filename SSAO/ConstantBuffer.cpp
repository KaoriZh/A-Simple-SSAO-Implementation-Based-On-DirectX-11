#include "ConstantBuffer.h"

template<class T>
ID3D11Buffer* ConstantBuffer<T>::Get() const {
	return buffer.Get();
}

template<class T>
ID3D11Buffer* const* ConstantBuffer<T>::GetAddressOf() const {
	return buffer.GetAddressOf();
}

template<class T>
HRESULT ConstantBuffer<T>::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext) {
	if (buffer.Get() != nullptr)
		buffer.Reset();

	this->deviceContext = deviceContext;

	D3D11_BUFFER_DESC desc;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.ByteWidth = static_cast<UINT>(sizeof(T) + (16 - (sizeof(T) % 16)));
	desc.StructureByteStride = 0;

	HRESULT hr = device->CreateBuffer(&desc, 0, buffer.GetAddressOf());
	return hr;
}

template<class T>
bool ConstantBuffer<T>::Apply() {
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HR(this->deviceContext->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
	CopyMemory(mappedResource.pData, &data, sizeof(T));
	this->deviceContext->Unmap(buffer.Get(), 0);
	return true;
}
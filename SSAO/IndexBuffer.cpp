#include "IndexBuffer.h"

ID3D11Buffer* IndexBuffer::Get()const {
	return buffer.Get();
}

ID3D11Buffer* const* IndexBuffer::GetAddressOf()const {
	return buffer.GetAddressOf();
}

UINT IndexBuffer::BufferSize() const {
	return this->bufferSize;
}

HRESULT IndexBuffer::Initialize(ID3D11Device* device, DWORD* data, UINT numIndices) {
	if (buffer.Get() != nullptr)
		buffer.Reset();

	this->bufferSize = numIndices;
	//Load Index Data
	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * numIndices;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA indexBufferData;
	indexBufferData.pSysMem = data;
	HRESULT hr = device->CreateBuffer(&indexBufferDesc, &indexBufferData, buffer.GetAddressOf());
	return hr;
}
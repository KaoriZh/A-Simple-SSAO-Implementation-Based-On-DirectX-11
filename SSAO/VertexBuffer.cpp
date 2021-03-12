#include "VertexBuffer.h"

template<class T>
VertexBuffer<T>::VertexBuffer(const VertexBuffer<T>& rhs) :
	buffer(rhs.buffer),
	bufferSize(rhs.bufferSize),
	stride(rhs.stride) {

}

template<class T>
VertexBuffer<T>& VertexBuffer<T>::operator=(const VertexBuffer<T>& a) {
	this->buffer = a.buffer;
	this->bufferSize = a.bufferSize;
	this->stride = a.stride;
	return *this;
}

template<class T>
ID3D11Buffer* VertexBuffer<T>::Get() const {
	return buffer.Get();
}

template<class T>
ID3D11Buffer* const* VertexBuffer<T>::GetAddressOf() const {
	return buffer.GetAddressOf();
}

template<class T>
UINT VertexBuffer<T>::BufferSize() const {
	return this->bufferSize;
}

template<class T>
const UINT VertexBuffer<T>::Stride() const {
	return *this->stride.get();
}

template<class T>
const UINT* VertexBuffer<T>::StridePtr() const {
	return this->stride.get();
}

template<class T>
HRESULT VertexBuffer<T>::Initialize(ID3D11Device* device, T* data, UINT numVertices) {
	if (buffer.Get() != nullptr)
		buffer.Reset();

	this->bufferSize = numVertices;
	if (this->stride.get() == nullptr)
		this->stride = std::make_shared<UINT>(sizeof(T));

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(T) * numVertices;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;
	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = data;

	HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, this->buffer.GetAddressOf());
	return hr;
}
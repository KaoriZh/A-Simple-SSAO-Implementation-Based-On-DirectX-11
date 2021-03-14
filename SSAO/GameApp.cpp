// Impl

#include "GameApp.h"
#include "d3dUtil.h"
#include "DXTrace.h"
using namespace DirectX;

GameApp::GameApp(HINSTANCE hInstance)
	: D3DApp(hInstance), 
	m_pVertexLayout(),
	m_VSConstantBuffer(),
	m_PSConstantBuffer(),
	m_DirLight(),
	m_PointLight(),
	m_SpotLight(),
	m_IsWireframeMode(false),
	m_Importer()
{
}

GameApp::~GameApp()
{
}

bool GameApp::Init()
{
	if (!D3DApp::Init())
		return false;

	// 初始化鼠标，键盘不需要
	m_pMouse->SetWindow(m_hMainWnd);
	m_pMouse->SetMode(DirectX::Mouse::MODE_ABSOLUTE);

	if (!InitResource())
		return false;

	return true;
}

void GameApp::OnResize()
{
	D3DApp::OnResize();
}

void GameApp::UpdateScene(float dt)
{
	UpdateMouse(dt);
	UpdateKeyboard(dt);
	UpdateConstantBuffer();
}

void GameApp::UpdateMouse(float dt) {
	// 获取鼠标状态
	Mouse::State mouseState = m_pMouse->GetState();
	Mouse::State lastMouseState = m_MouseTracker.GetLastState();

	// 缩放视图
	static int lastScrollValue = 0;
	float temp = (mouseState.scrollWheelValue - lastScrollValue) * 0.01f * logf(mRadius);
	mRadius -= temp;
	// 鼠标速度适配
	mMouseSpeed -= temp;
	lastScrollValue = mouseState.scrollWheelValue;
	static const XMVECTOR mUpDir = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	// 旋转视图
	m_MouseTracker.Update(mouseState);
	if (mouseState.rightButton == true && m_MouseTracker.rightButton == m_MouseTracker.HELD) {
		mTheta -= (mouseState.x - lastMouseState.x) * 0.01f;
		mPhi -= (mouseState.y - lastMouseState.y) * 0.01f;
	}
	mCameraPos = SphericalToCartesian(mRadius, mTheta, mPhi) + mTargetPos;

	// 拖拽视图
	if (mouseState.middleButton == true && m_MouseTracker.middleButton == m_MouseTracker.HELD) {
		XMVECTOR viewDir = XMVector3Normalize(mTargetPos - mCameraPos);
		XMVECTOR right = XMVector3Cross(mUpDir, viewDir);
		XMVECTOR up = XMVector3Cross(viewDir, right);
		right *= (mouseState.x - lastMouseState.x) * 0.002f * mMouseSpeed;
		up *= (mouseState.y - lastMouseState.y) * 0.002f * mMouseSpeed;
		mTargetPos += up; mTargetPos -= right;
		mCameraPos += up; mCameraPos -= right;
	}

	m_VSConstantBuffer.view = XMMatrixTranspose(XMMatrixLookAtLH(mCameraPos, mTargetPos, mUpDir));
	m_VSConstantBuffer.proj = XMMatrixTranspose(XMMatrixPerspectiveFovLH(XM_PIDIV2, AspectRatio(), NearZ, FarZ));
}

void GameApp::UpdateKeyboard(float dt) {
	// 键盘切换灯光类型
	Keyboard::State state = m_pKeyboard->GetState();
	m_KeyboardTracker.Update(state);
	if (m_KeyboardTracker.IsKeyPressed(Keyboard::D1)) {
		m_PSConstantBuffer.dirLight = m_DirLight;
		m_PSConstantBuffer.pointLight = PointLight();
		m_PSConstantBuffer.spotLight = SpotLight();
	} else if (m_KeyboardTracker.IsKeyPressed(Keyboard::D2)) {
		m_PSConstantBuffer.dirLight = DirectionalLight();
		m_PSConstantBuffer.pointLight = m_PointLight;
		m_PSConstantBuffer.spotLight = SpotLight();
	} else if (m_KeyboardTracker.IsKeyPressed(Keyboard::D3)) {
		m_PSConstantBuffer.dirLight = DirectionalLight();
		m_PSConstantBuffer.pointLight = PointLight();
		m_PSConstantBuffer.spotLight = m_SpotLight;
	}
	if (m_KeyboardTracker.IsKeyPressed(Keyboard::S)) {
		m_IsWireframeMode = !m_IsWireframeMode;
		m_pd3dImmediateContext->RSSetState(m_IsWireframeMode ? m_pRSWireframe.Get() : nullptr);
	}
	if (m_KeyboardTracker.IsKeyPressed(Keyboard::Space)) {
		ResetCameraState();
	}
}

void GameApp::ResetCameraState() {
	mTheta = DirectX::XM_PIDIV4;
	mPhi = DirectX::XM_PIDIV4;
	mRadius = 5.0f;
	mCameraPos = SphericalToCartesian(mRadius, mTheta, mPhi);
	mTargetPos = XMVectorZero();
	// 复位鼠标速度
	mMouseSpeed = 5.0f;
}

void GameApp::UpdateConstantBuffer() {
	// 更新常量缓冲区
	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(m_pd3dImmediateContext->Map(m_pConstantBuffers[0].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	memcpy_s(mappedData.pData, sizeof(VSConstantBuffer), &m_VSConstantBuffer, sizeof(VSConstantBuffer));
	m_pd3dImmediateContext->Unmap(m_pConstantBuffers[0].Get(), 0);

	HR(m_pd3dImmediateContext->Map(m_pConstantBuffers[1].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	memcpy_s(mappedData.pData, sizeof(PSConstantBuffer), &m_PSConstantBuffer, sizeof(PSConstantBuffer));
	m_pd3dImmediateContext->Unmap(m_pConstantBuffers[1].Get(), 0);
}

void GameApp::DrawScene()
{
	assert(m_pd3dImmediateContext);
	assert(m_pSwapChain);

	m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), reinterpret_cast<const float*>(&RenderTargetColor));
	m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	for (size_t i = 0; i < m_VertexBuffers.size(); ++i) {
		UINT stride = (UINT)sizeof(Vertex);
		UINT offset = 0;
		m_pd3dImmediateContext->IASetVertexBuffers(0, 1, m_VertexBuffers[i].GetAddressOf(), &stride, &offset);
		m_pd3dImmediateContext->IASetIndexBuffer(m_IndexBuffers[i].Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
		m_pd3dImmediateContext->DrawIndexed(m_IndexCounts[i], 0, 0);
	}

	HR(m_pSwapChain->Present(0, 0));
}

bool GameApp::InitResource() {

	if (!InitEffect()) return false;
	if (!InitModel()) return false;
	ResetCameraState();
	InitConstantBuffer();
	InitRasterizationState();
	InitBindAndSet();
	// ******************
	// 设置调试对象名
	//
	D3D11SetDebugObjectName(m_pVertexLayout.Get(), "VertexPosNormalTexLayout");
	D3D11SetDebugObjectName(m_pConstantBuffers[0].Get(), "VSConstantBuffer");
	D3D11SetDebugObjectName(m_pConstantBuffers[1].Get(), "PSConstantBuffer");
	D3D11SetDebugObjectName(m_pVertexShader.Get(), "VS");
	D3D11SetDebugObjectName(m_pPixelShader.Get(), "PS");

	return true;
}

bool GameApp::InitEffect() {
	ComPtr<ID3DBlob> blob;

	// 创建顶点着色器
	HR(CreateShaderFromFile(L"Shaders\\VS.cso", L"Shaders\\VS.hlsl", "main", "vs_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3dDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pVertexShader.GetAddressOf()));
	// 创建并绑定顶点布局
	HR(m_pd3dDevice->CreateInputLayout(VertexPosNormalColor::inputLayout, ARRAYSIZE(VertexPosNormalColor::inputLayout),
		blob->GetBufferPointer(), blob->GetBufferSize(), m_pVertexLayout.GetAddressOf()));

	// 创建像素着色器
	HR(CreateShaderFromFile(L"Shaders\\PS.cso", L"Shaders\\PS.hlsl", "main", "ps_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3dDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pPixelShader.GetAddressOf()));

	return true;
}

bool GameApp::InitModel() {

	ModelImporter importer;
	if (!importer.Import(ModelFilePath, model)) return false;

	// 释放旧资源
	m_VertexBuffers.clear();
	m_IndexBuffers.clear();

	// 设置顶点缓冲区描述
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	// 初始化子资源数据
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));

	// 设置索引缓冲区描述
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	
	for (auto& mesh : model.meshes) {
		// 新建顶点缓冲区
		vbd.ByteWidth = (UINT)mesh.vertices.size() * sizeof(Vertex);
		InitData.pSysMem = mesh.vertices.data();
		ComPtr<ID3D11Buffer> vertexBuffer;
		HR(m_pd3dDevice->CreateBuffer(&vbd, &InitData, vertexBuffer.GetAddressOf()));
		m_VertexBuffers.emplace_back(vertexBuffer);

		// 新建索引缓冲区
		ibd.ByteWidth = (UINT)mesh.indices.size() * sizeof(DWORD);
		InitData.pSysMem = mesh.indices.data();
		ComPtr<ID3D11Buffer> indexBuffer;
		HR(m_pd3dDevice->CreateBuffer(&ibd, &InitData, indexBuffer.GetAddressOf()));
		m_IndexBuffers.emplace_back(indexBuffer);

		m_IndexCounts.emplace_back((UINT)mesh.indices.size());
	}
	return true;
}

bool GameApp::InitConstantBuffer() {
	// ******************
	// 设置常量缓冲区描述
	//
	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof(cbd));
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.ByteWidth = sizeof(VSConstantBuffer);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	// 新建用于VS和PS的常量缓冲区
	HR(m_pd3dDevice->CreateBuffer(&cbd, nullptr, m_pConstantBuffers[0].GetAddressOf()));
	cbd.ByteWidth = sizeof(PSConstantBuffer);
	HR(m_pd3dDevice->CreateBuffer(&cbd, nullptr, m_pConstantBuffers[1].GetAddressOf()));

	// ******************
	// 初始化默认光照
	// 方向光
	m_DirLight.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_DirLight.diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	m_DirLight.specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_DirLight.direction = XMFLOAT3(0.577f, -0.577f, 0.577f);
	// 点光
	m_PointLight.position = XMFLOAT3(0.0f, 0.0f, -10.0f);
	m_PointLight.ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_PointLight.diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	m_PointLight.specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_PointLight.att = XMFLOAT3(0.0f, 0.1f, 0.0f);
	m_PointLight.range = 25.0f;
	// 聚光灯
	m_SpotLight.position = XMFLOAT3(0.0f, 0.0f, -5.0f);
	m_SpotLight.direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_SpotLight.ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_SpotLight.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_SpotLight.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_SpotLight.att = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_SpotLight.spot = 12.0f;
	m_SpotLight.range = 10000.0f;
	// 初始化用于VS的常量缓冲区的值
	m_VSConstantBuffer.world = XMMatrixIdentity();
	m_VSConstantBuffer.worldInvTranspose = XMMatrixIdentity();
	m_VSConstantBuffer.view = XMMatrixTranspose(XMMatrixLookAtLH(
		XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
	));
	m_VSConstantBuffer.proj = XMMatrixTranspose(XMMatrixPerspectiveFovLH(XM_PIDIV2, AspectRatio(), NearZ, FarZ));

	// 初始化用于PS的常量缓冲区的值
	m_PSConstantBuffer.material.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_PSConstantBuffer.material.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_PSConstantBuffer.material.specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 5.0f);
	// 使用默认平行光
	m_PSConstantBuffer.dirLight = m_DirLight;
	// 注意不要忘记设置此处的观察位置，否则高亮部分会有问题
	m_PSConstantBuffer.eyePos = XMFLOAT4(0.0f, 0.0f, -5.0f, 0.0f);

	// 更新PS常量缓冲区资源
	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(m_pd3dImmediateContext->Map(m_pConstantBuffers[1].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	memcpy_s(mappedData.pData, sizeof(PSConstantBuffer), &m_PSConstantBuffer, sizeof(PSConstantBuffer));
	m_pd3dImmediateContext->Unmap(m_pConstantBuffers[1].Get(), 0);
	return true;
}

bool GameApp::InitRasterizationState() {
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.DepthClipEnable = true;
	HR(m_pd3dDevice->CreateRasterizerState(&rasterizerDesc, m_pRSWireframe.GetAddressOf()));
	return true;
}

bool GameApp::InitBindAndSet() {
	// 设置图元类型，设定输入布局
	m_pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pd3dImmediateContext->IASetInputLayout(m_pVertexLayout.Get());
	// 将着色器绑定到渲染管线
	m_pd3dImmediateContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
	// VS常量缓冲区对应HLSL寄存于b0的常量缓冲区
	m_pd3dImmediateContext->VSSetConstantBuffers(0, 1, m_pConstantBuffers[0].GetAddressOf());
	// PS常量缓冲区对应HLSL寄存于b1的常量缓冲区
	m_pd3dImmediateContext->PSSetConstantBuffers(1, 1, m_pConstantBuffers[1].GetAddressOf());
	m_pd3dImmediateContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);
	return true;
}
#include "GameApp.h"
#include "d3dUtil.h"
#include "DXTrace.h"
using namespace DirectX;

GameApp::GameApp(HINSTANCE hInstance)
	: D3DApp(hInstance), 
	m_pBasicEffect(std::make_unique<BasicEffect>()),
	m_pShadowEffect(std::make_unique<ShadowEffect>()),
	m_pDebugEffect(std::make_unique<DebugEffect>()),
	m_pSSAOEffect(std::make_unique<SSAOEffect>()),
	m_EnableSSAO(EnableSSAODefault),
	m_EnableDebug(EnableDebugDefault),
	m_EnableShadow(EnableShadowDefault),
	m_SlopeIndex()
{
}

GameApp::~GameApp()
{
}

bool GameApp::Init()
{
	if (!D3DApp::Init())
		return false;

	// 先初始化所有渲染状态，以供下面的特效使用
	RenderStates::InitAll(m_pd3dDevice.Get());

	if (!m_pBasicEffect->InitAll(m_pd3dDevice.Get()))
		return false;

	if (!m_pShadowEffect->InitAll(m_pd3dDevice.Get()))
		return false;

	if (!m_pSSAOEffect->InitAll(m_pd3dDevice.Get()))
		return false;

	if (!m_pDebugEffect->InitAll(m_pd3dDevice.Get()))
		return false;

	if (!InitResource())
		return false;

	// 初始化鼠标，键盘不需要
	m_pMouse->SetWindow(m_hMainWnd);
	m_pMouse->SetMode(DirectX::Mouse::MODE_RELATIVE);

	return true;
}

void GameApp::OnResize()
{
	assert(m_pd2dFactory);
	assert(m_pdwriteFactory);
	// 释放D2D的相关资源
	m_pColorBrush.Reset();
	m_pd2dRenderTarget.Reset();

	D3DApp::OnResize();

	// 为D2D创建DXGI表面渲染目标
	ComPtr<IDXGISurface> surface;
	HR(m_pSwapChain->GetBuffer(0, __uuidof(IDXGISurface), reinterpret_cast<void**>(surface.GetAddressOf())));
	D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));
	HRESULT hr = m_pd2dFactory->CreateDxgiSurfaceRenderTarget(surface.Get(), &props, m_pd2dRenderTarget.GetAddressOf());
	surface.Reset();

	// 创建固定颜色刷和文本格式
	HR(m_pd2dRenderTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Black),
		m_pColorBrush.GetAddressOf()));
	HR(m_pdwriteFactory->CreateTextFormat(L"Consolas", nullptr, DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 15, L"zh-cn",
		m_pTextFormat.GetAddressOf()));

	// 摄像机变更显示
	if (m_pCamera != nullptr)
	{
		m_pCamera->SetFrustum(XM_PI / 3, AspectRatio(), NearZ, FarZ);
		m_pCamera->SetViewPort(0.0f, 0.0f, (float)m_ClientWidth, (float)m_ClientHeight);
		m_pBasicEffect->SetProjMatrix(m_pCamera->GetProjXM());
		m_pSSAOEffect->SetProjMatrix(m_pCamera->GetProjXM());
	}
	
	// SSAO图和调试用矩形重新设置
	if (m_pSSAOMap)
	{
		m_pSSAOMap->OnResize(m_pd3dDevice.Get(), m_ClientWidth, m_ClientHeight, XM_PI / 3, FarZ);

		Model quadModel;
		quadModel.SetMesh(m_pd3dDevice.Get(), Geometry::Create2DShow<VertexPosNormalTex>(XMFLOAT2(0.5f, -0.5f), XMFLOAT2(0.5f, 0.5f)));
		quadModel.modelParts[0].texDiffuse = m_pSSAOMap->GetAmbientTexture();
		m_DebugQuad.SetModel(std::move(quadModel));
	}

}

void GameApp::UpdateScene(float dt)
{

	// 更新鼠标事件，获取相对偏移量
	Mouse::State mouseState = m_pMouse->GetState();
	Mouse::State lastMouseState = m_MouseTracker.GetLastState();
	m_MouseTracker.Update(mouseState);

	Keyboard::State keyState = m_pKeyboard->GetState();
	m_KeyboardTracker.Update(keyState);

	auto cam1st = std::dynamic_pointer_cast<FirstPersonCamera>(m_pCamera);

	// 调试模式开关
	if (m_EnableSSAO && m_KeyboardTracker.IsKeyPressed(KeyDebug))
		m_EnableDebug = !m_EnableDebug;
	// SSAO开关
	if (m_KeyboardTracker.IsKeyPressed(KeySSAO))
	{
		m_EnableSSAO = !m_EnableSSAO;
		m_pBasicEffect->SetSSAOEnabled(m_EnableSSAO);
	}
	// 阴影贴图开关
	if (m_KeyboardTracker.IsKeyPressed(KeyShadow)) {
		m_EnableShadow = !m_EnableShadow;
		m_pBasicEffect->SetShadowEnabled(m_EnableShadow);
	}
		
	// ******************
	// 自由摄像机的操作
	//

	// 方向移动
	if (keyState.IsKeyDown(KeyUp))
		cam1st->MoveForward(dt * CameraSpeed);
	if (keyState.IsKeyDown(KeyDown))
		cam1st->MoveForward(dt * -CameraSpeed);
	if (keyState.IsKeyDown(KeyLeft))
		cam1st->Strafe(dt * -CameraSpeed);
	if (keyState.IsKeyDown(KeyRight))
		cam1st->Strafe(dt * CameraSpeed);

	// 在鼠标没进入窗口前仍为ABSOLUTE模式
	if (mouseState.positionMode == Mouse::MODE_RELATIVE)
	{
		cam1st->Pitch(mouseState.y * dt * 1.25f);
		cam1st->RotateY(mouseState.x * dt * 1.25f);
	}

	m_pBasicEffect->SetViewMatrix(m_pCamera->GetViewXM());
	// 为SSAO图设置观察矩阵
	m_pSSAOEffect->SetViewMatrix(m_pCamera->GetViewXM());
	m_pBasicEffect->SetEyePos(m_pCamera->GetPosition());

	

	// 更新光照
	static float theta = 0;	
	theta += dt * XM_2PI / 40.0f;

	for (int i = 0; i < 3; ++i)
	{
		XMVECTOR dirVec = XMLoadFloat3(&m_OriginalLightDirs[i]);
		dirVec = XMVector3Transform(dirVec, XMMatrixRotationY(theta));
		XMStoreFloat3(&m_DirLights[i].direction, dirVec);
		m_pBasicEffect->SetDirLight(i, m_DirLights[i]);
	}

	//
	// 投影区域为正方体，以原点为中心，以方向光为+Z朝向
	//
	XMVECTOR dirVec = XMLoadFloat3(&m_DirLights[0].direction);
	XMMATRIX LightView = XMMatrixLookAtLH(dirVec * 20.0f * (-2.0f), g_XMZero, g_XMIdentityR1);
	m_pShadowEffect->SetViewMatrix(LightView);
	
	// 将NDC空间 [-1, +1]^2 变换到纹理坐标空间 [0, 1]^2
	static XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);
	// S = V * P * T
	m_pBasicEffect->SetShadowTransformMatrix(LightView * XMMatrixOrthographicLH(40.0f, 40.0f, 20.0f, 60.0f) * T);

	// 退出程序，这里应向窗口发送销毁信息
	if (m_KeyboardTracker.IsKeyPressed(Keyboard::Escape))
		SendMessage(MainWnd(), WM_DESTROY, 0, 0);
}

void GameApp::DrawScene()
{
	assert(m_pd3dImmediateContext);
	assert(m_pSwapChain);

	m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), reinterpret_cast<const float*>(&Colors::Silver));
	m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	
	// ******************
	// 绘制到SSAO图
	//
	if (m_EnableSSAO)
	{
		m_pSSAOMap->Begin(m_pd3dImmediateContext.Get(), m_pDepthStencilView.Get());
		{
			// 绘制场景到法向量/深度缓冲区
			DrawScene(m_pSSAOEffect.get());

			// 计算环境光遮蔽值到SSAO图
			m_pSSAOMap->ComputeSSAO(m_pd3dImmediateContext.Get(), *m_pSSAOEffect, *m_pCamera);

			// 进行模糊
			m_pSSAOMap->BlurAmbientMap(m_pd3dImmediateContext.Get(), *m_pSSAOEffect, 4);
		}
		m_pSSAOMap->End(m_pd3dImmediateContext.Get());
	}
	
	// ******************
	// 绘制到阴影贴图
	//
	if (m_EnableShadow) {
		m_pShadowMap->Begin(m_pd3dImmediateContext.Get(), nullptr);
		{
			DrawScene(m_pShadowEffect.get());
		}
		m_pShadowMap->End(m_pd3dImmediateContext.Get());
	}

	// ******************
	// 正常绘制场景
	//
	m_pBasicEffect->SetTextureShadowMap(m_pShadowMap->GetOutputTexture());
	m_pBasicEffect->SetTextureSSAOMap(m_pSSAOMap->GetAmbientTexture());
	DrawScene(m_pBasicEffect.get());

	// 解除绑定
	m_pBasicEffect->SetTextureShadowMap(nullptr);
	m_pBasicEffect->SetTextureSSAOMap(nullptr);
	m_pBasicEffect->Apply(m_pd3dImmediateContext.Get());

	// ******************
	// 调试绘制SSAO图
	//
	if (m_EnableSSAO && m_EnableDebug)
	{
		m_pDebugEffect->SetRenderOneComponentGray(m_pd3dImmediateContext.Get(), 0);
		
		m_DebugQuad.Draw(m_pd3dImmediateContext.Get(), m_pDebugEffect.get());
		// 解除绑定
		m_pDebugEffect->SetTextureDiffuse(nullptr);
		m_pDebugEffect->Apply(m_pd3dImmediateContext.Get());
	}


	// ******************
	// 绘制Direct2D部分
	//
	if (m_pd2dRenderTarget != nullptr)
	{
		m_pd2dRenderTarget->BeginDraw();
		std::wostringstream outs;
		outs.precision(6);
		outs << L"FPS: " << m_Fps << L"\n"
			<< L"Frame Time: " << m_Mspf << L" (ms)\n";
		std::wstring text = outs.str();
		text += L"SSAO: " + (m_EnableSSAO ? std::wstring(L"ON") : std::wstring(L"OFF")) + L"\n";
		text += L"Debug SSAO Texture: " + (m_EnableDebug ? std::wstring(L"ON") : std::wstring(L"OFF")) + L"\n";
		text += L"Shadow: " + (m_EnableShadow ? std::wstring(L"ON") : std::wstring(L"OFF")) + L"\n";
		m_pd2dRenderTarget->DrawTextW(text.c_str(), (UINT32)text.length(), m_pTextFormat.Get(),
			D2D1_RECT_F{ 0.0f, 0.0f, 600.0f, 200.0f }, m_pColorBrush.Get());
		m_pd2dRenderTarget->EndDraw();
	}

	HR(m_pSwapChain->Present(0, 0));
}

void GameApp::DrawScene(BasicEffect* pBasicEffect, bool enableNormalMap)
{
	pBasicEffect->SetRenderDefault(m_pd3dImmediateContext.Get(), IEffect::RenderObject);
	m_Ground.Draw(m_pd3dImmediateContext.Get(), pBasicEffect);

	pBasicEffect->SetRenderDefault(m_pd3dImmediateContext.Get(), IEffect::RenderObject);
	m_Model.Draw(m_pd3dImmediateContext.Get(), pBasicEffect);
}

void GameApp::DrawScene(ShadowEffect* pShadowEffect)
{
	pShadowEffect->SetRenderDefault(m_pd3dImmediateContext.Get(), IEffect::RenderObject);
	m_Ground.Draw(m_pd3dImmediateContext.Get(), pShadowEffect);

	pShadowEffect->SetRenderDefault(m_pd3dImmediateContext.Get(), IEffect::RenderObject);
	m_Model.Draw(m_pd3dImmediateContext.Get(), pShadowEffect);
}

void GameApp::DrawScene(SSAOEffect* pSSAOEffect)
{
	pSSAOEffect->SetRenderNormalDepth(m_pd3dImmediateContext.Get(), IEffect::RenderObject);
	m_Ground.Draw(m_pd3dImmediateContext.Get(), pSSAOEffect);

	pSSAOEffect->SetRenderNormalDepth(m_pd3dImmediateContext.Get(), IEffect::RenderObject);
	m_Model.Draw(m_pd3dImmediateContext.Get(), pSSAOEffect);
}

bool GameApp::InitResource()
{
	// ******************
	// 初始化摄像机
	//
	auto camera = std::shared_ptr<FirstPersonCamera>(new FirstPersonCamera);
	m_pCamera = camera;

	camera->SetViewPort(0.0f, 0.0f, (float)m_ClientWidth, (float)m_ClientHeight);
	camera->SetFrustum(XM_PI / 3, AspectRatio(), NearZ, FarZ);
	camera->LookTo(XMFLOAT3(0.0f, 0.0f, -10.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));

	// ******************
	// 初始化阴影贴图、SSAO图和特效
	m_pShadowMap = std::make_unique<TextureRender>();
	HR(m_pShadowMap->InitResource(m_pd3dDevice.Get(), 2048, 2048, true));

	// 纹理、阴影、SSAO
	m_pBasicEffect->SetTextureUsed(false);
	m_pBasicEffect->SetShadowEnabled(m_EnableShadow);
	m_pBasicEffect->SetSSAOEnabled(m_EnableSSAO);
	m_pBasicEffect->SetViewMatrix(camera->GetViewXM());
	m_pBasicEffect->SetProjMatrix(camera->GetProjXM());

	m_pShadowEffect->SetProjMatrix(XMMatrixOrthographicLH(40.0f, 40.0f, 20.0f, 60.0f));

	m_pDebugEffect->SetWorldMatrix(XMMatrixIdentity());
	m_pDebugEffect->SetViewMatrix(XMMatrixIdentity());
	m_pDebugEffect->SetProjMatrix(XMMatrixIdentity());

	m_pSSAOEffect->SetViewMatrix(camera->GetViewXM());
	m_pSSAOEffect->SetProjMatrix(camera->GetProjXM());

	// ******************
	// 初始化SSAO图
	m_pSSAOMap = std::make_unique<SSAORender>();
	HR(m_pSSAOMap->InitResource(m_pd3dDevice.Get(), m_ClientWidth, m_ClientHeight, XM_PI / 3, FarZ));

	// ******************
	// 初始化对象
	//

	// 地面
	Model groundModel;

	groundModel.SetMesh(m_pd3dDevice.Get(), Geometry::CreatePlane(XMFLOAT2(20.0f, 20.0f), XMFLOAT2(1.0f, 1.0f)));
	groundModel.modelParts[0].material.ambient = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	groundModel.modelParts[0].material.diffuse = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	groundModel.modelParts[0].material.specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);
	groundModel.modelParts[0].material.reflect = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_Ground.SetModel(std::move(groundModel));
	m_Ground.GetTransform().SetPosition(0.0f, 0.0f, 0.0f);

	// 模型
	m_Importer.Import(ModelFilePath);
	m_Model.SetModel(Model(m_pd3dDevice.Get(), m_Importer));

	XMMATRIX S = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	BoundingBox modelBox = m_Model.GetLocalBoundingBox();
	modelBox.Transform(modelBox, S);
	
	Transform& modelTransform = m_Model.GetTransform();
	modelTransform.SetScale(0.01f, 0.01f, 0.01f);
	modelTransform.SetPosition(0.0f, modelBox.Extents.y - modelBox.Center.y, 0.0f);

	// 调试用矩形
	Model quadModel;
	quadModel.SetMesh(m_pd3dDevice.Get(), Geometry::Create2DShow<VertexPosNormalTex>(XMFLOAT2(0.5f, -0.5f), XMFLOAT2(0.5f, 0.5f)));
	quadModel.modelParts[0].texDiffuse = m_pSSAOMap->GetAmbientTexture();
	m_DebugQuad.SetModel(std::move(quadModel));

	// ******************
	// 初始化光照
	m_DirLights[0].ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_DirLights[0].diffuse = XMFLOAT4(0.5f, 0.5f, 0.4f, 1.0f);
	m_DirLights[0].specular = XMFLOAT4(0.8f, 0.8f, 0.7f, 1.0f);
	m_DirLights[0].direction = XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);

	m_DirLights[1].ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_DirLights[1].diffuse = XMFLOAT4(0.40f, 0.40f, 0.40f, 1.0f);
	m_DirLights[1].specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_DirLights[1].direction = XMFLOAT3(0.707f, -0.707f, 0.0f);

	m_DirLights[2].ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_DirLights[2].diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_DirLights[2].specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_DirLights[2].direction = XMFLOAT3(0.0f, 0.0, -1.0f);

	for (int i = 0; i < 3; ++i)
	{
		m_OriginalLightDirs[i] = m_DirLights[i].direction;
		m_pBasicEffect->SetDirLight(i, m_DirLights[i]);
	}
	
	// ******************
	// 设置调试对象名
	m_Ground.SetDebugObjectName("Ground");
	m_Model.SetDebugObjectName("Model");
	m_DebugQuad.SetDebugObjectName("DebugQuad");
	m_pShadowMap->SetDebugObjectName("ShadowMap");
	m_pSSAOMap->SetDebugObjectName("SSAOMap");

	return true;
}


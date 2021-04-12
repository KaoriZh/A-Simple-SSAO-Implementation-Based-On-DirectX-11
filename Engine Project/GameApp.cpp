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
	m_DirLight(),
	m_OriginalLightDir(),
	m_EnableSSAO(EnableSSAODefault),
	m_EnableDebug(EnableDebugDefault),
	m_EnableShadow(EnableShadowDefault),
	m_EnableSceneInfo(EnableSceneInfoDefault),
	m_EnableLightRotation(EnableLightRotationDefault),
	m_MouseModeRelative(true),
	m_CurrentSSAOLevelIndex(0),
	m_BlurCount(0),
	m_CurrentModelIndex(0)
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
		DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 14, L"zh-cn",
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
		m_pSSAOMap->OnResize(m_pd3dDevice.Get(), m_ClientWidth, m_ClientHeight, XM_PI / 3, FarZ, SSAOLevels[m_CurrentSSAOLevelIndex]);

		Model quadModel;
		quadModel.SetMesh(m_pd3dDevice.Get(), Geometry::Create2DShow<VertexPosNormalTex>(XMFLOAT2(0.5f, -0.5f), XMFLOAT2(0.5f, 0.5f)));
		quadModel.modelParts[0].texDiffuse = m_pSSAOMap->GetAmbientTexture();
		m_DebugQuad.SetModel(std::move(quadModel));
	}

}

void GameApp::UpdateScene(float dt)
{
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
	// 切换模型
	if (m_KeyboardTracker.IsKeyPressed(KeyModelSwitch)) {
		m_CurrentModelIndex = (m_CurrentModelIndex + 1) % m_Models.size();
	}
	// 场景信息显示开关
	if (m_KeyboardTracker.IsKeyPressed(KeySceneInfoSwitch))
		m_EnableSceneInfo = !m_EnableSceneInfo;
	// 旋转灯光
	if (m_KeyboardTracker.IsKeyPressed(KeyLightRotationSwitch))
		m_EnableLightRotation = !m_EnableLightRotation;
	// 切换鼠标模式
	if (m_KeyboardTracker.IsKeyPressed(KeyMouseModeSwitch))
		m_MouseModeRelative = !m_MouseModeRelative;
	if (m_MouseModeRelative) {
		m_pMouse->SetMode(DirectX::Mouse::MODE_RELATIVE);
	} else {
		m_pMouse->SetMode(DirectX::Mouse::MODE_ABSOLUTE);
	}
	// SSAO分辨率等级
	if (m_EnableSSAO && m_KeyboardTracker.IsKeyPressed(KeySSAOLevelSwitch)) {
		m_CurrentSSAOLevelIndex = (m_CurrentSSAOLevelIndex + 1) % SSAOLevels.size();
		m_pSSAOMap->OnResize(m_pd3dDevice.Get(), m_ClientWidth, m_ClientHeight, XM_PI / 3, FarZ, SSAOLevels[m_CurrentSSAOLevelIndex]);

		Model quadModel;
		quadModel.SetMesh(m_pd3dDevice.Get(), Geometry::Create2DShow<VertexPosNormalTex>(XMFLOAT2(0.5f, -0.5f), XMFLOAT2(0.5f, 0.5f)));
		quadModel.modelParts[0].texDiffuse = m_pSSAOMap->GetAmbientTexture();
		m_DebugQuad.SetModel(std::move(quadModel));
	}
	// 模糊次数
	if (m_EnableSSAO && m_KeyboardTracker.IsKeyPressed(KeyBlurCountSwitch)) {
		m_BlurCount = (m_BlurCount + 1) % MaxBlurCount;
	}

	// ******************
	// 自由摄像机的操作
	
	// 方向移动
	auto speed = dt * CameraSpeed;
	if (keyState.IsKeyDown(KeySpeedControl))
		speed *= SpeedFactor;
	if (keyState.IsKeyDown(KeyUp))
		cam1st->MoveForward(speed);
	if (keyState.IsKeyDown(KeyDown))
		cam1st->MoveForward(-speed);
	if (keyState.IsKeyDown(KeyLeft))
		cam1st->Strafe(-speed);
	if (keyState.IsKeyDown(KeyRight))
		cam1st->Strafe(speed);

	// 更新鼠标事件，获取相对偏移量
	Mouse::State mouseState = m_pMouse->GetState();
	Mouse::State lastMouseState = m_MouseTracker.GetLastState();
	m_MouseTracker.Update(mouseState);

	// 在鼠标没进入窗口前仍为ABSOLUTE模式
	if (mouseState.positionMode == Mouse::MODE_RELATIVE)
	{
		auto speedx = mouseState.x * dt * 1.25f,
			speedy = mouseState.y * dt * 1.25f;
		if (keyState.IsKeyDown(KeySpeedControl)) {
			speedx *= SpeedFactor;
			speedy *= SpeedFactor;
		}
		cam1st->Pitch(speedy);
		cam1st->RotateY(speedx);
	}

	m_pBasicEffect->SetTextureUsed(EnableTextureUsed);
	m_pBasicEffect->SetViewMatrix(m_pCamera->GetViewXM());
	// 为SSAO图设置观察矩阵
	m_pSSAOEffect->SetViewMatrix(m_pCamera->GetViewXM());
	m_pBasicEffect->SetEyePos(m_pCamera->GetPosition());
	

	// 更新光照
	static float theta = 0;
	if (m_EnableLightRotation)
		theta += dt * XM_2PI / 40.0f;
	

	XMVECTOR dirVec = XMLoadFloat3(&m_OriginalLightDir);
	dirVec = XMVector3Transform(dirVec, XMMatrixRotationY(theta));
	XMStoreFloat3(&m_DirLight.direction, dirVec);
	m_pBasicEffect->SetDirLight(0, m_DirLight);

	//
	// 投影区域为正方体，以原点为中心，以方向光为+Z朝向
	//
	dirVec = XMLoadFloat3(&m_DirLight.direction);
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
	if (m_KeyboardTracker.IsKeyPressed(KeyQuit))
		SendMessage(MainWnd(), WM_DESTROY, 0, 0);
}

void GameApp::DrawScene()
{
	assert(m_pd3dImmediateContext);
	assert(m_pSwapChain);

	m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), reinterpret_cast<const float*>(&Colors::White));
	m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	
	// ******************
	// 绘制到SSAO图
	//
	if (m_EnableSSAO)
	{
		m_pSSAOMap->Begin(m_pd3dImmediateContext.Get(), m_pDepthStencilView.Get());
		{
			// 绘制场景到法向量/深度缓冲区
			const auto& pSSAOEffect = m_pSSAOEffect.get();
			pSSAOEffect->SetRenderNormalDepth(m_pd3dImmediateContext.Get(), IEffect::RenderObject);
			m_Models[m_CurrentModelIndex].Draw(m_pd3dImmediateContext.Get(), pSSAOEffect);

			// 计算环境光遮蔽值到SSAO图
			m_pSSAOMap->ComputeSSAO(m_pd3dImmediateContext.Get(), *m_pSSAOEffect, *m_pCamera);

			// 进行模糊
			m_pSSAOMap->BlurAmbientMap(m_pd3dImmediateContext.Get(), *m_pSSAOEffect, m_BlurCount);
		}
		m_pSSAOMap->End(m_pd3dImmediateContext.Get());
	}
	
	// ******************
	// 绘制到阴影贴图
	//
	if (m_EnableShadow) {
		m_pShadowMap->Begin(m_pd3dImmediateContext.Get(), nullptr);
		{
			const auto& pShadowEffect = m_pShadowEffect.get();
			pShadowEffect->SetRenderDefault(m_pd3dImmediateContext.Get(), IEffect::RenderObject);
			m_Models[m_CurrentModelIndex].Draw(m_pd3dImmediateContext.Get(), pShadowEffect);
		}
		m_pShadowMap->End(m_pd3dImmediateContext.Get());
	}

	// ******************
	// 正常绘制场景
	//
	m_pBasicEffect->SetTextureShadowMap(m_pShadowMap->GetOutputTexture());
	m_pBasicEffect->SetTextureSSAOMap(m_pSSAOMap->GetAmbientTexture());

	const auto& pBasicEffect = m_pBasicEffect.get();
	pBasicEffect->SetRenderDefault(m_pd3dImmediateContext.Get(), IEffect::RenderObject);
	m_Models[m_CurrentModelIndex].Draw(m_pd3dImmediateContext.Get(), pBasicEffect);

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
	if (m_pd2dRenderTarget != nullptr && m_EnableSceneInfo)
	{
		static std::wstring ON(L"ON"), OFF(L"OFF");
		std::wostringstream outs;
		outs.precision(6);

		std::wstring text = L"Resolution: " + std::to_wstring(m_ClientWidth) + L" x " + std::to_wstring(m_ClientHeight) + L"\n";
		text += L"FPS: " + std::to_wstring((int)m_Fps) + L"    " + 
		        L"Frame Time: " + std::to_wstring(m_Mspf) + L" (ms)\n";

		outs << L"Model: " << m_ModelFileNames[m_CurrentModelIndex].c_str() << L"\n"
			<< L"Vertices: " << m_Models[m_CurrentModelIndex].GetModelVertexCount() << L"    "
			<< L"Faces: " << m_Models[m_CurrentModelIndex].GetModelFaceCount() << L"\n";
		text += outs.str();

		text += L"Shadow: " + (m_EnableShadow ? ON : OFF) + L"\n";
		text += L"SSAO: " + (m_EnableSSAO ? ON : OFF) + L"\n";
		static std::wstring s;
		if (m_EnableSSAO) {
			s = std::to_wstring(SSAOLevels[m_CurrentSSAOLevelIndex]);
			s.resize(3);
			text += L"SSAO Level: " + s + L"\n";
			text += L"Blur: " + std::to_wstring(m_BlurCount) + L"\n";
		}
		text += L"Debug SSAO Texture: " + (m_EnableDebug ? ON : OFF) + L"\n";

		m_pd2dRenderTarget->BeginDraw();
		m_pd2dRenderTarget->DrawTextW(text.c_str(), (UINT32)text.length(), m_pTextFormat.Get(),
			D2D1_RECT_F{ 0.0f, 0.0f, (float)m_ClientWidth, (float)m_ClientHeight }, m_pColorBrush.Get());
		m_pd2dRenderTarget->EndDraw();
	}

	HR(m_pSwapChain->Present(0, 0));
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
	camera->LookTo(XMFLOAT3(10.0f, 10.0f, -10.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));

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
	HR(m_pSSAOMap->InitResource(m_pd3dDevice.Get(), m_ClientWidth, m_ClientHeight, XM_PI / 3, FarZ, SSAOLevels[m_CurrentSSAOLevelIndex]));
	m_pSSAOMap->SetDebugObjectName("SSAOMap_" + std::to_string(m_CurrentSSAOLevelIndex));

	// ******************
	// 初始化对象
	//

	GetFiles(ModelFilePath, ModelFileFormat, m_ModelFileNames);

	// 模型
	for (const auto& name : m_ModelFileNames) {

		auto path = ModelFilePath + "\\" + name + "." + ModelFileFormat;

		m_Importer.Import(path, ImportModelAsOnePart);
		GameObject go;
		go.SetModel(Model(m_pd3dDevice.Get(), m_Importer));

		XMMATRIX S = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		BoundingBox modelBox = go.GetLocalBoundingBox();
		modelBox.Transform(modelBox, S);

		Transform& modelTransform = go.GetTransform();
		modelTransform.SetScale(0.01f, 0.01f, 0.01f);
		modelTransform.SetPosition(0.0f, modelBox.Extents.y - modelBox.Center.y, 0.0f);

		m_Models.emplace_back(go);
	}

	// 调试用矩形
	Model quadModel;
	quadModel.SetMesh(m_pd3dDevice.Get(), Geometry::Create2DShow<VertexPosNormalTex>(XMFLOAT2(0.5f, -0.5f), XMFLOAT2(0.5f, 0.5f)));
	quadModel.modelParts[0].texDiffuse = m_pSSAOMap->GetAmbientTexture();
	m_DebugQuad.SetModel(std::move(quadModel));

	// ******************
	// 初始化光照
	m_DirLight = DefaultLight;
	m_OriginalLightDir = m_DirLight.direction;
	m_pBasicEffect->SetDirLight(0, m_DirLight);
	
	// ******************
	// 设置调试对象名
	m_DebugQuad.SetDebugObjectName("DebugQuad");
	m_pShadowMap->SetDebugObjectName("ShadowMap");

	return true;
}
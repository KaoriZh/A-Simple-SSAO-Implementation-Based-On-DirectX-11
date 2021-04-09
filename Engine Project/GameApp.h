#pragma once

#include <random>
#include "d3dApp.h"
#include "Camera.h"
#include "GameObject.h"
#include "ObjReader.h"
#include "ModelImporter.h"
#include "SSAORender.h"
#include "TextureRender.h"

class GameApp : public D3DApp
{
public:
	// 摄像机模式
	enum class CameraMode { FirstPerson, ThirdPerson, Free };
public:
	GameApp(HINSTANCE hInstance);
	~GameApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();
	void DrawScene(BasicEffect* pBasicEffect, bool enableNormalMap = false);
	void DrawScene(ShadowEffect* pShadowEffect);
	void DrawScene(SSAOEffect* pSSAOEffect);

private:
	bool InitResource();

private:
	
	ComPtr<ID2D1SolidColorBrush> m_pColorBrush;				    // 单色笔刷
	ComPtr<IDWriteFont> m_pFont;								// 字体
	ComPtr<IDWriteTextFormat> m_pTextFormat;					// 文本格式

	bool m_EnableDebug;											// 开启调试模式
	bool m_EnableSSAO;											// 开启SSAO
	bool m_EnableShadow;										// 阴影
	size_t m_SlopeIndex;										// 斜率索引

	GameObject m_Ground;										// 地面

	GameObject m_Model;											// 主模型

	GameObject m_DebugQuad;										// 调试用四边形

	DirectionalLight m_DirLights[3] = {};						// 方向光
	DirectX::XMFLOAT3 m_OriginalLightDirs[3] = {};				// 初始光方向

	std::unique_ptr<BasicEffect> m_pBasicEffect;				// 基础特效
	std::unique_ptr<ShadowEffect> m_pShadowEffect;				// 阴影特效
	std::unique_ptr<SSAOEffect> m_pSSAOEffect;					// SSAO特效
	std::unique_ptr<DebugEffect> m_pDebugEffect;				// 调试用显示纹理的特效

	std::unique_ptr<TextureRender> m_pShadowMap;				// 阴影贴图
	std::unique_ptr<SSAORender> m_pSSAOMap;						// SSAO贴图

	std::shared_ptr<Camera> m_pCamera;						    // 摄像机

	ModelImporter m_Importer;					// 模型导入器
};
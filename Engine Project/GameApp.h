#pragma once

#include <random>
#include <string>
#include "d3dApp.h"
#include "Camera.h"
#include "GameObject.h"
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

private:
	bool InitResource();

private:
	
	ComPtr<ID2D1SolidColorBrush> m_pColorBrush;				    // 单色笔刷
	ComPtr<IDWriteTextFormat> m_pTextFormat;					// 文本格式
	ComPtr<IDWriteFont> m_pFont;								// 字体

	bool m_EnableDebug;											// 开启调试模式
	bool m_EnableSSAO;											// 开启SSAO
	bool m_EnableShadow;										// 阴影
	bool m_EnableSceneInfo;										// 场景信息
	bool m_EnableLightRotation;									// 灯光旋转
	bool m_MouseModeRelative;

	size_t m_CurrentSSAOLevelIndex;
	int m_BlurCount;

	std::vector<std::string> m_ModelFileNames;
	std::vector<GameObject> m_Models;
	size_t m_CurrentModelIndex;

	GameObject m_DebugQuad;										// SSAO Debug Quad

	DirectionalLight m_DirLight;						// 方向光
	DirectX::XMFLOAT3 m_OriginalLightDir;				// 初始光方向

	std::unique_ptr<BasicEffect> m_pBasicEffect;				// 基础特效
	std::unique_ptr<ShadowEffect> m_pShadowEffect;				// 阴影特效
	std::unique_ptr<SSAOEffect> m_pSSAOEffect;					// SSAO特效
	std::unique_ptr<DebugEffect> m_pDebugEffect;				// 调试用显示纹理的特效

	std::unique_ptr<TextureRender> m_pShadowMap;				// 阴影贴图
	std::unique_ptr<SSAORender> m_pSSAOMap;

	std::shared_ptr<Camera> m_pCamera;						    // 摄像机

	ModelImporter m_Importer;					// 模型导入器
};
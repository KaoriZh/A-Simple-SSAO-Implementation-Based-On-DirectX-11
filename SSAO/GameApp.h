// Core

#ifndef GAMEAPP_H
#define GAMEAPP_H
#include "d3dApp.h"
#include "Vertex.h"
#include "Buffer.h"
#include "Light.h"
#include "Material.h"
#include "Geometry.h"
#include "ModelImporter.h"

class GameApp : public D3DApp
{
public:
	GameApp(HINSTANCE hInstance);
	~GameApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

private:
	bool InitResource();
	bool InitEffect();
	bool InitModel();
	bool InitConstantBuffer();
	bool InitRasterizationState();
	bool InitBindAndSet();
	void UpdateMouse(float dt);
	void UpdateKeyboard(float dt);
	void UpdateConstantBuffer();
	void ResetCameraState();

private:
	ComPtr<ID3D11InputLayout> m_pVertexLayout;    	// 顶点输入布局
	std::vector<ComPtr<ID3D11Buffer>> m_VertexBuffers;		    // 顶点缓冲区
	std::vector<ComPtr<ID3D11Buffer>> m_IndexBuffers;       	// 索引缓冲区
	std::vector<UINT> m_IndexCounts;				// 索引数量
	ComPtr<ID3D11VertexShader> m_pVertexShader;	    // 顶点着色器
	ComPtr<ID3D11PixelShader> m_pPixelShader;	    // 像素着色器

	VSConstantBuffer m_VSConstantBuffer;			// 用于修改用于VS的GPU常量缓冲区的变量
	PSConstantBuffer m_PSConstantBuffer;			// 用于修改用于PS的GPU常量缓冲区的变量
	ComPtr<ID3D11Buffer> m_pConstantBuffers[2];	    // 常量缓冲区

	DirectionalLight m_DirLight;					// 默认环境光
	PointLight m_PointLight;						// 默认点光
	SpotLight m_SpotLight;						    // 默认汇聚光

	ComPtr<ID3D11RasterizerState> m_pRSWireframe;	// 光栅化状态: 线框模式
	bool m_IsWireframeMode;							// 当前是否为线框模式

	ModelImporter m_Importer;                       // 模型导入器
	Model model;

	float mMouseSpeed = 5.0f;

	// 相机在世界空间下的坐标
	// 从子类迁移到基类，方便调整视角
	// 球坐标系参数，基于被观察点
	float mTheta;
	float mPhi;
	float mRadius;
	// 直角坐标系坐标
	XMVECTOR mCameraPos;
	// 被观察点，默认为世界空间原点
	XMVECTOR mTargetPos;
};

#endif
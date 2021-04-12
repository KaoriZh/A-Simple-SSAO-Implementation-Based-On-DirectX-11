#pragma once

#include <string>
#include <vector>
#include "Keyboard.h"
#include "Light.h"

namespace {

#pragma region WindowSetting
	static const std::wstring WindowName = L"LiF Engine"; // Window Title

	static const int WindowWidth = 1000;
	static const int WindowHeight = 750;
#pragma endregion

#pragma region Asset
	static const std::string ModelFilePath = "Asset\\Model"; // Model's file path, relative to .vcxproj
	static const std::string ModelFileFormat = "obj"; // all file with format "ModelFileFormat" will be imported
	
	static const bool ImportModelAsOnePart = true; // if true, the whole scene can be rendered in ONE draw call, but in one DEFAULT material without textures.
#pragma endregion

#pragma region RenderSetting
	static const float NearZ = 1.0f; // frustum near plane z
	static const float FarZ = 5000.0f; // frustum far plane z

	static const std::vector<float> SSAOLevels = { 0.5f, 1.0f, 2.0f, 4.0f }; // ssao texture scaling relative to window size
	static const int MaxBlurCount = 8; // maximum ssao blur allowed

	static const bool EnableShadowDefault = false;
	static const bool EnableSSAODefault = false;
	static const bool EnableDebugDefault = false;
	static const bool EnableSceneInfoDefault = true;
	static const bool EnableLightRotationDefault = false;
	static const bool EnableTextureUsed = false;

	static const float CameraSpeed = 6.0f; // velocity
	static const float SpeedFactor = 0.1f; // when "KeySpeedControl" is pressed, speed will be multipled by this factor

	static const DirectionalLight DefaultLight = {
		DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),			// ambient
		DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),			// diffuse
		DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f),			// specular
		DirectX::XMFLOAT3(-0.57735f, -0.57735f, 0.57735f)	// reflect
	};

	static const Material DefaultMaterial = {
		DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f),		// ambient
		DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f),		// diffuse
		DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f),		// specular
		DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)		// reflect
	};
#pragma endregion

#pragma region UserInput
	static const DirectX::Keyboard::Keys KeyUp = DirectX::Keyboard::Keys::W;
	static const DirectX::Keyboard::Keys KeyDown = DirectX::Keyboard::Keys::S;
	static const DirectX::Keyboard::Keys KeyLeft = DirectX::Keyboard::Keys::A;
	static const DirectX::Keyboard::Keys KeyRight = DirectX::Keyboard::Keys::D;
	static const DirectX::Keyboard::Keys KeySpeedControl = DirectX::Keyboard::Keys::LeftControl;

	static const DirectX::Keyboard::Keys KeyDebug = DirectX::Keyboard::Keys::Q;
	static const DirectX::Keyboard::Keys KeySSAO = DirectX::Keyboard::Keys::E;
	static const DirectX::Keyboard::Keys KeyShadow = DirectX::Keyboard::Keys::R;

	static const DirectX::Keyboard::Keys KeySSAOLevelSwitch = DirectX::Keyboard::Keys::Z;
	static const DirectX::Keyboard::Keys KeyBlurCountSwitch = DirectX::Keyboard::Keys::X;

	static const DirectX::Keyboard::Keys KeyModelSwitch = DirectX::Keyboard::Keys::Tab;
	static const DirectX::Keyboard::Keys KeySceneInfoSwitch = DirectX::Keyboard::Keys::M;
	static const DirectX::Keyboard::Keys KeyLightRotationSwitch = DirectX::Keyboard::Keys::L;

	static const DirectX::Keyboard::Keys KeyMouseModeSwitch = DirectX::Keyboard::Keys::Space;
	static const DirectX::Keyboard::Keys KeyQuit = DirectX::Keyboard::Keys::Escape;
#pragma endregion
}
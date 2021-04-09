#pragma once

#include <string>

namespace {

#pragma region WindowSetting
	static const std::wstring WindowName = L"LiF Engine";

	static const int WindowWidth = 800;
	static const int WindowHeight = 600;
#pragma endregion

#pragma region Asset
	static const std::string ModelFilePath = "Asset\\Model\\sponza.obj";
#pragma endregion

#pragma region RenderSetting
	static const float NearZ = 1.0f;
	static const float FarZ = 5000.0f;

	static const bool EnableShadowDefault = false;
	static const bool EnableSSAODefault = false;
	static const bool EnableDebugDefault = false;

	static const float CameraSpeed = 6.0f;
#pragma endregion

#pragma region UserInput
	static const DirectX::Keyboard::Keys KeyUp = DirectX::Keyboard::Keys::W;
	static const DirectX::Keyboard::Keys KeyDown = DirectX::Keyboard::Keys::S;
	static const DirectX::Keyboard::Keys KeyLeft = DirectX::Keyboard::Keys::A;
	static const DirectX::Keyboard::Keys KeyRight = DirectX::Keyboard::Keys::D;

	static const DirectX::Keyboard::Keys KeyDebug = DirectX::Keyboard::Keys::Q;
	static const DirectX::Keyboard::Keys KeySSAO = DirectX::Keyboard::Keys::E;
	static const DirectX::Keyboard::Keys KeyShadow = DirectX::Keyboard::Keys::R;
#pragma endregion
}
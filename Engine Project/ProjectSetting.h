#pragma once

#include <string>

namespace {

#pragma region WindowSetting
	static const std::wstring WindowName = L"LiF Engine";

	static const int WindowWidth = 800;
	static const int WindowHeight = 600;
#pragma endregion

#pragma region Asset
	static const std::wstring ModelFilePath = L"Asset\\Model\\house.obj";
	static const wchar_t* ModelFilePathObj = L"Asset\\Model\\house.obj";
	static const wchar_t* ModelFilePathMbo = L"Asset\\Model\\house.mbo";
#pragma endregion

#pragma region RenderSetting
	static const float NearZ = 1.0f;
	static const float FarZ = 10000.0f;
#pragma endregion

}
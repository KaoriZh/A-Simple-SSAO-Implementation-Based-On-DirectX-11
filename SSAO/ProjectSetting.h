#pragma once

#include <string>
#include <DirectXColors.h>
#include <d3d11_1.h>

namespace ProjectSetting {

	const std::wstring WindowName = L"LiF Engine";

	const int WindowWidth = 800;
	const int WindowHeight = 600;
	const bool IsEnableMSAA = true;
	const float NearZ = 1.0f;
	const float FarZ = 10000.0f;
	const UINT RefreshRateNumerator = 60;
	const UINT RefreshRateDenominator = 1;

	const DirectX::XMVECTORF32 RenderTargetColor = DirectX::Colors::White;

	const std::string ModelFilePath = "Models\\Eiffel_Tower.obj";

}
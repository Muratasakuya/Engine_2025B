#pragma once

//============================================================================
//	include
//============================================================================
#include <Lib/MathUtils/Vector4.h>

// directX
#include <d3d12.h>
// c++
#include <cstdint>
#include <array>

//============================================================================
//	DxStructures
//============================================================================

// 描画先の情報
struct RenderTarget {

	uint32_t width;
	uint32_t height;
	Color clearColor;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
};

enum BlendMode {

	kBlendModeNormal,   // 通常αブレンド
	kBlendModeAdd,      // 加算
	kBlendModeSubtract, // 減算
	kBlendModeMultiply, // 乗算
	kBlendModeScreen,   // スクリーン

	BlendCount
};
constexpr std::array<BlendMode, static_cast<size_t>(BlendMode::BlendCount)>
CreateBlendModeTypes() {
	std::array<BlendMode, static_cast<size_t>(BlendMode::BlendCount)> types = {};
	for (uint32_t i = 0; i < static_cast<uint32_t>(BlendMode::BlendCount); ++i) {
		types[i] = static_cast<BlendMode>(i);
	}
	return types;
}
static constexpr uint32_t blendModeNum = static_cast<uint32_t>(BlendMode::BlendCount);
static constexpr
std::array<BlendMode, static_cast<size_t>(BlendMode::BlendCount)>
blendModeTypes = CreateBlendModeTypes();

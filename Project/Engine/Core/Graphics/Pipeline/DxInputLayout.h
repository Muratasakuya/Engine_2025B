#pragma once

//============================================================================
//	include
//============================================================================

// directX
#include <d3d12.h>
// c++
#include <vector>
#include <memory>
#include <string>
#include <optional>
// json
#include <Externals/nlohmann/json.hpp>
// using
using Json = nlohmann::json;

//============================================================================
//	DxInputLayout class
//============================================================================
class DxInputLayout {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	DxInputLayout() = default;
	~DxInputLayout() = default;

	void Create(const Json& json, std::optional<D3D12_INPUT_LAYOUT_DESC>& desc);
};
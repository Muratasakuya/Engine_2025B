#pragma once

//============================================================================
//	include
//============================================================================

// windows
#include <Windows.h>
// directX
#include <d3d12.h>
// c++
#include <cstdint>
#include <string>

//============================================================================
//	ImGuiManager class
//============================================================================
class ImGuiManager {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	ImGuiManager() = default;
	~ImGuiManager() = default;

	void Init(HWND hwnd, UINT bufferCount, ID3D12Device* device, class SRVManager* srvManager);

	void Begin();
	void End();

	void Draw(ID3D12GraphicsCommandList* commandList);

	void Finalize();
};
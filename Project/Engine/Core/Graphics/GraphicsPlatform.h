#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/DxDevice.h>
#include <Engine/Core/Graphics/DxCommand.h>
#include <Engine/Core/Graphics/Pipeline/DxShaderCompiler.h>

// directX
#include <dxgidebug.h>
#include <dxgi1_6.h>

//============================================================================
//	GraphicsPlatform class
//============================================================================
class GraphicsPlatform {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	GraphicsPlatform() = default;
	~GraphicsPlatform() = default;

	void Init();

	void Finalize(HWND hwnd);

	//--------- accessor -----------------------------------------------------

	ID3D12Device8* GetDevice() const { return dxDevice_->Get(); }
	IDXGIFactory7* GetDxgiFactory() const { return dxDevice_->GetDxgiFactory(); }

	DxCommand* GetDxCommand() const { return dxCommand_.get(); }

	DxShaderCompiler* GetDxShaderCompiler() const { return dxShaderComplier_.get(); }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::unique_ptr<DxDevice> dxDevice_;

	std::unique_ptr<DxCommand> dxCommand_;

	std::unique_ptr<DxShaderCompiler> dxShaderComplier_;

	//--------- functions ----------------------------------------------------

	void InitDXDevice();
};
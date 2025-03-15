#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Lib/DxStructures.h>
#include <Engine/Core/Lib/ComPtr.h>

// front
class RTVManager;
class SRVManager;

//============================================================================
//	RenderTexture class
//============================================================================
class RenderTexture {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	RenderTexture() = default;
	~RenderTexture() = default;

	void Create(uint32_t width, uint32_t height, const Color& color,
		DXGI_FORMAT format, ID3D12Device* device, RTVManager* rtvManager, SRVManager* srvManager);

	//--------- accessor -----------------------------------------------------

	ID3D12Resource* GetResource() const { return resource_.Get(); }

	const RenderTarget& GetRenderTarget() const { return renderTarget_; }

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandle() const { return gpuHandle_; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	RenderTarget renderTarget_;

	ComPtr<ID3D12Resource> resource_;
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle_;

	static int textureCount_;

	//--------- functions ----------------------------------------------------

	void CreateTextureResource(ComPtr<ID3D12Resource>& resource, uint32_t width, uint32_t height,
		const Color& color, DXGI_FORMAT format, ID3D12Device* device);
};
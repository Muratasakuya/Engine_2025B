#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Lib/DxStructures.h>
#include <Engine/Core/Graphics/Lib/ComPtr.h>

// front
class RTVDescriptor;
class SRVDescriptor;

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
		DXGI_FORMAT format, ID3D12Device* device, RTVDescriptor* rtvDescriptor, SRVDescriptor* srvDescriptor);

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

//============================================================================
//	GuiRenderTexture class
//============================================================================
class GuiRenderTexture {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	GuiRenderTexture() = default;
	~GuiRenderTexture() = default;

	void Create(uint32_t width, uint32_t height, DXGI_FORMAT format,
		ID3D12Device* device, SRVDescriptor* srvDescriptor);

	//--------- accessor -----------------------------------------------------

	ID3D12Resource* GetResource() const { return resource_.Get(); }

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandle() const { return gpuHandle_; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	ComPtr<ID3D12Resource> resource_;
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle_;

	static int textureCount_;

	//--------- functions ----------------------------------------------------

	void CreateTextureResource(ComPtr<ID3D12Resource>& resource, uint32_t width, uint32_t height,
		DXGI_FORMAT format, ID3D12Device* device);
};
#include "ShadowMap.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Managers/DSVManager.h>
#include <Engine/Core/Graphics/Managers/SRVManager.h>

//============================================================================
//	ShadowMap classMethods
//============================================================================

void ShadowMap::Create(uint32_t width, uint32_t height, DSVManager* dsvManager, SRVManager* srvManager) {

	// DSV作成
	dsvManager->CreateDSV(width, height, cpuHandle_, resource_,
		DXGI_FORMAT_R32_TYPELESS, DXGI_FORMAT_D32_FLOAT);

	// SRV作成
	uint32_t srvIndex = 0;
	// Descの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvManager->CreateSRV(srvIndex, resource_.Get(), srvDesc);
	gpuHandle_ = srvManager->GetGPUHandle(srvIndex);
}
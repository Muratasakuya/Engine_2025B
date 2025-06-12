#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/ECS/System/Base/ISystem.h>
#include <Engine/Core/ECS/Components/SkyboxComponent.h>

// directX
#include <d3d12.h>

//============================================================================
//	SkyboxRenderSystem class
//============================================================================
class SkyboxRenderSystem :
	public ISystem {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	SkyboxRenderSystem() = default;
	~SkyboxRenderSystem() = default;

	Archetype Signature() const override;

	void Update(EntityManager& entityManager) override;

	void Render(ID3D12GraphicsCommandList* commandList);

	//--------- accessor -----------------------------------------------------

	bool IsCreated() const { return isCreated_; }

	uint32_t GetTextureIndex() const;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	const SkyboxComponent* component_;

	bool isCreated_;
};
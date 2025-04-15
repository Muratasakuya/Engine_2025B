#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Component/PrimitiveMeshComponent.h>
#include <Engine/Core/Component/Base/IComponent.h>

//============================================================================
//	PrimitiveMeshStore class
//============================================================================
class PrimitiveMeshStore :
	public IComponentStore<PrimitiveMeshComponent> {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PrimitiveMeshStore() = default;
	~PrimitiveMeshStore() = default;

	void AddComponent(uint32_t entityId, std::any args) override;
	void RemoveComponent(uint32_t entityId) override;

	void Update() override;

	//--------- accessor -----------------------------------------------------

	PrimitiveMeshComponent* GetComponent(uint32_t entityId) override;
	std::vector<PrimitiveMeshComponent*> GetComponentList([[maybe_unused]] uint32_t entityId) override { return { nullptr }; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::vector<PrimitiveMeshComponent> components_;
};
#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Component/Base/IComponent.h>
#include <Engine/Core/Graphics/GPUObject/DxConstBuffer.h>
#include <Engine/Core/Component/MaterialComponent.h>

// front
class Asset;

//============================================================================
//	MaterialStore class
//============================================================================
class MaterialStore :
	public IComponentStore<MaterialComponent> {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	MaterialStore() = default;
	~MaterialStore() = default;

	void AddComponent(uint32_t entityId, std::any args) override;

	void RemoveComponent(uint32_t entityId) override;

	void Update() override;

	//--------- accessor -----------------------------------------------------

	MaterialComponent* GetComponent(uint32_t entityId) override;
	std::vector<MaterialComponent*> GetComponentList(uint32_t entityId) override;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::vector<std::vector<MaterialComponent>> components_;
};

//============================================================================
//	SpriteMaterialStore class
//============================================================================
class SpriteMaterialStore :
	public IComponentStore<SpriteMaterial> {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	SpriteMaterialStore() = default;
	~SpriteMaterialStore() = default;

	void AddComponent(uint32_t entityId, std::any args) override;

	void RemoveComponent(uint32_t entityId) override;

	void Update() override {};

	//--------- accessor -----------------------------------------------------

	SpriteMaterial* GetComponent(uint32_t entityId) override;
	std::vector<SpriteMaterial*> GetComponentList([[maybe_unused]] uint32_t entityId) override { return { nullptr }; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::vector<SpriteMaterial> components_;
};
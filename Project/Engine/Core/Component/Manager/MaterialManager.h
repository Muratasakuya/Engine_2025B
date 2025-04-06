#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Component/Base/IComponent.h>
#include <Engine/Core/Graphics/GPUObject/DxConstBuffer.h>
#include <Engine/Core/Graphics/GPUObject/CBufferStructures.h>

// front
class Asset;

//============================================================================
//	MaterialManager class
//============================================================================
class MaterialManager :
	public IComponent<Material> {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	MaterialManager() = default;
	~MaterialManager() = default;

	void AddComponent(EntityID entity, std::any args) override;

	void RemoveComponent(EntityID entity) override;

	void Update() override {};

	//--------- accessor -----------------------------------------------------

	Material* GetComponent(EntityID entity) override;
	std::vector<Material*> GetComponentList(EntityID entity) override;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::vector<std::vector<Material>> components_;

	std::unordered_map<EntityID, size_t> entityToIndex_;
	std::vector<EntityID> indexToEntity_;
};

//============================================================================
//	SpriteMaterialManager class
//============================================================================
class SpriteMaterialManager :
	public IComponent<SpriteMaterial> {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	SpriteMaterialManager() = default;
	~SpriteMaterialManager() = default;

	void AddComponent(EntityID entity, std::any args) override;

	void RemoveComponent(EntityID entity) override;

	void Update() override {};

	//--------- accessor -----------------------------------------------------

	SpriteMaterial* GetComponent(EntityID entity) override;
	std::vector<SpriteMaterial*> GetComponentList([[maybe_unused]] EntityID entity) override { return { nullptr }; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::vector<SpriteMaterial> components_;

	std::unordered_map<EntityID, size_t> entityToIndex_;
	std::vector<EntityID> indexToEntity_;
};
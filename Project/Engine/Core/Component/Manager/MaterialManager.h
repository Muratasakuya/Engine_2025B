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
//	MaterialManager class
//============================================================================
class MaterialManager :
	public IComponent<MaterialComponent> {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	MaterialManager() = default;
	~MaterialManager() = default;

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
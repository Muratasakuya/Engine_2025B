#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Lib/ComponentStructures.h>
#include <Engine/Component/Base/IComponent.h>
#include <Engine/Core/CBuffer/DxConstBuffer.h>

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
};
#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Component/ModelComponent.h>
#include <Engine/Component/Base/IComponent.h>

// entityID
using EntityID = uint32_t;

//============================================================================
//	ModelComponentManager class
//============================================================================
class ModelComponentManager :
	public IComponent<ModelComponent> {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	ModelComponentManager() = default;
	~ModelComponentManager() = default;

	void AddComponent(EntityID entity, std::any args) override;
	void RemoveComponent(EntityID entity) override;

	void Update() override {};

	//--------- accessor -----------------------------------------------------

	ModelComponent* GetComponent(EntityID entity) override;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::unordered_map<EntityID, ModelComponent> components_;
};
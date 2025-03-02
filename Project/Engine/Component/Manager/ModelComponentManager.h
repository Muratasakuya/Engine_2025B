#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Component/ModelComponent.h>

// entityID
using EntityID = uint32_t;

//============================================================================
//	ModelComponentManager class
//============================================================================
class ModelComponentManager {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	ModelComponentManager() = default;
	~ModelComponentManager() = default;

	ModelComponent* AddComponent(EntityID entity, const std::string& modelName,
		const std::optional<std::string>& animationName, ID3D12Device* device,
		class Asset* asset, class SRVManager* srvManager);

	void RemoveComponent(EntityID entity);

	//--------- accessor -----------------------------------------------------

	ModelComponent* GetComponent(EntityID entity);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::unordered_map<EntityID, ModelComponent> components_;
};
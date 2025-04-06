#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Component/AnimationComponent.h>
#include <Engine/Core/Component/Base/IComponent.h>

//============================================================================
//	AnimationComponentManager class
//============================================================================
class AnimationComponentManager :
	public IComponent<AnimationComponent> {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	AnimationComponentManager() = default;
	~AnimationComponentManager() = default;

	void AddComponent(EntityID entity, std::any args) override;
	void RemoveComponent(EntityID entity) override;

	void Update() override;

	//--------- accessor -----------------------------------------------------

		//--------- accessor -----------------------------------------------------

	AnimationComponent* GetComponent(EntityID entity) override;
	std::vector<AnimationComponent*> GetComponentList([[maybe_unused]] EntityID entity) override { return { nullptr }; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::vector<AnimationComponent> components_;

	std::unordered_map<EntityID, size_t> entityToIndex_;
	std::vector<EntityID> indexToEntity_;
};
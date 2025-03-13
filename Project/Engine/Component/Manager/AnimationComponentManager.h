#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Component/AnimationComponent.h>
#include <Engine/Component/Base/IComponent.h>

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
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::unordered_map<EntityID, AnimationComponent> components_;
};
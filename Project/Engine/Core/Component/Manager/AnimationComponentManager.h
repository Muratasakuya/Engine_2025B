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

	void AddComponent(uint32_t entityId, std::any args) override;
	void RemoveComponent(uint32_t entityId) override;

	void Update() override;

	//--------- accessor -----------------------------------------------------

		//--------- accessor -----------------------------------------------------

	AnimationComponent* GetComponent(uint32_t entityId) override;
	std::vector<AnimationComponent*> GetComponentList([[maybe_unused]] uint32_t entityId) override { return { nullptr }; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::vector<AnimationComponent> components_;
};
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

	void AddComponent(uint32_t entity, std::any args) override;
	void RemoveComponent(uint32_t entity) override;

	void Update() override;

	//--------- accessor -----------------------------------------------------

		//--------- accessor -----------------------------------------------------

	AnimationComponent* GetComponent(uint32_t entity) override;
	std::vector<AnimationComponent*> GetComponentList([[maybe_unused]] uint32_t entity) override { return { nullptr }; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::vector<AnimationComponent> components_;

	std::unordered_map<uint32_t, size_t> entityToIndex_;
	std::vector<uint32_t> indexToEntity_;
};
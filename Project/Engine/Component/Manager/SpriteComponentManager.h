#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Component/SpriteComponent.h>
#include <Engine/Component/TransformComponent.h>
#include <Engine/Component/Base/IComponent.h>

//============================================================================
//	SpriteComponentManager class
//============================================================================
class SpriteComponentManager :
	public IComponent<SpriteComponent> {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	SpriteComponentManager() = default;
	~SpriteComponentManager() = default;

	void AddComponent(EntityID entity, std::any args) override;
	void RemoveComponent(EntityID entity) override;

	void Update() override;

	//--------- accessor -----------------------------------------------------

	SpriteComponent* GetComponent(EntityID entity) override;
	std::vector<SpriteComponent*> GetComponentList([[maybe_unused]] EntityID entity) override { return { nullptr }; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::vector<std::unique_ptr<SpriteComponent>> components_;
	// spriteのvertex更新用
	std::vector<Transform2DComponent*> transforms_;

	std::unordered_map<EntityID, size_t> entityToIndex_;
	std::vector<EntityID> indexToEntity_;
};
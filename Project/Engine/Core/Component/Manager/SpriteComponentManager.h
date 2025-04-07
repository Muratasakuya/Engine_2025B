#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Component/SpriteComponent.h>
#include <Engine/Core/Component/TransformComponent.h>
#include <Engine/Core/Component/Base/IComponent.h>

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

	void AddComponent(uint32_t entity, std::any args) override;
	void RemoveComponent(uint32_t entity) override;

	void Update() override;

	//--------- accessor -----------------------------------------------------

	SpriteComponent* GetComponent(uint32_t entity) override;
	std::vector<SpriteComponent*> GetComponentList([[maybe_unused]] uint32_t entity) override { return { nullptr }; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::vector<std::unique_ptr<SpriteComponent>> components_;
	// spriteのvertex更新用
	std::vector<Transform2DComponent*> transforms_;

	std::unordered_map<uint32_t, size_t> entityToIndex_;
	std::vector<uint32_t> indexToEntity_;
};
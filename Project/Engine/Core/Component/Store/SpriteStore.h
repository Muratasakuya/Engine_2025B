#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Component/SpriteComponent.h>
#include <Engine/Core/Component/TransformComponent.h>
#include <Engine/Core/Component/Base/IComponent.h>

//============================================================================
//	SpriteStore class
//============================================================================
class SpriteStore :
	public IComponentStore<SpriteComponent> {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	SpriteStore() = default;
	~SpriteStore() = default;

	void AddComponent(uint32_t entityId, std::any args) override;
	void RemoveComponent(uint32_t entityId) override;

	void Update() override;

	//--------- accessor -----------------------------------------------------

	SpriteComponent* GetComponent(uint32_t entityId) override;
	std::vector<SpriteComponent*> GetComponentList([[maybe_unused]] uint32_t entityId) override { return { nullptr }; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::vector<std::unique_ptr<SpriteComponent>> components_;
	// spriteのvertex更新用
	std::vector<Transform2DComponent*> transforms_;
};
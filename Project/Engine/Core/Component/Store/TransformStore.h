#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Component/TransformComponent.h>
#include <Engine/Core/Component/Base/IComponent.h>

//============================================================================
//	Transform3DStore class
//============================================================================
class Transform3DStore :
	public IComponentStore<Transform3DComponent> {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	Transform3DStore() = default;
	~Transform3DStore() = default;

	void AddComponent(uint32_t entityId, std::any args) override;
	void RemoveComponent(uint32_t entityId) override;

	void Update() override;

	//--------- accessor -----------------------------------------------------

	Transform3DComponent* GetComponent(uint32_t entityId) override;
	std::vector<Transform3DComponent*> GetComponentList([[maybe_unused]] uint32_t entityId) override { return { nullptr }; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::vector<Transform3DComponent> components_;
};

//============================================================================
//	Transform2DStore class
//============================================================================
class Transform2DStore :
	public IComponentStore<Transform2DComponent> {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	Transform2DStore() = default;
	~Transform2DStore() = default;

	void AddComponent(uint32_t entityId, std::any args) override;
	void RemoveComponent(uint32_t entityId) override;

	void Update() override;

	//--------- accessor -----------------------------------------------------

	Transform2DComponent* GetComponent(uint32_t entityId) override;
	std::vector<Transform2DComponent*> GetComponentList([[maybe_unused]] uint32_t entityId) override { return { nullptr }; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::vector<std::unique_ptr<Transform2DComponent>> components_;
};
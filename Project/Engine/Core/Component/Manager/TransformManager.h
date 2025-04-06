#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Component/TransformComponent.h>
#include <Engine/Core/Component/Base/IComponent.h>

//============================================================================
//	Transform3DManager class
//============================================================================
class Transform3DManager :
	public IComponent<Transform3DComponent> {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	Transform3DManager() = default;
	~Transform3DManager() = default;

	void AddComponent(EntityID entity, std::any args) override;
	void RemoveComponent(EntityID entity) override;

	void Update() override;

	//--------- accessor -----------------------------------------------------

	Transform3DComponent* GetComponent(EntityID entity) override;
	std::vector<Transform3DComponent*> GetComponentList([[maybe_unused]] EntityID entity) override { return { nullptr }; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::vector<Transform3DComponent> components_;

	std::unordered_map<EntityID, size_t> entityToIndex_;
	std::vector<EntityID> indexToEntity_;
};

//============================================================================
//	Transform2DManager class
//============================================================================
class Transform2DManager :
	public IComponent<Transform2DComponent> {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	Transform2DManager() = default;
	~Transform2DManager() = default;

	void AddComponent(EntityID entity, std::any args) override;
	void RemoveComponent(EntityID entity) override;

	void Update() override;

	//--------- accessor -----------------------------------------------------

	Transform2DComponent* GetComponent(EntityID entity) override;
	std::vector<Transform2DComponent*> GetComponentList([[maybe_unused]] EntityID entity) override { return { nullptr }; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::vector<std::unique_ptr<Transform2DComponent>> components_;

	std::unordered_map<EntityID, size_t> entityToIndex_;
	std::vector<EntityID> indexToEntity_;
};
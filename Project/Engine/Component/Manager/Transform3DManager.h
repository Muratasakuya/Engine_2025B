#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Component/TransformComponent.h>
#include <Engine/Component/Base/IComponent.h>
#include <Engine/Core/CBuffer/DxConstBuffer.h>

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
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::unordered_map<EntityID, Transform3DComponent> components_;
};
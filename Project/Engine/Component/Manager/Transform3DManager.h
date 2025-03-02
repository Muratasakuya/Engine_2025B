#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Component/TransformComponent.h>
#include <Engine/Core/CBuffer/DxConstBuffer.h>

// entityID
using EntityID = uint32_t;

//============================================================================
//	Transform3DManager class
//============================================================================
class Transform3DManager {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	Transform3DManager() = default;
	~Transform3DManager() = default;

	Transform3DComponent* AddComponent(EntityID entity, ID3D12Device* device);

	void RemoveComponent(EntityID entity);

	void Update();

	//--------- accessor -----------------------------------------------------

	Transform3DComponent* GetComponent(EntityID entity);
	const DxConstBuffer<TransformationMatrix>& GetBuffer(EntityID entity) const;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::unordered_map<EntityID, Transform3DComponent> components_;
	std::unordered_map<EntityID, DxConstBuffer<TransformationMatrix>> buffers_;
};
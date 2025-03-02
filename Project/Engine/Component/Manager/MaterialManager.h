#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Lib/ComponentStructures.h>
#include <Engine/Core/CBuffer/DxConstBuffer.h>

// entityID
using EntityID = uint32_t;

//============================================================================
//	MaterialManager class
//============================================================================
class MaterialManager {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	MaterialManager() = default;
	~MaterialManager() = default;

	std::vector<Material*> AddComponent(EntityID entity, size_t meshNum, ID3D12Device* device);

	void RemoveComponent(EntityID entity);

	void Update();

	//--------- accessor -----------------------------------------------------

	std::vector<Material*> GetComponent(EntityID entity);
	const std::vector<DxConstBuffer<Material>>& GetBuffer(EntityID entity) const;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::unordered_map<EntityID, std::vector<Material>> components_;
	std::unordered_map<EntityID, std::vector<DxConstBuffer<Material>>> buffers_;
};
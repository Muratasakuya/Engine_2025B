#pragma once

//============================================================================
//	include
//============================================================================

// c++
#include <cstdint>
#include <string>
#include <unordered_map>

// entityID
using EntityID = uint32_t;

//============================================================================
//	EntityManager class
//============================================================================
class EntityManager {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	EntityManager() = default;
	~EntityManager() = default;

	EntityID CreateEntity(const std::string& name);

	void DestroyEntity(EntityID id);

	//--------- accessor -----------------------------------------------------

	const std::unordered_map<EntityID, std::string>& GetNames() const { return entityNames_; }

	EntityID GetEntityCount() const { return static_cast<EntityID>(entityNames_.size()); }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	EntityID nextId_;
	std::unordered_map<EntityID, std::string> entityNames_;

	std::unordered_map<std::string, int> nameCounts_;

	//--------- functions ----------------------------------------------------

	std::string CheckName(const std::string& name);
	std::string SplitBaseNameAndNumber(const std::string& name, int& number);
};
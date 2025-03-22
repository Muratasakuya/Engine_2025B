#pragma once

//============================================================================
//	include
//============================================================================

// c++
#include <cstdint>
#include <string>
#include <unordered_map>
#include <optional>
#include <algorithm>

// entityID
using EntityID = uint32_t;

//============================================================================
//	EntityManager class
//============================================================================
class EntityManager {
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- structure ----------------------------------------------------

	struct EntityInformation {

		std::string name;
		std::optional<std::string> groupName;
	};
public:
	//========================================================================
	//	public Methods
	//========================================================================

	EntityManager() = default;
	~EntityManager() = default;

	EntityID CreateEntity(const std::string& name,
		const std::optional<std::string>& groupName);

	void RemoveEntity(EntityID id);

	//--------- accessor -----------------------------------------------------

	const std::vector<EntityInformation>& GetNames() const { return entityInformations_; }

	EntityID GetEntityCount() const { return static_cast<EntityID>(entityInformations_.size()); }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	EntityID nextId_;
	std::vector<EntityInformation> entityInformations_;

	std::unordered_map<std::string, int> nameCounts_;

	//--------- functions ----------------------------------------------------

	std::string CheckName(const std::string& name);
	std::string SplitBaseNameAndNumber(const std::string& name, int& number);
};
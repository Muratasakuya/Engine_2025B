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

//============================================================================
//	EntityRegistry class
//============================================================================
class EntityRegistry {
public:
	//========================================================================
	//	public Methods
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

	EntityRegistry() = default;
	~EntityRegistry() = default;

	uint32_t CreateEntity(const std::string& name,
		const std::optional<std::string>& groupName);

	void RemoveEntity(uint32_t id);

	//--------- accessor -----------------------------------------------------

	const std::vector<EntityInformation>& GetNames() const { return entityInformations_; }

	const std::vector<uint32_t>& GetIndexToEntity() const { return indexToEntity_; }

	uint32_t GetIndex(uint32_t id) const;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	uint32_t nextId_;
	std::vector<EntityInformation> entityInformations_;

	std::unordered_map<uint32_t, uint32_t> entityToIndex_;
	std::vector<uint32_t> indexToEntity_;

	std::unordered_map<std::string, int> nameCounts_;

	//--------- functions ----------------------------------------------------

	std::string CheckName(const std::string& name);
	std::string SplitBaseNameAndNumber(const std::string& name, int& number);
};
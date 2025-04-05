#define NOMINMAX

#include "EntityManager.h"

//============================================================================
//	include
//============================================================================
#include <Lib/MathUtils/Algorithm.h>

//============================================================================
//	EntityManager classMethods
//============================================================================

EntityID EntityManager::CreateEntity(const std::string& name,
	const std::optional<std::string>& groupName) {

	EntityID id = nextId_;
	++nextId_;
	std::string uniqueName = CheckName(name);

	EntityID index = static_cast<EntityID>(entityInformations_.size());
	entityInformations_.emplace_back(EntityInformation{ uniqueName, groupName });
	entityToIndex_[id] = index;
	indexToEntity_.push_back(id);

	return id;
}

void EntityManager::RemoveEntity(EntityID id) {

	if (!Algorithm::Find(entityToIndex_, id)) {
		return;
	}

	auto it = entityToIndex_.find(id);
	EntityID index = it->second;
	EntityID lastIndex = static_cast<EntityID>(entityInformations_.size() - 1);

	if (index != lastIndex) {

		// 末尾と交換
		std::swap(entityInformations_[index], entityInformations_[lastIndex]);

		// 交換されたentityIdを更新
		EntityID movedId = indexToEntity_[lastIndex];
		indexToEntity_[index] = movedId;
		entityToIndex_[movedId] = index;
	}

	// 削除
	entityInformations_.pop_back();
	indexToEntity_.pop_back();
	entityToIndex_.erase(id);
}

EntityID EntityManager::GetIndex(EntityID id) const {

	if (Algorithm::Find(entityToIndex_, id, true)) {

		return entityToIndex_.at(id);
	}
	return UINT();
}

std::string EntityManager::CheckName(const std::string& name) {

	int trailingNumber = 0;
	std::string base = SplitBaseNameAndNumber(name, trailingNumber);

	int& count = nameCounts_[base];

	if (trailingNumber > count) {
		count = trailingNumber;
	}

	std::string uniqueName;
	if (count == 0) {

		uniqueName = base;
	} else {

		uniqueName = base + std::to_string(count);
	}

	count++;

	return uniqueName;
}

std::string EntityManager::SplitBaseNameAndNumber(const std::string& name, int& number) {

	int idx = static_cast<int>(name.size()) - 1;
	while (idx >= 0 && std::isdigit(name[idx])) {
		idx--;
	}

	int startOfDigits = idx + 1;
	if (startOfDigits < static_cast<int>(name.size())) {

		// 末尾に数字がある場合
		number = std::stoi(name.substr(startOfDigits));
	} else {

		// 末尾に数字が無い場合
		number = 0;
	}

	return name.substr(0, startOfDigits);
}
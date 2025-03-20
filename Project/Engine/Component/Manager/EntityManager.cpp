#define NOMINMAX

#include "EntityManager.h"

//============================================================================
//	EntityManager classMethods
//============================================================================

EntityID EntityManager::CreateEntity(const std::string& name) {

	EntityID  id = nextId_;
	++nextId_;

	// 重複しないよう名前を生成
	std::string uniqueName = CheckName(name);
	entityNames_.resize(std::max(static_cast<EntityID>(entityNames_.size()), id + 1));
	entityNames_[id] = uniqueName;

	return id;
}

void EntityManager::RemoveEntity(EntityID id) {

	// entity削除
	entityNames_.erase(entityNames_.begin() + id);
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
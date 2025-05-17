#include "TagSystem.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/ECS/Core/EntityManager.h>
#include <Engine/Core/ECS/Components/TransformComponent.h>

//============================================================================
//	TagSystem classMethods
//============================================================================

Archetype TagSystem::Signature() const {

	Archetype arch{};
	arch.set(EntityManager::GetTypeID<TagComponent>());
	arch.set(EntityManager::GetTypeID<Transform3DComponent>()); // 3D
	return arch;
}

void TagSystem::Update(EntityManager& entityManager) {

	groups_.clear();
	idToTag_.clear();

	for (uint32_t entity : entityManager.View(Signature())) {

		auto* tag = entityManager.GetComponent<TagComponent>(entity);
		idToTag_[entity] = tag;
		groups_[tag->groupName].push_back(entity);
	}
}

std::string TagSystem::CheckName(const std::string& name) {

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

std::string TagSystem::SplitBaseNameAndNumber(const std::string& name, int& number) {

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
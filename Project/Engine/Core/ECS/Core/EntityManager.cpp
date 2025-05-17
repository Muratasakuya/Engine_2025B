#include "EntityManager.h"

//============================================================================
//	include
//============================================================================

// components
#include <Engine/Core/Component/TransformComponent.h>
#include <Engine/Core/Component/MaterialComponent.h>
#include <Engine/Core/Component/AnimationComponent.h>
#include <Engine/Core/ECS/Components/TagComponent.h>

//============================================================================
//	EntityManager classMethods
//============================================================================

uint32_t EntityManager::Create() {

	return alive_.empty() ? next_++ : PopAlive();
}

void EntityManager::Destroy(uint32_t entity) {

	// componentは各poolが持っているためarchetypeをクリア
	if (auto it = entityToArch_.find(entity); it != entityToArch_.end()) {

		archToEntities_[it->second].erase(
			std::remove(archToEntities_[it->second].begin(), archToEntities_[it->second].end(), entity),
			archToEntities_[it->second].end());
		entityToArch_.erase(it);
	}
	alive_.push_back(entity);
}

std::vector<uint32_t> EntityManager::View(const Archetype& mask) const {

	std::vector<uint32_t> result;
	for (auto& [arch, list] : archToEntities_) {
		if ((arch & mask) == mask) {

			result.insert(result.end(), list.begin(), list.end());
		}
	}
	return result;
}

template<class T>
void EntityManager::SetBit(uint32_t entity, bool enable) {

	size_t id = GetTypeID<T>();

	Archetype& arch = entityToArch_[entity];   // 旧アーキタイプ参照
	Archetype  old = arch;

	arch.set(id, enable);

	// 旧リスト削除
	auto& oldList = archToEntities_[old];
	oldList.erase(std::remove(oldList.begin(), oldList.end(), entity), oldList.end());

	// 新リスト追加
	archToEntities_[arch].push_back(entity);
}
// 各componentを明示的にインスタンス化
template void EntityManager::SetBit<class Transform3DComponent>(uint32_t, bool);
template void EntityManager::SetBit<class MaterialComponent>(uint32_t, bool);
template void EntityManager::SetBit<class AnimationComponent>(uint32_t, bool);
template void EntityManager::SetBit<struct TagComponent>(uint32_t, bool);

uint32_t EntityManager::PopAlive() {

	uint32_t entity = alive_.back(); alive_.pop_back(); return entity;
}
#include "EntityManager.h"

//============================================================================
//	include
//============================================================================
#include <Lib/MathUtils/Algorithm.h>

// components
#include <Engine/Core/ECS/Components/TransformComponent.h>
#include <Engine/Core/ECS/Components/MaterialComponent.h>
#include <Engine/Core/ECS/Components/AnimationComponent.h>
#include <Engine/Core/ECS/Components/TagComponent.h>
#include <Engine/Core/ECS/Components/SpriteComponent.h>

//============================================================================
//	EntityManager classMethods
//============================================================================

uint32_t EntityManager::Create() {

	return alive_.empty() ? next_++ : PopAlive();
}

void EntityManager::Destroy(uint32_t entity) {

	Archetype arch = entityToArch_[entity];

	// bitが立っている全タイプのpoolからRemove
	for (size_t typeId = 0; typeId < kMaxComponentTypes; ++typeId) {
		if (!arch.test(typeId) || typeId >= pools_.size()) {
			continue;
		}
		if (pools_[typeId]) {

			pools_[typeId]->Remove(entity);
		}
	}

	// archetypeテーブルから除去
	if (auto it = entityToArch_.find(entity); it != entityToArch_.end()) {

		archToEntities_[it->second].erase(
			std::remove(archToEntities_[it->second].begin(),
				archToEntities_[it->second].end(), entity),
			archToEntities_[it->second].end());
		entityToArch_.erase(it);
	}
	// 再利用キューへ
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

void EntityManager::ImGui() {

	// 登録済みプールを列挙
	for (size_t id = 0; id < pools_.size(); ++id) {
		if (!pools_[id]) continue;  // 未生成

		// 型名を取るなら RTTI 等を使う、ここではインデックスで表示
		std::string label = std::string(typeid(*pools_[id]).name()) + " [" + std::to_string(id) + "]";
		label = Algorithm::RemoveSubstring(label, "class");
		label = Algorithm::RemoveSubstring(label, "struct");
		label = Algorithm::RemoveSubstring(label, "ComponentPool");
		pools_[id]->Debug(label.c_str());
	}
}

template<class T>
void EntityManager::SetBit(uint32_t entity, bool enable) {

	size_t id = GetTypeID<T>();

	Archetype& arch = entityToArch_[entity];
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
template void EntityManager::SetBit<class Transform2DComponent>(uint32_t, bool);
template void EntityManager::SetBit<class MaterialComponent>(uint32_t, bool);
template void EntityManager::SetBit<class SpriteMaterialComponent>(uint32_t, bool);
template void EntityManager::SetBit<class AnimationComponent>(uint32_t, bool);
template void EntityManager::SetBit<class SpriteComponent>(uint32_t, bool);
template void EntityManager::SetBit<class SkyboxComponent>(uint32_t, bool);
template void EntityManager::SetBit<struct TagComponent>(uint32_t, bool);

uint32_t EntityManager::PopAlive() {

	uint32_t entity = alive_.back(); alive_.pop_back(); return entity;
}
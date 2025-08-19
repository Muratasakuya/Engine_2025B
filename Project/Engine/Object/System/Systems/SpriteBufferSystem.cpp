#include "SpriteBufferSystem.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Object/Core/ObjectPoolManager.h>
#include <Engine/Object/Data/Transform.h>
#include <Engine/Object/Data/Material.h>

//============================================================================
//	SpriteBufferSystem classMethods
//============================================================================

Archetype SpriteBufferSystem::Signature() const {

	Archetype arch{};
	arch.set(ObjectPoolManager::GetTypeID<Transform2D>());
	arch.set(ObjectPoolManager::GetTypeID<SpriteMaterial>());
	arch.set(ObjectPoolManager::GetTypeID<Sprite>());
	return arch;
}

void SpriteBufferSystem::Update(ObjectPoolManager& ObjectPoolManager) {

	// データクリア
	spriteDataMap_.clear();

	const auto& view = ObjectPoolManager.View(Signature());

	for (const auto& object : view) {

		auto* transform = ObjectPoolManager.GetData<Transform2D>(object);
		auto* material = ObjectPoolManager.GetData<SpriteMaterial>(object);
		auto* sprite = ObjectPoolManager.GetData<Sprite>(object);

		// spriteの更新処理
		sprite->UpdateVertex(*transform);

		// mapに追加
		spriteDataMap_[sprite->GetLayer()].push_back(SpriteData(transform, material, sprite));
	}

	// 描画順インデックスでソートを行う
	for (auto& [phase, vector] : spriteDataMap_) {
		std::stable_sort(vector.begin(), vector.end(),
			[](const SpriteData& a, const SpriteData& b) {
				return a.sprite->GetLayerIndex() < b.sprite->GetLayerIndex();
			});
	}
}
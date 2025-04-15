#include "PrimitiveMeshStore.h"

//============================================================================
//	include
//============================================================================
#include <Lib/MathUtils/Algorithm.h>

//============================================================================
//	PrimitiveMeshStore classMethods
//============================================================================

void PrimitiveMeshStore::AddComponent(uint32_t entityId, std::any args) {

	size_t index = components_.size();

	auto [device, asset, modelName] =
		std::any_cast<std::tuple<ID3D12Device*, Asset*, std::string>>(args);

	// component追加
	components_.emplace_back();
	components_.back().Init(device, asset, modelName);

	// index設定
	SetEntityIndex(entityId, index);
}

void PrimitiveMeshStore::RemoveComponent(uint32_t entityId) {

	// 見つからなければ削除しない
	if (!Algorithm::Find(entityToIndex_, entityId)) {
		return;
	}

	// indexを末尾と交換して削除
	size_t index = entityToIndex_.at(entityId);
	size_t lastIndex = components_.size() - 1;

	// entityIndex削除
	SwapToPopbackIndex(entityId, lastIndex);

	// component削除
	std::swap(components_[index], components_[lastIndex]);
	components_.pop_back();
}

void PrimitiveMeshStore::Update() {
	// 今のところ更新処理はなし
}

PrimitiveMeshComponent* PrimitiveMeshStore::GetComponent(uint32_t entityId) {

	size_t index = entityToIndex_[entityId];
	return &components_[index];
}

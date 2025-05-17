#pragma once

//============================================================================
//	include
//============================================================================

// c++
#include <memory>
#include <vector>
#include <bitset>
#include <unordered_map>
#include <typeindex>

// componentBitSize
constexpr size_t kMaxComponentTypes = 128;
using Archetype = std::bitset<kMaxComponentTypes>;

//============================================================================
//	ComponentPool class
//============================================================================
template<class T, bool kMultiple = false>
class ComponentPool {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	ComponentPool() = default;
	~ComponentPool() = default;

	//--------- variables ----------------------------------------------------

	// entity -> index
	std::unordered_map<uint32_t, size_t> entityToIndex_;
	// index -> entity
	std::vector<uint32_t> indexToEntity_;

	// componentData
	using Storage = std::conditional_t<kMultiple, std::vector<T>, T>;
	std::vector<Storage> data_;

	//--------- functions ----------------------------------------------------

	// 追加
	template<class... Args>
	void Add(uint32_t entity, Args&&... args);
	// 削除
	void Remove(uint32_t entity);
	// component取得
	Storage* Get(uint32_t entity);
};

//============================================================================
//	ComponentPool templateMethods
//============================================================================

template<class T, bool kMultiple>
template<class ...Args>
inline void ComponentPool<T, kMultiple>::Add(uint32_t entity, Args && ...args) {

	// entity追加処理
	size_t index = data_.size();
	entityToIndex_[entity] = index;
	indexToEntity_.push_back(entity);
	// Tが配列か判定
	if constexpr (kMultiple) {

		// vector<T>配列処理
		data_.emplace_back(Storage{ std::forward<Args>(args)... });
	} else {

		// T、配列じゃない
		data_.emplace_back(std::forward<Args>(args)...);
	}
}

template<class T, bool kMultiple>
inline void ComponentPool<T, kMultiple>::Remove(uint32_t entity) {

	// 存在しないentityの場合処理しない
	auto it = entityToIndex_.find(entity);
	if (it == entityToIndex_.end()) {
		return;
	}

	size_t index = it->second;
	size_t last = data_.size() - 1;
	if (index != last) {

		std::swap(data_[index], data_[last]);
		uint32_t moved = indexToEntity_[last];
		entityToIndex_[moved] = index;
		indexToEntity_[index] = moved;
	}
	data_.pop_back();
	indexToEntity_.pop_back();
	entityToIndex_.erase(it);
}

template<class T, bool kMultiple>
inline ComponentPool<T, kMultiple>::Storage* ComponentPool<T, kMultiple>::Get(uint32_t entity) {

	auto it = entityToIndex_.find(entity);
	return (it != entityToIndex_.end()) ? &data_[it->second] : nullptr;
}
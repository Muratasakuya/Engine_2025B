#pragma once

//============================================================================
//	include
//============================================================================

// c++
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <any>

//============================================================================
//	IComponentBase class
//============================================================================
class IComponentBase {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	IComponentBase() = default;
	virtual ~IComponentBase() = default;

	virtual void Update() = 0;
};

//============================================================================
//	IComponentStore class
//============================================================================
template <typename T>
class IComponentStore :
	public IComponentBase {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	IComponentStore() = default;
	virtual ~IComponentStore() = default;

	virtual void AddComponent(uint32_t entityId, std::any args) = 0;
	virtual void RemoveComponent(uint32_t entityId) = 0;

	void SetEntityIndex(uint32_t entityId, size_t currentComponentIndex);

	void SwapToPopbackIndex(uint32_t entityId, size_t lastIndex);

	//--------- accessor -----------------------------------------------------

	virtual T* GetComponent(uint32_t entityId) = 0;
	virtual std::vector<T*> GetComponentList(uint32_t entityId) = 0;
protected:
	//========================================================================
	//	protected Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// entityIdからcomponent配列のindexを求めるmap
	std::unordered_map<uint32_t, size_t> entityToIndex_;
	// component配列のindexからentityIdを求めるためのtable
	std::vector<uint32_t> indexToEntity_;
};

//============================================================================
//	IComponentStore templateMethods
//============================================================================

template<typename T>
inline void IComponentStore<T>::SetEntityIndex(uint32_t entityId, size_t currentComponentIndex) {

	// index設定
	entityToIndex_[entityId] = currentComponentIndex;
	indexToEntity_.push_back(entityId);
}

template<typename T>
inline void IComponentStore<T>::SwapToPopbackIndex(uint32_t entityId, size_t lastIndex) {

	size_t index = entityToIndex_.at(entityId);

	if (index != lastIndex) {

		// 交換されたentityIdを更新
		uint32_t movedEntityId = indexToEntity_[lastIndex];
		entityToIndex_[movedEntityId] = index;
		indexToEntity_[index] = movedEntityId;
	}

	// 末尾を削除
	indexToEntity_.pop_back();
	entityToIndex_.erase(entityId);
}
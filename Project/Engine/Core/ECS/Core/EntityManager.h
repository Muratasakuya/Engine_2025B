#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/ECS/Core/ComponentPool.h>

//============================================================================
//	EntityManager class
//============================================================================
class EntityManager {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	EntityManager() = default;
	~EntityManager() = default;

	// entity作成
	uint32_t Create();
	// entity削除
	void Destroy(uint32_t entity);

	template<class T, bool Flag = false>
	ComponentPool<T, Flag>& GetPool();

	// component追加
	template<class T, bool Flag = false, class... Args>
	typename ComponentPool<T, Flag>::Storage* AddComponent(uint32_t entity, Args&&... args);
	// component削除
	template<class T, bool Flag = false>
	void RemoveComponent(uint32_t entity);

	std::vector<uint32_t> View(const Archetype& mask) const;

	//--------- accessor -----------------------------------------------------

	template<class T>
	static size_t GetTypeID();

	template<class T, bool Flag = false>
	typename ComponentPool<T, Flag>::Storage* GetComponent(uint32_t entity);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::unordered_map<uint32_t, Archetype> entityToArch_;
	std::unordered_map<Archetype, std::vector<uint32_t>> archToEntities_;

	std::vector<std::shared_ptr<void>> pools_;

	uint32_t next_ = 1;
	std::vector<uint32_t> alive_;

	static inline size_t typeCounter_ = 0;

	//--------- functions ----------------------------------------------------

	template<class T>
	void SetBit(uint32_t entity, bool enable);

	uint32_t PopAlive();
};

template<class T, bool Flag>
inline ComponentPool<T, Flag>& EntityManager::GetPool() {

	size_t id = GetTypeID<T>();
	if (id >= pools_.size()) {
		pools_.resize(id + 1);
	}

	if (!pools_[id]) {
		pools_[id] = std::make_shared<ComponentPool<T, Flag>>();
	}

	return *static_cast<ComponentPool<T, Flag>*>(pools_[id].get());
}

template<class T, bool Flag, class... Args>
inline auto EntityManager::AddComponent(uint32_t entity, Args&&... args)
-> typename ComponentPool<T, Flag>::Storage* {

	auto& pool = GetPool<T, Flag>();
	pool.Add(entity, std::forward<Args>(args)...);
	SetBit<T>(entity, true);
	return GetComponent<T, Flag>(entity);
}

template<class T, bool Flag>
inline void EntityManager::RemoveComponent(uint32_t entity) {

	GetPool<T, Flag>().Remove(entity);
	SetBit<T>(entity, false);
}

template<class T, bool Flag>
inline typename ComponentPool<T, Flag>::Storage* EntityManager::GetComponent(uint32_t entity) {

	return GetPool<T, Flag>().Get(entity);
}

template<class T>
inline size_t EntityManager::GetTypeID() {

	static size_t id = ++typeCounter_;
	return id;
}
#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Object/System/Base/ISystem.h>

// c++
#include <unordered_map>
#include <algorithm>
#include <ranges>

//============================================================================
//	SystemManager class
//============================================================================
class SystemManager {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	SystemManager() = default;
	~SystemManager() = default;

	template<class T, class... Args>
	void AddSystem(Args&&... args);

	void UpdateData(ObjectPoolManager& ObjectPoolManager);

	void UpdateBuffer(ObjectPoolManager& ObjectPoolManager);

	//--------- accessor -----------------------------------------------------

	template<class T>
	T* GetSystem() const;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::unordered_map<std::type_index, std::unique_ptr<ISystem>> systems_;
};

template<class T, class ...Args>
inline void SystemManager::AddSystem(Args&& ...args) {

	// system登録
	const std::type_index key = typeid(T);
	auto system = std::make_unique<T>(std::forward<Args>(args)...);
	systems_.emplace(key, std::move(system));
}

template<typename T>
inline T* SystemManager::GetSystem() const {

	if (auto it = systems_.find(std::type_index(typeid(T))); it != systems_.end()) {

		return static_cast<T*>(it->second.get());
	}
	return nullptr;
}
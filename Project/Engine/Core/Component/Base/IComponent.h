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
//	IComponentManager class
//============================================================================
class IComponentManager {
public:

	IComponentManager() = default;
	virtual ~IComponentManager() = default;

	virtual void AddComponent(uint32_t id, std::any args) = 0;
	virtual void RemoveComponent(uint32_t id) = 0;

	virtual void Update() = 0;
};

//============================================================================
//	IComponent class
//============================================================================
template <typename T>
class IComponent :
	public IComponentManager {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	IComponent() = default;
	virtual ~IComponent() = default;

	//--------- accessor -----------------------------------------------------

	virtual T* GetComponent(uint32_t entity) = 0;
	virtual std::vector<T*> GetComponentList(uint32_t entity) = 0;
};
#pragma once

//============================================================================
//	include
//============================================================================

// c++
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <any>
// using
using EntityID = uint32_t;

//============================================================================
//	IComponentManager class
//============================================================================
class IComponentManager {
public:

	IComponentManager() = default;
	virtual ~IComponentManager() = default;

	virtual void AddComponent(EntityID id, std::any args) = 0;
	virtual void RemoveComponent(EntityID id) = 0;

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

	virtual T* GetComponent(EntityID entity) = 0;
	virtual std::vector<T*> GetComponentList(EntityID entity) = 0;
};
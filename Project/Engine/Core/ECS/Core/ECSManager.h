#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/ECS/Core/EntityManager.h>
#include <Engine/Core/ECS/System/Manager/SystemManager.h>

// directX
#include <d3d12.h>
// c++
#include <optional>
#include <string>
// front
class Asset;
class DxCommand;

//============================================================================
//	ECSManager class
//============================================================================
class ECSManager {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	ECSManager() = default;
	~ECSManager() = default;

	void Init(ID3D12Device* device, Asset* asset, DxCommand* dxCommand);

	void UpdateComponent();
	void UpdateBuffer();

	// entity追加
	// helper
	uint32_t CreateObject3D(const std::string& modelName, const std::string& name,
		const std::string& groupName, const std::optional<std::string>& animationName = std::nullopt);

	// entity削除
	void Destroy(uint32_t entity);

	//--------- accessor -----------------------------------------------------

	template<class T, bool Flag = false>
	typename ComponentPool<T, Flag>::Storage* GetComponent(uint32_t entity);

	template<class T>
	T* GetSystem() const;

	// singleton
	static ECSManager* GetInstance();
	static void Finalize();
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	Asset* asset_;

	static ECSManager* instance_;

	std::unique_ptr<EntityManager> entityManager_;

	std::unique_ptr<SystemManager> systemManager_;

	//--------- functions ----------------------------------------------------

	uint32_t BuildEmptyEntity(const std::string& name, const std::string& groupName);
};

//============================================================================
//	ECSManager templateMethods
//============================================================================

template<class T, bool Flag>
inline typename ComponentPool<T, Flag>::Storage* ECSManager::GetComponent(uint32_t entity) {

	return entityManager_->GetComponent<T, Flag>(entity);
}

template<class T>
inline T* ECSManager::GetSystem() const {

	return systemManager_->GetSystem<T>();
}
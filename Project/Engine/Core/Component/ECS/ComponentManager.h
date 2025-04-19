#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Component/ECS/EntityRegistry.h>
#include <Engine/Core/Component/Base/IComponent.h>
#include <Lib/MathUtils/Algorithm.h>

// directX
#include <d3d12.h>
// c++
#include <cstdint>
#include <array>
#include <variant>
#include <typeindex>
#include <unordered_map>
// front
class Asset;
class GPUObjectSystem;

//============================================================================
//	enum class
//============================================================================

// componentの種類
enum class ComponentType
	: uint32_t {
	Object3D,
	Object2D,
	Effect,

	Count
};
static constexpr uint32_t kComponentCount = static_cast<uint32_t>(ComponentType::Count);

//============================================================================
//	ComponentManager class
//============================================================================
class ComponentManager {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	void Init(ID3D12Device* device, Asset* asset, GPUObjectSystem* gpuObjectSystem);

	void Update();

	// componentStoreの追加
	template <typename T>
	void RegisterComponentStore(IComponentStore<T>* manager);

	//--------- object3D -----------------------------------------------------

	// 追加、作成処理
	uint32_t CreateObject3D(const std::string& modelName, const std::string& objectName,
		const std::optional<std::string>& groupName,
		const std::optional<std::string>& animationName);
	// 指定されたidのentity削除
	void RemoveObject3D(uint32_t id);

	//---------- effect ------------------------------------------------------

	// 追加、作成処理
	uint32_t CreateEffect(const std::string& modelName, const std::string& textureName,
		const std::string& objectName, const std::optional<std::string>& groupName = std::nullopt);
	// 指定されたidのentity削除
	void RemoveEffect(uint32_t entityId);

	//--------- object2D -----------------------------------------------------

	// 追加、作成処理
	uint32_t CreateObject2D(const std::string& textureName, const std::string& objectName,
		const std::optional<std::string>& groupName = std::nullopt);
	// 指定されたidのentity削除
	void RemoveObject2D(uint32_t entityId);

	//--------- accessor -----------------------------------------------------

	// singleton
	static ComponentManager* GetInstance();
	static void Finalize();

	// componentの取得
	template <typename T>
	T* GetComponent(uint32_t entityId);
	template <typename T>
	std::vector<T*> GetComponentList(uint32_t entityId);

	// entityの数取得
	const std::vector<uint32_t>& GetEntityList(ComponentType type) const;

	uint32_t GetEntityIndex(ComponentType type, uint32_t entityId) const;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//- dependencies variables -----------------------------------------------

	ID3D12Device* device_;
	ID3D12GraphicsCommandList* commandList_;
	Asset* asset_;
	GPUObjectSystem* gpuObjectSystem_;

	//--------- variables ----------------------------------------------------

	static ComponentManager* instance_;

	// entity番号の管理
	std::array<std::unique_ptr<EntityRegistry>, kComponentCount> entityRegistries_;
	// 登録済みのcomponentStore
	std::unordered_map<std::type_index, void*> componentStores_;

	//--------- functions ----------------------------------------------------

	template <typename T, typename... Args>
	void AddComponent(uint32_t entityId, Args&&... args);
	template <typename T>
	void RemoveComponent(uint32_t entityId);

	ComponentManager() = default;
	~ComponentManager() = default;
	ComponentManager(const ComponentManager&) = delete;
	ComponentManager& operator=(const ComponentManager&) = delete;
};

//============================================================================
//	ComponentManager templateMethods
//============================================================================

template<typename T>
inline void ComponentManager::RegisterComponentStore(IComponentStore<T>* manager) {

	ASSERT(manager, "not create this ComponentClass");

	componentStores_[std::type_index(typeid(T))] = manager;
}

template<typename T, typename ...Args>
inline void ComponentManager::AddComponent(uint32_t entityId, Args && ...args) {

	if ((Algorithm::Find(componentStores_, std::type_index(typeid(T)), true))) {

		static_cast<IComponentStore<T>*>(componentStores_.at(std::type_index(typeid(T))))->AddComponent(entityId, std::make_tuple(std::forward<Args>(args)...));
	}
}

template<typename T>
inline void ComponentManager::RemoveComponent(uint32_t entityId) {

	if (Algorithm::Find(componentStores_, std::type_index(typeid(T)), true)) {

		static_cast<IComponentStore<T>*>(componentStores_.at(std::type_index(typeid(T))))->RemoveComponent(entityId);
	}
}

template<typename T>
inline T* ComponentManager::GetComponent(uint32_t entityId) {

	if (Algorithm::Find(componentStores_, std::type_index(typeid(T)), true)) {

		return static_cast<IComponentStore<T>*>(componentStores_.at(std::type_index(typeid(T))))->GetComponent(entityId);
	}
	return nullptr;
}

template<typename T>
inline std::vector<T*> ComponentManager::GetComponentList(uint32_t entityId) {

	if (Algorithm::Find(componentStores_, std::type_index(typeid(T)), true)) {

		return static_cast<IComponentStore<T>*>(componentStores_.at(std::type_index(typeid(T))))->GetComponentList(entityId);
	}
	return { nullptr };
}
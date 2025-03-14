#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Lib/ComponentStructures.h>
#include <Engine/Component/Manager/EntityManager.h>
#include <Engine/Component/Manager/Transform3DManager.h>
#include <Engine/Component/Manager/MaterialManager.h>
#include <Engine/Component/Manager/AnimationComponentManager.h>
#include <Engine/Component/Manager/ModelComponentManager.h>
#include <Engine/Component/Manager/ImGuiComponentManager.h>
#include <Lib/MathUtils/Algorithm.h>

// directX
#include <d3d12.h>
// c++
#include <cstdint>
#include <variant>
#include <typeindex>
#include <unordered_map>
// front
class Asset;
class SRVManager;
class RenderObjectManager;

// 03/08
// 削除したObjectにアクセスしてしまう可能性大
// ユーザーにはIDのみ渡して毎回Get()してObjectを使用する方面に移行 ✓
// imguiの操作について
// camera、lightはsceneで追加されたやつを操作できるようにする 後回し
// id:nameをhierarchyで表示し、inspectorで操作できるようにする ✓
// objectのinstancing描画、CreateInstancing("instancingする用の名前",instancingCount);
// CreateObject3D()にinstancingで描画するかstd::optional<std::string>で、"instancingする用の名前"を
// 受け取って、has_value()ならbufferは作成せず、instancingBufferに渡すだけにする

//============================================================================
//	ComponentManager class
//============================================================================
class ComponentManager {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	ComponentManager() = default;
	~ComponentManager() = default;

	void Init(ID3D12Device* device, ID3D12GraphicsCommandList* commandList,
		Asset* asset, SRVManager* srvManager, RenderObjectManager* renderObjectManager);

	void InitImGui(Transform3DManager* transform3DManager,
		MaterialManager* materialManager, ModelComponentManager* modelComponentManager);

	void Update();

	// componentManagerの追加
	template <typename T>
	void RegisterComponentManager(IComponent<T>* manager);

	// instancingの作成
	void CreateInstancing(const std::string& modelName, const std::string& name, uint32_t instanceCount);

	// object3Dの追加、削除
	EntityID CreateObject3D(const std::string& modelName,
		const std::optional<std::string>& animationName, const std::string& objectName);
	void RemoveObject3D(EntityID id);

	//--------- accessor -----------------------------------------------------

	// singleton
	static ComponentManager* GetInstance();
	static void Finalize();

	// componentの取得
	template <typename T>
	T* GetComponent(EntityID id);
	template <typename T>
	std::vector<T*> GetComponentList(EntityID entity);

	// entityの数取得
	EntityID GetEntityCount() const { return entityManager_->GetEntityCount(); }

	// imguiManagerの取得
	ImGuiComponentManager* GetImGuiComponentManager() const { return imguiComponentManager_.get(); }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	static ComponentManager* instance_;

	ID3D12Device* device_;
	ID3D12GraphicsCommandList* commandList_;
	Asset* asset_;
	SRVManager* srvManager_;
	RenderObjectManager* renderObjectManager_;

	// entity番号の管理
	std::unique_ptr<EntityManager> entityManager_;
	// 登録済みのcomponentManager
	std::unordered_map<std::type_index, void*> componentManagers_;

	// imgui
	std::unique_ptr<ImGuiComponentManager> imguiComponentManager_;

	//--------- functions ----------------------------------------------------

	template <typename T, typename... Args>
	void AddComponent(EntityID entity, Args&&... args);
	template <typename T>
	void RemoveComponent(EntityID entity);
};

//============================================================================
//	ComponentManager templateMethods
//============================================================================

template<typename T>
inline void ComponentManager::RegisterComponentManager(IComponent<T>* manager) {

	ASSERT(manager, "not create this ComponentClass");

	componentManagers_[std::type_index(typeid(T))] = manager;
}

template<typename T, typename ...Args>
inline void ComponentManager::AddComponent(EntityID entity, Args && ...args) {

	if ((Algorithm::Find(componentManagers_, std::type_index(typeid(T)), true))) {

		static_cast<IComponent<T>*>(componentManagers_.at(std::type_index(typeid(T))))->AddComponent(entity, std::make_tuple(std::forward<Args>(args)...));
	}
}

template<typename T>
inline void ComponentManager::RemoveComponent(EntityID entity) {

	if (Algorithm::Find(componentManagers_, std::type_index(typeid(T)), true)) {

		static_cast<IComponent<T>*>(componentManagers_.at(std::type_index(typeid(T))))->RemoveComponent(entity);
	}
}

template<typename T>
inline T* ComponentManager::GetComponent(EntityID id) {

	if (Algorithm::Find(componentManagers_, std::type_index(typeid(T)), true)) {

		return static_cast<IComponent<T>*>(componentManagers_.at(std::type_index(typeid(T))))->GetComponent(id);
	}
	return nullptr;
}

template<typename T>
inline std::vector<T*> ComponentManager::GetComponentList(EntityID entity) {

	if (Algorithm::Find(componentManagers_, std::type_index(typeid(T)), true)) {

		return dynamic_cast<IComponent<std::vector<T>>*>(componentManagers_.at(std::type_index(typeid(T))))->GetComponentList(entity);
	}
	return {};
}
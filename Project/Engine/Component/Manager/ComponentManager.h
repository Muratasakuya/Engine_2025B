#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Lib/ComponentStructures.h>
#include <Engine/Component/Manager/EntityManager.h>
#include <Engine/Component/Manager/Transform3DManager.h>
#include <Engine/Component/Manager/MaterialManager.h>
#include <Engine/Component/Manager/ModelComponentManager.h>
#include <Engine/Editor/ComponentImGui.h>
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

// 今現段階で必要と思った種類
// 3DObjectを扱いたいとき
// Transform3D、Materials、Model(Animationするかで分岐)、Rendering情報(BlendModeなど)、Collisionは複数設定したい、Physics
// これらはすべて同じIDで作成したい
// AddComponnentしたら引き数でModelName、Animationするかしないか、ObjectNameを受け取り作成し、ユーザーに操作できるデータをポインタで渡す
// Spriteを扱いたいとき
// Transform2D、Color、Rendering情報(BlendModeなど)、Collision
// これらはすべて同じIDで作成したい
// AddComponnentしたら引き数でTextureName、ObjectNameを受け取り作成し、ユーザーに操作できるデータをポインタで渡す

// 03/08
// 削除したObjectにアクセスしてしまう可能性大
// ユーザーにはIDのみ渡して毎回Get()してObjectを使用する方面に移行

//============================================================================
//	ComponentManager class
//============================================================================
class ComponentManager {
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- structures ---------------------------------------------------

	struct Object3DForGPU {

		DxConstBuffer<TransformationMatrix> matrix;
		std::vector<DxConstBuffer<Material>> materials;
		ModelComponent* model;
	};
public:
	//========================================================================
	//	public Methods
	//========================================================================

	ComponentManager() = default;
	~ComponentManager() = default;

	void Init(ID3D12Device* device, Asset* asset, SRVManager* srvManager,
		RenderObjectManager* renderObjectManager);
	void Update();

	// componentManagerの追加
	template <typename T>
	void RegisterComponentManager(IComponent<T>* manager);

	// object3Dの追加、削除
	EntityID CreateObject3D(const std::string& modelName,
		const std::optional<std::string>& animationName, const std::string& objectName);
	void RemoveObject3D(EntityID id);

	//--------- accessor -----------------------------------------------------

	// singleton
	static ComponentManager* GetInstance();
	static void Finalize();

	// componentの取得、基本的にinitで取得する
	template <typename T>
	T* GetComponent(EntityID id);
	template <typename T>
	std::vector<T*> GetComponentList(EntityID entity);

	// 全てのentityのIDを取得
	const std::vector<EntityID>& GetEntityIDs() const { return entityManager_->GetIDs(); };
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	static ComponentManager* instance_;

	ID3D12Device* device_;
	Asset* asset_;
	SRVManager* srvManager_;
	RenderObjectManager* renderObjectManager_;

	// entity番号の管理
	std::unique_ptr<EntityManager> entityManager_;
	// 登録済みのcomponentManager
	std::unordered_map<std::type_index, void*> componentManagers_;

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
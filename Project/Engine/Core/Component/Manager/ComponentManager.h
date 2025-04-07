#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Component/Manager/EntityManager.h>
#include <Engine/Core/Component/Manager/TransformManager.h>
#include <Engine/Core/Component/Manager/MaterialManager.h>
#include <Engine/Core/Component/Manager/AnimationComponentManager.h>
#include <Engine/Core/Component/Manager/SpriteComponentManager.h>
#include <Engine/Core/Component/Manager/ImGuiComponentManager.h>
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
class SRVManager;
class GPUObjectSystem;

//============================================================================
//	enum class
//============================================================================

// componentの種類
enum class ComponentType : uint32_t {
	Object3D,
	Object2D,
	Trail,

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

	ComponentManager() = default;
	~ComponentManager() = default;

	void Init(ID3D12Device* device, Asset* asset, GPUObjectSystem* gpuObjectSystem);

	void InitImGui(
		// 3D
		Transform3DManager* transform3DManager, MaterialManager* materialManager,
		// 2D
		Transform2DManager* transform2DManager, SpriteMaterialManager* SpriteMaterialManager,
		SpriteComponentManager* spriteComponentManager);

	void Update();

	// componentManagerの追加
	template <typename T>
	void RegisterComponentManager(IComponent<T>* manager);

	//--------- object3D -----------------------------------------------------

	// 追加、作成処理
	uint32_t CreateObject3D(const std::string& modelName, const std::string& objectName,
		const std::optional<std::string>& groupName,
		const std::optional<std::string>& animationName);
	// 指定されたidのentity削除
	void RemoveObject3D(uint32_t id);

	//--------- object2D -----------------------------------------------------

	// 追加、作成処理
	uint32_t CreateObject2D(const std::string& textureName, const std::string& objectName,
		const std::optional<std::string>& groupName = std::nullopt);
	// 指定されたidのentity削除
	void RemoveObject2D(uint32_t id);

	//--------- accessor -----------------------------------------------------

	// singleton
	static ComponentManager* GetInstance();
	static void Finalize();

	// imgui
	void SelectObject();
	void EditObject();
	void SetImGuiFunc(uint32_t entityId, std::function<void()> func);
	void ResetImGui();

	// componentの取得
	template <typename T>
	T* GetComponent(uint32_t id);
	template <typename T>
	std::vector<T*> GetComponentList(uint32_t entity);

	// entityの数取得
	const std::vector<uint32_t>& GetEntityList(ComponentType type) const;

	uint32_t GetEntityIndex(ComponentType type, uint32_t id) const { return entityManagers_[static_cast<uint32_t>(type)]->GetIndex(id); }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//- dependencies variables -----------------------------------------------

	ID3D12Device* device_;
	ID3D12GraphicsCommandList* commandList_;
	Asset* asset_;
	SRVManager* srvManager_;
	GPUObjectSystem* gpuObjectSystem_;

	//--------- variables ----------------------------------------------------

	static ComponentManager* instance_;

	// entity番号の管理
	std::array<std::unique_ptr<EntityManager>, kComponentCount> entityManagers_;
	// 登録済みのcomponentManager
	std::unordered_map<std::type_index, void*> componentManagers_;

	// imgui
	std::unique_ptr<ImGuiComponentManager> imguiComponentManager_;

	//--------- functions ----------------------------------------------------

	template <typename T, typename... Args>
	void AddComponent(uint32_t entity, Args&&... args);
	template <typename T>
	void RemoveComponent(uint32_t entity);
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
inline void ComponentManager::AddComponent(uint32_t entity, Args && ...args) {

	if ((Algorithm::Find(componentManagers_, std::type_index(typeid(T)), true))) {

		static_cast<IComponent<T>*>(componentManagers_.at(std::type_index(typeid(T))))->AddComponent(entity, std::make_tuple(std::forward<Args>(args)...));
	}
}

template<typename T>
inline void ComponentManager::RemoveComponent(uint32_t entity) {

	if (Algorithm::Find(componentManagers_, std::type_index(typeid(T)), true)) {

		static_cast<IComponent<T>*>(componentManagers_.at(std::type_index(typeid(T))))->RemoveComponent(entity);
	}
}

template<typename T>
inline T* ComponentManager::GetComponent(uint32_t id) {

	if (Algorithm::Find(componentManagers_, std::type_index(typeid(T)), true)) {

		return static_cast<IComponent<T>*>(componentManagers_.at(std::type_index(typeid(T))))->GetComponent(id);
	}
	return nullptr;
}

template<typename T>
inline std::vector<T*> ComponentManager::GetComponentList(uint32_t id) {

	if (Algorithm::Find(componentManagers_, std::type_index(typeid(T)), true)) {

		return static_cast<IComponent<T>*>(componentManagers_.at(std::type_index(typeid(T))))->GetComponentList(id);
	}
	return { nullptr };
}
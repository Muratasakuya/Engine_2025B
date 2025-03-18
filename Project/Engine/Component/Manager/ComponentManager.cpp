#include "ComponentManager.h"

//============================================================================
//	EntityComponent classMethods
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Engine/Core/Debug/Assert.h>
#include <Engine/Renderer/Managers/RenderObjectManager.h>
#include <Lib/MathUtils/Algorithm.h>

//============================================================================
//	ComponentManager classMethods
//============================================================================

ComponentManager* ComponentManager::instance_ = nullptr;

ComponentManager* ComponentManager::GetInstance() {

	if (instance_ == nullptr) {
		instance_ = new ComponentManager();
	}
	return instance_;
}

void ComponentManager::Finalize() {

	if (instance_ != nullptr) {

		delete instance_;
		instance_ = nullptr;
	}
}

void ComponentManager::Init(ID3D12Device* device, ID3D12GraphicsCommandList* commandList,
	Asset* asset, SRVManager* srvManager, RenderObjectManager* renderObjectManager) {

	device_ = nullptr;
	device_ = device;

	commandList_ = nullptr;
	commandList_ = commandList;

	asset_ = nullptr;
	asset_ = asset;

	srvManager_ = nullptr;
	srvManager_ = srvManager;

	renderObjectManager_ = nullptr;
	renderObjectManager_ = renderObjectManager;

	for (uint32_t type : Algorithm::GetEnumArray(ComponentType::Count)) {

		entityManagers_[type] = std::make_unique<EntityManager>();
	}
}

void ComponentManager::InitImGui(Transform3DManager* transform3DManager,
	MaterialManager* materialManager, ModelComponentManager* modelComponentManager) {

#ifdef _DEBUG
	imguiComponentManager_ = std::make_unique<ImGuiComponentManager>();
	imguiComponentManager_->Init(entityManagers_[static_cast<uint32_t>(ComponentType::Object3D)].get(), transform3DManager,
		materialManager, modelComponentManager);
#endif // _DEBUG
}

void ComponentManager::Update() {

	// 全Componentの更新処理
	for (const auto& [type, manager] : componentManagers_) {

		static_cast<IComponentManager*>(manager)->Update();
	}
}

EntityID ComponentManager::CreateObject3D(const std::string& modelName,
	const std::optional<std::string>& animationName, const std::string& objectName,
	const std::optional<std::string>& instancingName) {

	// entityID発行
	EntityID id = entityManagers_[static_cast<uint32_t>(ComponentType::Object3D)]->CreateEntity(objectName);

	// object3Dに必要なcomponentを追加
	AddComponent<Transform3DComponent>(id);
	AddComponent<Material>(id, asset_->GetModelData(modelName).meshes.size());
	AddComponent<AnimationComponent>(id, animationName, asset_);
	AddComponent<ModelComponent>(id, modelName, animationName, device_, commandList_, asset_, srvManager_);
	// buffer作成
	renderObjectManager_->CreateObject3D(id, instancingName, GetComponent<ModelComponent>(id), device_);

	return id;
}

void ComponentManager::RemoveObject3D(EntityID id) {

	// idの削除
	entityManagers_[static_cast<uint32_t>(ComponentType::Object3D)]->DestroyEntity(id);

	// object3Dで使用していたcomponentの削除
	RemoveComponent<Transform3DComponent>(id);
	RemoveComponent<Material>(id);
	RemoveComponent<AnimationComponent>(id);
	RemoveComponent<ModelComponent>(id);
	// buffer削除
	renderObjectManager_->RemoveObject3D(id);
}
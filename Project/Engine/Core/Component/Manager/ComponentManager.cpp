#include "ComponentManager.h"

//============================================================================
//	EntityComponent classMethods
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Engine/Core/Debug/Assert.h>
#include <Engine/Core/Graphics/GPUObject/GPUObjectSystem.h>
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

void ComponentManager::SelectObject() {

	imguiComponentManager_->SelectObject();
}

void ComponentManager::EditObject() {

	imguiComponentManager_->EditObject();
}

void ComponentManager::SetImGuiFunc(uint32_t entityId, std::function<void()> func) {

	imguiComponentManager_->SetImGuiFunc(entityId, func);
}

void ComponentManager::ResetImGui() {

	imguiComponentManager_->Reset();
}

const std::vector<uint32_t>& ComponentManager::GetEntityList(ComponentType type) const {

	return entityManagers_[static_cast<uint32_t>(type)]->GetIndexToEntity();
}

void ComponentManager::Init(ID3D12Device* device, Asset* asset, GPUObjectSystem* gpuObjectSystem) {

	device_ = nullptr;
	device_ = device;

	asset_ = nullptr;
	asset_ = asset;

	gpuObjectSystem_ = nullptr;
	gpuObjectSystem_ = gpuObjectSystem;

	for (uint32_t type : Algorithm::GetEnumArray(ComponentType::Count)) {

		entityManagers_[type] = std::make_unique<EntityManager>();
	}
}

void ComponentManager::InitImGui(

	// 3D
	Transform3DManager* transform3DManager, MaterialManager* materialManager,
	// 2D
	Transform2DManager* transform2DManager, SpriteMaterialManager* spriteMaterialManager,
	SpriteComponentManager* spriteComponentManager) {

#ifdef _DEBUG
	imguiComponentManager_ = std::make_unique<ImGuiComponentManager>();
	imguiComponentManager_->Init(
		// 3D
		entityManagers_[static_cast<uint32_t>(ComponentType::Object3D)].get(), transform3DManager,
		materialManager,
		// 2D
		entityManagers_[static_cast<uint32_t>(ComponentType::Object2D)].get(), transform2DManager,
		spriteMaterialManager, spriteComponentManager);
#endif // _DEBUG
}

void ComponentManager::Update() {

	// 全Componentの更新処理
	for (const auto& [type, manager] : componentManagers_) {

		static_cast<IComponentManager*>(manager)->Update();
	}
}

uint32_t ComponentManager::CreateObject3D(
	const std::string& modelName, const std::string& objectName,
	const std::optional<std::string>& groupName, const std::optional<std::string>& animationName) {

	animationName;

	// entityID発行
	uint32_t id = entityManagers_[static_cast<uint32_t>(ComponentType::Object3D)]->CreateEntity(objectName, groupName);

	// object3Dに必要なcomponentを追加
	AddComponent<Transform3DComponent>(id);

	auto transform = GetComponent<Transform3DComponent>(id);
	transform->SetInstancingName(modelName);
	AddComponent<MaterialComponent>(id, asset_->GetModelData(modelName), asset_);

	// buffer作成
	gpuObjectSystem_->CreateMesh(modelName);

	return id;
}

uint32_t ComponentManager::CreateObject2D(const std::string& textureName, const std::string& objectName,
	const std::optional<std::string>& groupName) {

	// entityID発行
	uint32_t id = entityManagers_[static_cast<uint32_t>(ComponentType::Object2D)]->CreateEntity(objectName, groupName);

	// object2Dに必要なcomponentを追加
	AddComponent<Transform2DComponent>(id);
	AddComponent<SpriteMaterial>(id);
	AddComponent<SpriteComponent>(id, textureName, GetComponent<Transform2DComponent>(id), device_, asset_);
	// buffer作成
	gpuObjectSystem_->CreateObject2D(id, GetComponent<SpriteComponent>(id), device_);

	return id;
}

void ComponentManager::RemoveObject3D(uint32_t entityId) {

	// idの削除
	entityManagers_[static_cast<uint32_t>(ComponentType::Object3D)]->RemoveEntity(entityId);

	// object3Dで使用していたcomponentの削除
	RemoveComponent<Transform3DComponent>(entityId);
	RemoveComponent<MaterialComponent>(entityId);
}

void ComponentManager::RemoveObject2D(uint32_t entityId) {

	// idの削除
	entityManagers_[static_cast<uint32_t>(ComponentType::Object2D)]->RemoveEntity(entityId);

	// buffer削除
	gpuObjectSystem_->RemoveObject2D(entityId);
	// object2Dで使用していたcomponentの削除
	RemoveComponent<Transform2DComponent>(entityId);
	RemoveComponent<SpriteMaterial>(entityId);
	RemoveComponent<SpriteComponent>(entityId);
}
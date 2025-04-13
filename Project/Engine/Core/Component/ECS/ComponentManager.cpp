#include "ComponentManager.h"

//============================================================================
//	EntityComponent classMethods
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Engine/Core/Debug/Assert.h>
#include <Engine/Core/Graphics/GPUObject/GPUObjectSystem.h>
#include <Engine/Editor/ImGuiInspector.h>
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

const std::vector<uint32_t>& ComponentManager::GetEntityList(ComponentType type) const {

	return entityRegistries_[static_cast<uint32_t>(type)]->GetIndexToEntity();
}

uint32_t ComponentManager::GetEntityIndex(ComponentType type, uint32_t entityId) const {

	return entityRegistries_[static_cast<uint32_t>(type)]->GetIndex(entityId);
}

void ComponentManager::Init(ID3D12Device* device, Asset* asset, GPUObjectSystem* gpuObjectSystem) {

	device_ = nullptr;
	device_ = device;

	asset_ = nullptr;
	asset_ = asset;

	gpuObjectSystem_ = nullptr;
	gpuObjectSystem_ = gpuObjectSystem;

	for (uint32_t type : Algorithm::GetEnumArray(ComponentType::Count)) {

		entityRegistries_[type] = std::make_unique<EntityRegistry>();
	}

	// inspectorへentityManagerをセット
	ImGuiInspector::GetInstance()->SetEntityManager(
		entityRegistries_[static_cast<uint32_t>(ComponentType::Object3D)].get(),
		entityRegistries_[static_cast<uint32_t>(ComponentType::Object2D)].get());
}

void ComponentManager::Update() {

	// 全Componentの更新処理
	for (const auto& [type, manager] : componentStores_) {

		static_cast<IComponentBase*>(manager)->Update();
	}
}

uint32_t ComponentManager::CreateObject3D(
	const std::string& modelName, const std::string& objectName,
	const std::optional<std::string>& groupName, const std::optional<std::string>& animationName) {

	animationName;

	// entityID発行
	uint32_t id = entityRegistries_[static_cast<uint32_t>(ComponentType::Object3D)]->CreateEntity(objectName, groupName);

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
	uint32_t id = entityRegistries_[static_cast<uint32_t>(ComponentType::Object2D)]->CreateEntity(objectName, groupName);

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
	entityRegistries_[static_cast<uint32_t>(ComponentType::Object3D)]->RemoveEntity(entityId);

	// object3Dで使用していたcomponentの削除
	RemoveComponent<Transform3DComponent>(entityId);
	RemoveComponent<MaterialComponent>(entityId);
}

void ComponentManager::RemoveObject2D(uint32_t entityId) {

	// idの削除
	entityRegistries_[static_cast<uint32_t>(ComponentType::Object2D)]->RemoveEntity(entityId);

	// buffer削除
	gpuObjectSystem_->RemoveObject2D(entityId);
	// object2Dで使用していたcomponentの削除
	RemoveComponent<Transform2DComponent>(entityId);
	RemoveComponent<SpriteMaterial>(entityId);
	RemoveComponent<SpriteComponent>(entityId);
}

uint32_t ComponentManager::CreateEffect(
	const std::string& modelName, const std::string& objectName,
	const std::optional<std::string>& groupName) {

	modelName;
	objectName;
	groupName;

	// entityID発行
	//uint32_t id = entityRegistries_[static_cast<uint32_t>(ComponentType::Effect)]->CreateEntity(objectName, groupName);

	// effectに必要なcomponentを追加

	return 0;
}

void ComponentManager::RemoveEffect(uint32_t entityId) {

	entityId;
}
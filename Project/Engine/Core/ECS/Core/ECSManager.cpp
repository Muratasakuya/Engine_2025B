#include "ECSManager.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Engine/Editor/ImGuiEntityEditor.h>

// components
#include <Engine/Core/ECS/Components/TransformComponent.h>
#include <Engine/Core/ECS/Components/MaterialComponent.h>
#include <Engine/Core/ECS/Components/AnimationComponent.h>
#include <Engine/Core/ECS/Components/TagComponent.h>
// systems
#include <Engine/Core/ECS/System/Systems/TransformSystem.h>
#include <Engine/Core/ECS/System/Systems/MaterialSystem.h>
#include <Engine/Core/ECS/System/Systems/AnimationSystem.h>
#include <Engine/Core/ECS/System/Systems/InstancedMeshSystem.h>
#include <Engine/Core/ECS/System/Systems/SpriteBufferSystem.h>
#include <Engine/Core/ECS/System/Systems/SkyboxRenderSystem.h>
#include <Engine/Core/ECS/System/Systems/TagSystem.h>

//============================================================================
//	ECSManager classMethods
//============================================================================

ECSManager* ECSManager::instance_ = nullptr;

ECSManager* ECSManager::GetInstance() {

	if (instance_ == nullptr) {
		instance_ = new ECSManager();
	}
	return instance_;
}

void ECSManager::Finalize() {

	if (instance_ != nullptr) {

		delete instance_;
		instance_ = nullptr;
	}
}

void ECSManager::Init(ID3D12Device* device, Asset* asset, DxCommand* dxCommand) {

	asset_ = nullptr;
	asset_ = asset;

	device_ = nullptr;
	device_ = device;

	entityManager_ = std::make_unique<EntityManager>();
	systemManager_ = std::make_unique<SystemManager>();

	// system登録
	systemManager_->AddSystem<Transform3DSystem>();
	systemManager_->AddSystem<Transform2DSystem>();
	systemManager_->AddSystem<AnimationSystem>();
	systemManager_->AddSystem<MaterialSystem>();
	systemManager_->AddSystem<SpriteMaterialSystem>();
	systemManager_->AddSystem<TagSystem>();
	systemManager_->AddSystem<InstancedMeshSystem>(device, asset, dxCommand);
	systemManager_->AddSystem<SpriteBufferSystem>();
	systemManager_->AddSystem<SkyboxRenderSystem>();

	ImGuiEntityEditor::GetInstance()->Init();
}

void ECSManager::UpdateComponent() {

	systemManager_->UpdateComponent(*entityManager_.get());
}

void ECSManager::UpdateBuffer() {

	systemManager_->UpdateBuffer(*entityManager_.get());
}

uint32_t ECSManager::CreateObject3D(const std::string& modelName,
	const std::string& name, const std::string& groupName,
	const std::optional<std::string>& animationName) {

	// entity作成
	uint32_t entity = BuildEmptyEntity(name, groupName);
	// 必要なcomponentを作成
	auto* transform = entityManager_->AddComponent<Transform3DComponent>(entity);
	auto* materialsPtr = entityManager_->AddComponent<MaterialComponent, true>(entity);

	// 各componentを初期化
	// transform
	transform->Init();
	// instancingのデータ名を設定
	transform->SetInstancingName(modelName);

	// material
	const ModelData& modelData = asset_->GetModelData(modelName);
	auto& materials = *materialsPtr;
	systemManager_->GetSystem<MaterialSystem>()->Init(
		materials, modelData, asset_);

	if (animationName.has_value()) {

		// animation処理がある場合はcomponentを追加
		auto* animation = entityManager_->AddComponent<AnimationComponent>(entity);
		// 初期化
		animation->Init(*animationName, asset_);

		// bufferを作成
		systemManager_->GetSystem<InstancedMeshSystem>()->CreateSkinnedMesh(modelName);
	} else {

		// bufferを作成
		systemManager_->GetSystem<InstancedMeshSystem>()->CreateStaticMesh(modelName);
	}

	return entity;
}

uint32_t ECSManager::CreateSkybox(const std::string& textureName) {
	
	// entity作成
	uint32_t entity = BuildEmptyEntity("skybox", "Environment");
	// 必要なcomponentを作成
	auto* skybox = entityManager_->AddComponent<SkyboxComponent>(entity);

	// componentを初期化
	skybox->Create(device_, asset_->GetTextureGPUIndex(textureName));

	return entity;
}

uint32_t ECSManager::CreateObject2D(const std::string& textureName,
	const std::string& name, const std::string& groupName) {

	// entity作成
	uint32_t entity = BuildEmptyEntity(name, groupName);
	// 必要なcomponentを作成
	auto* transform = entityManager_->AddComponent<Transform2DComponent>(entity);
	auto* material = entityManager_->AddComponent<SpriteMaterialComponent>(entity);

	// 各componentを初期化
	// transform
	transform->Init(device_);
	// material
	material->Init(device_);
	// sprite
	entityManager_->AddComponent<SpriteComponent>(entity,
		device_, asset_, textureName, *transform);

	return entity;
}

void ECSManager::Destroy(uint32_t entity) {

	entityManager_->Destroy(entity);
}

uint32_t ECSManager::BuildEmptyEntity(
	[[maybe_unused]] const std::string& name,
	[[maybe_unused]] const std::string& groupName) {

	// entity作成
	uint32_t entity = entityManager_->Create();
	// tag設定
	auto* tag = entityManager_->AddComponent<TagComponent>(entity);
	tag->name = systemManager_->GetSystem<TagSystem>()->CheckName(name);
	tag->groupName = groupName;

	return entity;
}
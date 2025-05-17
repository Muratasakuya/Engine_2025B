#include "ECSManager.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>

// components
#include <Engine/Core/Component/TransformComponent.h>
#include <Engine/Core/Component/MaterialComponent.h>
#include <Engine/Core/Component/AnimationComponent.h>
// systems
#include <Engine/Core/ECS/System/Systems/TransformSystem.h>
#include <Engine/Core/ECS/System/Systems/MaterialSystem.h>
#include <Engine/Core/ECS/System/Systems/AnimationSystem.h>
#include <Engine/Core/ECS/System/Systems/InstancedMeshSystem.h>

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

	entityManager_ = std::make_unique<EntityManager>();
	systemManager_ = std::make_unique<SystemManager>();

	// system登録
	systemManager_->AddSystem<Transform3DSystem>();
	systemManager_->AddSystem<MaterialSystem>();
	systemManager_->AddSystem<AnimationSystem>();

	// 必ず最後におこなう
	systemManager_->AddSystem<InstancedMeshSystem>(device, asset, dxCommand);
}

void ECSManager::Update() {

	systemManager_->Update(*entityManager_.get());
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
	transform->Init();
	transform->SetInstancingName(modelName);

	const ModelData& modelData = asset_->GetModelData(modelName);
	auto& materials = *materialsPtr;
	materials.resize(modelData.meshes.size());
	for (uint32_t meshIndex = 0; meshIndex < modelData.meshes.size(); ++meshIndex) {

		materials[meshIndex].Init();
		materials[meshIndex].material.textureIndex =
			asset_->GetTextureGPUIndex(modelData.meshes[meshIndex].textureName.value_or("white"));

		// normalMap用のTextureがあれば設定する
		if (modelData.meshes[meshIndex].normalMapTexture.has_value()) {

			materials[meshIndex].material.normalMapTextureIndex =
				asset_->GetTextureGPUIndex(modelData.meshes[meshIndex].normalMapTexture.value());
			materials[meshIndex].material.enableNormalMap = true;
		}

		// baseColorがあれば色を設定する
		if (modelData.meshes[meshIndex].baseColor.has_value()) {

			materials[meshIndex].material.color = modelData.meshes[meshIndex].baseColor.value();
			materials[meshIndex].material.emissionColor = Vector3(
				modelData.meshes[meshIndex].baseColor.value().r,
				modelData.meshes[meshIndex].baseColor.value().g,
				modelData.meshes[meshIndex].baseColor.value().b);
		}
	}

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

void ECSManager::Destroy(uint32_t entity) {

	entityManager_->Destroy(entity);
}

uint32_t ECSManager::BuildEmptyEntity(
	[[maybe_unused]] const std::string& name,
	[[maybe_unused]] const std::string& groupName) {

	uint32_t entity = entityManager_->Create();
	return entity;
}
#include "InstancedMeshSystem.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/ECS/Core/EntityManager.h>

//============================================================================
//	InstancedMeshSystem classMethods
//============================================================================

InstancedMeshSystem::InstancedMeshSystem(ID3D12Device* device, Asset* asset,
	DxCommand* dxCommand) : device_{ device }, asset_{ asset }, dxCommand_{ dxCommand } {

	meshRegistry_ = std::make_unique<MeshRegistry>();
	meshRegistry_->Init(device_, asset_);

	instancedBuffer_ = std::make_unique<InstancedMeshBuffer>();
	instancedBuffer_->Init(device_, asset_);
}

void InstancedMeshSystem::CreateStaticMesh(const std::string& modelName) {

	// 最大instance数
	const uint32_t kMaxInstanceNum = 0xffff;

	// meshの作成、登録
	meshRegistry_->RegisterMesh(modelName, false, 0);
	// instancingデータ作成
	instancedBuffer_->Create(meshRegistry_->GetMeshes().at(modelName).get(),
		modelName, kMaxInstanceNum);
}

void InstancedMeshSystem::CreateSkinnedMesh(const std::string& modelName) {

	// 最大instance数
	const uint32_t kMaxInstanceNum = 64;

	// meshの作成、登録
	meshRegistry_->RegisterMesh(modelName, true, kMaxInstanceNum);
	// instancingデータ作成
	instancedBuffer_->Create(meshRegistry_->GetMeshes().at(modelName).get(),
		modelName, kMaxInstanceNum);
}

Archetype InstancedMeshSystem::Signature() const {

	Archetype arch{};
	arch.set(EntityManager::GetTypeID<Transform3DComponent>());
	arch.set(EntityManager::GetTypeID<MaterialComponent>());
	return arch;
}

void InstancedMeshSystem::Update(EntityManager& entityManager) {

	// bufferクリア
	instancedBuffer_->Reset();

	const auto& view = entityManager.View(Signature());

	for (const auto& entity : view) {

		auto* transform = entityManager.GetComponent<Transform3DComponent>(entity);
		auto* materials = entityManager.GetComponent<MaterialComponent, true>(entity);
		auto* animation = entityManager.GetComponent<AnimationComponent>(entity);

		const std::string& instancingName = transform->GetInstancingName();
		instancedBuffer_->SetUploadData(
			instancingName, transform->matrix, *materials, *animation);
	}

	// buffer転送
	instancedBuffer_->Update(dxCommand_);
}
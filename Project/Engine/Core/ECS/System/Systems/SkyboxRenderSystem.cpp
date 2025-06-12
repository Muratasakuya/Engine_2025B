#include "SkyboxRenderSystem.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/ECS/Core/EntityManager.h>

//============================================================================
//	SkyboxRenderSystem classMethods
//============================================================================

Archetype SkyboxRenderSystem::Signature() const {

	Archetype arch{};
	arch.set(EntityManager::GetTypeID<SkyboxComponent>());
	return arch;
}

void SkyboxRenderSystem::Update(EntityManager& entityManager) {

	const auto& view = entityManager.View(Signature());

	for (const auto& entity : view) {

		auto* skybox = entityManager.GetComponent<SkyboxComponent>(entity);

		// skyboxの更新
		skybox->Update();
		component_ = skybox;
		isCreated_ = true;
	}
}

void SkyboxRenderSystem::Render(ID3D12GraphicsCommandList* commandList) {

	// 作成されていない場合は処理しない
	if (!isCreated_) {
		return;
	}

	// buffer設定
	// vertex
	commandList->IASetVertexBuffers(0, 1, &component_->GetVertexBuffer().GetVertexBuffer());
	commandList->IASetIndexBuffer(&component_->GetIndexBuffer().GetIndexBuffer());
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// matrix
	commandList->SetGraphicsRootConstantBufferView(0, component_->GetMatrixBuffer().GetResource()->GetGPUVirtualAddress());
	// material
	commandList->SetGraphicsRootConstantBufferView(3, component_->GetMaterialBuffer().GetResource()->GetGPUVirtualAddress());

	// 描画
	commandList->DrawIndexedInstanced(component_->GetIndexCount(), 1, 0, 0, 0);
}

uint32_t SkyboxRenderSystem::GetTextureIndex() const {

	if (!isCreated_) {
		assert(false);
		return 0;
	}

	return component_->GetTextureIndex();
}
#include "InstancedMesh.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Managers/SRVManager.h>
#include <Lib/MathUtils/Algorithm.h>

//============================================================================
//	InstancedMesh classMethods
//============================================================================

void InstancedMesh::Init(ID3D12Device* device, SRVManager* srvManager) {

	device_ = nullptr;
	device_ = device;

	srvManager_ = nullptr;
	srvManager_ = srvManager;

	// srvDescの共通設定
	srvBufferDesc_ = {};
	srvBufferDesc_.Format = DXGI_FORMAT_UNKNOWN;
	srvBufferDesc_.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvBufferDesc_.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvBufferDesc_.Buffer.FirstElement = 0;
	srvBufferDesc_.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
}

void InstancedMesh::CreateBuffers(const std::string& name) {

	auto& meshGroup = meshGroups_[name];

	// transform
	{
		// buffer作成
		meshGroup.matrix.CreateStructuredBuffer(device_, meshGroup.maxInstance);

		// desc設定
		srvBufferDesc_.Buffer.NumElements = meshGroup.maxInstance;
		srvBufferDesc_.Buffer.StructureByteStride = sizeof(TransformationMatrix);
		// srv作成
		srvManager_->CreateSRV(meshGroup.transformSrvIndex,
			meshGroup.matrix.GetResource(), srvBufferDesc_);
	}

	// material
	{
		// meshの数だけ作成する
		const size_t meshNum = meshGroup.model.model->GetMeshNum();
		meshGroup.materialSrvIndices.resize(meshGroup.model.model->GetMeshNum());
		meshGroup.materials.resize(meshGroup.model.model->GetMeshNum());
		// componentの数もここで決める
		meshGroup.materialComponents.resize(meshGroup.model.model->GetMeshNum());

		// desc設定
		srvBufferDesc_.Buffer.NumElements = meshGroup.maxInstance;
		srvBufferDesc_.Buffer.StructureByteStride = sizeof(InstancingMaterial);

		for (uint32_t meshIndex = 0; meshIndex < meshNum; ++meshIndex) {

			// buffer作成
			meshGroup.materials[meshIndex].CreateStructuredBuffer(device_, meshGroup.maxInstance);

			// srv作成
			srvManager_->CreateSRV(meshGroup.materialSrvIndices[meshIndex],
				meshGroup.materials[meshIndex].GetResource(), srvBufferDesc_);
		}
	}
}

void InstancedMesh::Create(const ModelComponent& modelComponent,
	const std::string& name, uint32_t numInstance) {

	// すでにある場合は作成しない
	if (Algorithm::Find(meshGroups_, name)) {
		return;
	}

	// 最大のinstance数設定
	meshGroups_[name].maxInstance = numInstance;
	// 描画を行うmodelの設定
	meshGroups_[name].model.model = modelComponent.model.get();
	if (modelComponent.isAnimation) {
		// animationは対応無し
		ASSERT(FALSE, "unsupported skinnedAnimationMesh");
	}

	// bufferの作成
	CreateBuffers(name);
}


void InstancedMesh::SetComponent(const std::string& name,
	const TransformationMatrix& matrix, const std::vector<Material>& materials) {

	meshGroups_[name].matrixComponents.emplace_back(matrix);

	for (uint32_t meshIndex = 0; meshIndex < meshGroups_[name].materialComponents.size(); ++meshIndex) {

		InstancingMaterial material{};
		material.SetMaterial(materials[meshIndex]);
		meshGroups_[name].materialComponents[meshIndex].emplace_back(material);
	}

	// instance数インクリメント
	++meshGroups_[name].numInstance;
}

void InstancedMesh::Update() {

	// 何もなければ処理をしない
	if (meshGroups_.empty()) {
		return;
	}

	for (auto& [name, meshGroup] : meshGroups_) {
		// instance数が0なら処理をしない
		if (meshGroup.numInstance == 0) {
			continue;
		}

		// buffer転送
		meshGroup.matrix.TransferVectorData(meshGroup.matrixComponents);
		for (uint32_t meshIndex = 0; meshIndex < meshGroup.model.model->GetMeshNum(); ++meshIndex) {

			meshGroup.materials[meshIndex].TransferVectorData(meshGroup.materialComponents[meshIndex]);
		}
	}
}

void InstancedMesh::Reset() {

	// 何もなければ処理をしない
	if (meshGroups_.empty()) {
		return;
	}

	for (auto& meshGroup : std::views::values(meshGroups_)) {

		// componentクリア
		meshGroup.matrixComponents.clear();

		for (uint32_t meshIndex = 0; meshIndex < meshGroup.model.model->GetMeshNum(); ++meshIndex) {

			meshGroup.materialComponents[meshIndex].clear();
		}
		meshGroup.numInstance = 0;
	}
}
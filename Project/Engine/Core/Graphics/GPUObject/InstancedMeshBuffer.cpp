#include "InstancedMeshBuffer.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Descriptors/SRVDescriptor.h>
#include <Lib/MathUtils/Algorithm.h>

//============================================================================
//	InstancedMeshBuffer classMethods
//============================================================================

void InstancedMeshBuffer::SetDevice(ID3D12Device* device) {

	device_ = nullptr;
	device_ = device;
}

void InstancedMeshBuffer::CreateBuffers(const std::string& name) {

	auto& meshGroup = meshGroups_[name];

	// transform
	// buffer作成
	meshGroup.matrix.CreateStructuredBuffer(device_, meshGroup.maxInstance);

	// material
	// meshの数だけ作成する
	const size_t meshNum = meshGroup.meshNum;
	meshGroup.materials.resize(meshNum);
	// componentの数もここで決める
	meshGroup.materialUploadData.resize(meshNum);

	for (uint32_t meshIndex = 0; meshIndex < meshNum; ++meshIndex) {

		// buffer作成
		meshGroup.materials[meshIndex].CreateStructuredBuffer(device_, meshGroup.maxInstance);
	}
}

void InstancedMeshBuffer::Create(Mesh* mesh,
	const std::string& name, uint32_t numInstance) {

	// すでにある場合は作成しない
	if (Algorithm::Find(meshGroups_, name)) {
		return;
	}

	// 最大のinstance数設定
	meshGroups_[name].maxInstance = numInstance;
	// 描画を行うrenderingDataの設定
	meshGroups_[name].meshNum = mesh->GetMeshCount();

	// bufferの作成
	CreateBuffers(name);
}

void InstancedMeshBuffer::SetUploadData(const std::string& name,
	const TransformationMatrix& matrix, const std::vector<MaterialComponent>& materials) {

	meshGroups_[name].matrixUploadData.emplace_back(matrix);

	for (uint32_t meshIndex = 0; meshIndex < meshGroups_[name].materialUploadData.size(); ++meshIndex) {

		meshGroups_[name].materialUploadData[meshIndex].emplace_back(materials[meshIndex].material);
	}

	// instance数インクリメント
	++meshGroups_[name].numInstance;

	// 最大instance数を超えたらエラー
	if (meshGroups_[name].numInstance > meshGroups_[name].maxInstance) {

		ASSERT(FALSE, "numInstance > maxInstance");
	}
}

void InstancedMeshBuffer::Update() {

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
		meshGroup.matrix.TransferVectorData(meshGroup.matrixUploadData);
		for (uint32_t meshIndex = 0; meshIndex < meshGroup.meshNum; ++meshIndex) {

			meshGroup.materials[meshIndex].TransferVectorData(meshGroup.materialUploadData[meshIndex]);
		}
	}
}

void InstancedMeshBuffer::Reset() {

	// 何もなければ処理をしない
	if (meshGroups_.empty()) {
		return;
	}

	for (auto& meshGroup : std::views::values(meshGroups_)) {

		// componentクリア
		meshGroup.matrixUploadData.clear();

		for (uint32_t meshIndex = 0; meshIndex < meshGroup.meshNum; ++meshIndex) {
			if (meshGroup.materialUploadData[meshIndex].size() != 0) {

				meshGroup.materialUploadData[meshIndex].clear();
			}
		}
		meshGroup.numInstance = 0;
	}
}
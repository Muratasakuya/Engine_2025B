#include "RaytracingScene.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Mesh/Mesh.h>

//============================================================================
//	RaytracingScene classMethods
//============================================================================

void RaytracingScene::Init(ID3D12Device8* device) {

	device_ = nullptr;
	device_ = device;

	first_ = true;
}

void RaytracingScene::BuildBLASes(ID3D12GraphicsCommandList6* commandList, const std::vector<IMesh*>& meshes) {

	for (auto& mesh : meshes) {

		const uint32_t subCount = mesh->GetMeshCount();
		for (uint32_t i = 0; i < subCount; ++i) {

			MeshKey key{ mesh, i };
			auto& blas = blases_[key];
			if (!blas.GetResource()) {

				BuildRequest request{ mesh, i, mesh->IsSkinned() };
				blas.Build(device_, commandList, request);
			} else if (mesh->IsSkinned()) {

				// 更新処理
				blas.Update(commandList);
			}
		}
	}
}

void RaytracingScene::BuildTLAS(ID3D12GraphicsCommandList6* commandList,
	const std::vector<RayTracingInstance>& instances) {

	if (first_) {

		tlas_.Build(device_, commandList, instances, true);
		first_ = false;
	} else {

		tlas_.Update(commandList, instances);
	}
}

ID3D12Resource* RaytracingScene::GetBLASResource(IMesh* mesh, uint32_t meshCount) const {

	MeshKey key{ mesh, meshCount };
	auto it = blases_.find(key);
	return (it != blases_.end()) ? it->second.GetResource() : nullptr;
}
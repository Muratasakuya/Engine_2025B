#include "RenderObjectManager.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Component/Manager/ComponentManager.h>

//============================================================================
//	RenderObjectManager classMethods
//============================================================================

void RenderObjectManager::Init(ID3D12Device* device, SRVManager* srvManager) {

	instancedMesh_ = std::make_unique<InstancedMesh>();
	instancedMesh_->Init(device, srvManager);
}

void RenderObjectManager::CreateObject3D(EntityID id, const std::optional<std::string>& instancingName,
	ModelComponent* model, ID3D12Device* device) {

	object3DBuffers_.emplace_back();

	// instancing処理
	if (instancingName.has_value()) {

		// 最大instance数
		const uint32_t kMaxInstanceNum = 4096;

		// instancingデータ作成
		instancedMesh_->Create(*model, *instancingName, kMaxInstanceNum);

		object3DBuffers_[id].model.renderingData.instancingEnable = true;
		object3DBuffers_[id].model.renderingData.instancingName = *instancingName;
	}
	// 通常描画処理
	else {

		// buffer作成
		object3DBuffers_[id].matrix.CreateConstBuffer(device);

		if (model->isAnimation) {

			object3DBuffers_[id].materials.resize(model->animationModel->GetMeshNum());
		} else {

			object3DBuffers_[id].materials.resize(model->model->GetMeshNum());
		}
		for (auto& material : object3DBuffers_[id].materials) {

			material.CreateConstBuffer(device);
		}

		// model情報の取得
		object3DBuffers_[id].model.model = model->model.get();
		object3DBuffers_[id].model.renderingData.instancingEnable = false;
		object3DBuffers_[id].model.animationModel = model->animationModel.get();
		object3DBuffers_[id].model.isAnimation = model->isAnimation;
		object3DBuffers_[id].model.renderingData = model->renderingData;
	}
}

void RenderObjectManager::RemoveObject3D(EntityID id) {

	// buffer削除
	object3DBuffers_.erase(object3DBuffers_.begin() + id);
}

void RenderObjectManager::CreateObject2D(EntityID id, SpriteComponent* sprite, ID3D12Device* device) {

	object2DBuffers_.emplace_back();

	// buffer作成
	object2DBuffers_[id].matrix.CreateConstBuffer(device);
	object2DBuffers_[id].material.CreateConstBuffer(device);

	// spriteの情報を取得
	object2DBuffers_[id].sprite.sprite = sprite;
}

void RenderObjectManager::RemoveObject2D(EntityID id) {

	// buffer削除
	object2DBuffers_.erase(object2DBuffers_.begin() + id);
}

void RenderObjectManager::Update() {

	// buffer更新
	// 3D
	UpdateObject3D();
	// 2D
	UpdateObject2D();
}

void RenderObjectManager::UpdateObject3D() {

	auto componentManager = ComponentManager::GetInstance();

	instancedMesh_->Reset();

	// entityごとのGPUデータ転送
	for (uint32_t index = 0; index < componentManager->GetEntityCount(ComponentType::Object3D); ++index) {

		if (!object3DBuffers_[index].model.renderingData.instancingEnable) {

			object3DBuffers_[index].matrix.TransferData(componentManager->GetComponent<Transform3DComponent>(index)->matrix);

			for (uint32_t mIndex = 0; mIndex < object3DBuffers_[index].materials.size(); ++mIndex) {

				object3DBuffers_[index].materials[mIndex].TransferData(*componentManager->GetComponent<Material>(index));
			}
		} else {

			std::vector<Material*> materialPtrs = componentManager->GetComponentList<Material>(index);
			std::vector<Material> materials;
			materials.reserve(materialPtrs.size());

			for (const auto* mat : materialPtrs) {

				materials.emplace_back(*mat);
			}

			const ModelComponent* model = componentManager->GetComponent<ModelComponent>(index);
			const Transform3DComponent* transform = componentManager->GetComponent<Transform3DComponent>(index);
			TransformationMatrix matrix{};

			matrix = transform->matrix;
			// 描画しないのであればmatrixを0.0fにする
			// modelを1個ごとに操作できないため
			if (!model->renderingData.drawEnable) {

				matrix.world = Matrix4x4::Zero();
				matrix.worldInverseTranspose = Matrix4x4::Zero();
			}

			// bufferに送るデータをセット
			instancedMesh_->SetComponent(object3DBuffers_[index].model.renderingData.instancingName,
				matrix, materials);
		}
	}

	instancedMesh_->Update();
}

void RenderObjectManager::UpdateObject2D() {

	auto componentManager = ComponentManager::GetInstance();

	// entityごとのGPUデータ転送
	for (uint32_t index = 0; index < componentManager->GetEntityCount(ComponentType::Object2D); ++index) {

		object2DBuffers_[index].matrix.TransferData(componentManager->GetComponent<Transform2DComponent>(index)->matrix);
		object2DBuffers_[index].material.TransferData(*componentManager->GetComponent<SpriteMaterial>(index));
	}
}
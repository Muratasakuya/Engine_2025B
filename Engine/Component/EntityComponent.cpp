#include "EntityComponent.h"

//============================================================================
//	EntityComponent classMethods
//============================================================================

EntityComponent* EntityComponent::instance_ = nullptr;

EntityComponent* EntityComponent::GetInstance() {

	if (instance_ == nullptr) {
		instance_ = new EntityComponent();
	}
	return instance_;
}

void EntityComponent::Finalize() {

	if (instance_ != nullptr) {

		delete instance_;
		instance_ = nullptr;
	}
}

EntityData* EntityComponent::AddComponent(const std::string& modelName,
	const std::optional<std::string>& animationName) {

	// entity追加
	uint32_t entityID = nextID_++;
	entities_[entityID] = EntityData();
	EntityData* entityData = &entities_[entityID];
	entityData->id = entityID;

	// Animationを行うかどうか
	if (animationName.has_value()) {

		buffers_[entityID] = EntityAnimationBufferData();

		// buffer作成
		if (auto* animationBuffer = std::get_if<EntityAnimationBufferData>(&buffers_[entityID])) {

			animationBuffer->model = std::make_unique<AnimationModel>();
			animationBuffer->model->Init(modelName, *animationName, device_, asset_, srvManager_);

			animationBuffer->transform.Init(*animationName, asset_);
			animationBuffer->materials.resize(animationBuffer->model->GetMeshNum());
			entityData->materials.resize(animationBuffer->model->GetMeshNum());
			for (auto& material : animationBuffer->materials) {

				material.CreateConstBuffer(device_);
			}
		}
	} else {

		buffers_[entityID] = EntityBufferData();

		// buffer作成
		if (auto* buffer = std::get_if<EntityBufferData>(&buffers_[entityID])) {

			buffer->model = std::make_unique<BaseModel>();
			buffer->model->Init(modelName, device_, asset_);

			buffer->transform.CreateConstBuffer(device_);
			buffer->materials.resize(buffer->model->GetMeshNum());
			entityData->materials.resize(buffer->model->GetMeshNum());
			for (auto& material : buffer->materials) {

				material.CreateConstBuffer(device_);
			}
		}
	}

	needsSorting_ = true;
	return entityData;
}

void EntityComponent::RemoveComponent(uint32_t entityID) {

	entities_.erase(entityID);
	buffers_.erase(entityID);
}

void EntityComponent::Init(ID3D12Device* device, Asset* asset, SRVManager* srvManager) {

	device_ = nullptr;
	device_ = device;

	asset_ = nullptr;
	asset_ = asset;

	srvManager_ = nullptr;
	srvManager_ = srvManager;
}

void EntityComponent::Update() {

	for (auto& [entityID, entity] : entities_) {

		auto it = buffers_.find(entityID);
		if (auto* model = std::get_if<EntityBufferData>(&it->second)) {

			model->transform.TransferData(entities_[entityID].transform.matrix);
			for (uint32_t meshIndex = 0; meshIndex < model->materials.size(); ++meshIndex) {

				model->materials[meshIndex].TransferData(entities_[entityID].materials[meshIndex]);
			}
			model->renderingData.drawEnable = entities_[entityID].renderingData.drawEnable;
			model->renderingData.blendMode = entities_[entityID].renderingData.blendMode;
		} else if (auto* animationModel = std::get_if<EntityAnimationBufferData>(&it->second)) {

			animationModel->transform.Update(entities_[entityID].transform.matrix);
			for (uint32_t meshIndex = 0; meshIndex < animationModel->materials.size(); ++meshIndex) {

				animationModel->materials[meshIndex].TransferData(entities_[entityID].materials[meshIndex]);
			}
			animationModel->renderingData.drawEnable = entities_[entityID].renderingData.drawEnable;
			animationModel->renderingData.blendMode = entities_[entityID].renderingData.blendMode;
		}
	}
}

const std::unordered_map<BlendMode, std::vector<EntityComponent::EntityBufferPtr>>& EntityComponent::GetBuffers() const {

	if (needsSorting_) {
		SortEntitiesByBlendMode();
	}
	return sortedEntities_;
}

void EntityComponent::SortEntitiesByBlendMode() const {

	// 既存のデータをクリア
	sortedEntities_.clear();

	for (auto& [entityID, entityVariant] : buffers_) {
		if (auto* buffer = std::get_if<EntityBufferData>(&entityVariant)) {

			if (buffer && !buffer->renderingData.drawEnable) continue;

			sortedEntities_[buffer->renderingData.blendMode].push_back(
				std::variant<EntityBufferData*, EntityAnimationBufferData*>(const_cast<EntityBufferData*>(buffer)));
		} else if (auto* animatedBuffer = std::get_if<EntityAnimationBufferData>(&entityVariant)) {

			if (animatedBuffer && !animatedBuffer->renderingData.drawEnable) continue;

			sortedEntities_[animatedBuffer->renderingData.blendMode].push_back(
				std::variant<EntityBufferData*, EntityAnimationBufferData*>(const_cast<EntityAnimationBufferData*>(animatedBuffer)));
		}
	}
	// ソート処理完了
	needsSorting_ = false;
}
#define NOMINMAX

#include "ModelComponentManager.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Managers/SRVManager.h>
#include <Lib/MathUtils/Algorithm.h>

//============================================================================
//	ModelComponentManager classMethods
//============================================================================

void ModelComponentManager::Init(ID3D12Device* device, DxShaderCompiler* shaderCompiler) {

	skinningPipeline_ = std::make_unique<PipelineState>();
	skinningPipeline_->Create("Skinning.json", device, shaderCompiler);
}

void ModelComponentManager::AddComponent(EntityID entity, std::any args) {

	size_t index = components_.size();

	entityToIndex_[entity] = index;
	indexToEntity_.emplace_back(entity);

	auto [modelName, animationName, device, commandList, asset, srvManager] =
		std::any_cast<std::tuple<
		std::string, std::optional<std::string>, ID3D12Device*, ID3D12GraphicsCommandList*, Asset*, SRVManager*>>(args);

	if (!commandList_.has_value()) {
		commandList_ = commandList;
	}

	// component追加
	components_.resize(std::max(static_cast<EntityID>(components_.size()), entity + 1));

	components_[entity] = ModelComponent();

	// Animation有無
	components_[entity].isAnimation = animationName.has_value();

	// 初期化
	components_[entity].renderingData.drawEnable = true;
	components_[entity].renderingData.blendMode = BlendMode::kBlendModeNormal;

	if (components_[entity].isAnimation) {

		components_[entity].animationModel = std::make_unique<AnimationModel>();
		components_[entity].animationModel->Init(modelName, *animationName, device, commandList, asset, srvManager);
	} else {

		components_[entity].model = std::make_unique<BaseModel>();
		components_[entity].model->Init(modelName, device, asset);
	}
}

void ModelComponentManager::RemoveComponent(EntityID entity) {

	size_t index = entityToIndex_.at(entity);
	size_t lastIndex = components_.size() - 1;

	if (index != lastIndex) {

		// 末尾と交換
		std::swap(components_[index], components_[lastIndex]);

		// 交換されたentityIdを更新
		EntityID movedEntityId = indexToEntity_[lastIndex];
		entityToIndex_[movedEntityId] = index;
		indexToEntity_[index] = movedEntityId;
	}

	// 末尾を削除
	components_.pop_back();
	indexToEntity_.pop_back();
	entityToIndex_.erase(entity);
}

void ModelComponentManager::Update() {

	for (const auto& component : components_) {
		
		// skinningAnimation処理
		if (component.isAnimation) {
			if (!setPipeline_) {

				commandList_.value()->SetComputeRootSignature(skinningPipeline_->GetRootSignature());
				commandList_.value()->SetPipelineState(skinningPipeline_->GetComputePipeline());
				setPipeline_ = true;
			}

			component.animationModel->Skinning();
		}
	}

	setPipeline_ = false;
}

ModelComponent* ModelComponentManager::GetComponent(EntityID entity) {

	if (Algorithm::Find(entityToIndex_, entity)) {

		size_t index = entityToIndex_.at(entity);
		return &components_.at(index);

	}
	return nullptr;
}
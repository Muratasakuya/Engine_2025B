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

	auto [modelName, animationName, device, commandList, asset, srvManager] =
		std::any_cast<std::tuple<
		std::string, std::optional<std::string>, ID3D12Device*, ID3D12GraphicsCommandList*, Asset*, SRVManager*>>(args);

	if (!commandList_.has_value()) {
		commandList_ = commandList;
	}

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

	components_.erase(entity);
}

void ModelComponentManager::Update() {

	for (auto& [entityID, component] : components_) {
		
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

	if (Algorithm::Find(components_, entity, true)) {

		return &components_[entity];
	}
	return nullptr;
}
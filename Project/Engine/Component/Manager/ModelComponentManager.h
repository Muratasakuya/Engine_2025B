#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Component/ModelComponent.h>
#include <Engine/Component/Base/IComponent.h>
#include <Engine/Core/Graphics/Pipeline/PipelineState.h>

//============================================================================
//	ModelComponentManager class
//============================================================================
class ModelComponentManager :
	public IComponent<ModelComponent> {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	ModelComponentManager() = default;
	~ModelComponentManager() = default;

	void Init(ID3D12Device* device, DxShaderCompiler* shaderCompiler);

	void AddComponent(EntityID entity, std::any args) override;
	void RemoveComponent(EntityID entity) override;

	void Update() override;

	//--------- accessor -----------------------------------------------------

	ModelComponent* GetComponent(EntityID entity) override;
	std::vector<ModelComponent*> GetComponentList([[maybe_unused]] EntityID entity) override { return { nullptr }; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::optional<ID3D12GraphicsCommandList*> commandList_ = std::nullopt;
	bool setPipeline_;

	std::unique_ptr<PipelineState> skinningPipeline_;

	std::vector<ModelComponent> components_;
};
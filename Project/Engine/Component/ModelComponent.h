#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/CBuffer/InputAssembler.h>
#include <Engine/Core/CBuffer/IOVertexBuffer.h>
#include <Engine/Core/Lib/DxStructures.h>

// c++
#include <optional>
// front
class Asset;

//============================================================================
//	BaseModel class
//============================================================================
class BaseModel {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	BaseModel() = default;
	virtual ~BaseModel() = default;

	void Init(const std::string& modelName, ID3D12Device* device, Asset* asset);

	//--------- accessor -----------------------------------------------------

	void SetTexture(const std::string& textureName, const std::optional<uint32_t>& meshIndex = std::nullopt);

	const InputAssembler& GetIA() const { return inputAssembler_; }

	const std::string& GetTexture(uint32_t meshIndex) const { return *modelData_.meshes[meshIndex].textureName; }
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetTextureGPUHandle(uint32_t meshIndex);
	uint32_t GetTextureGPUIndex(uint32_t meshIndex) const;

	const ModelData& GetModelData() const { return modelData_; }

	size_t GetMeshNum() const { return meshNum_; }
protected:
	//========================================================================
	//	protected Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	ModelData modelData_;
	size_t meshNum_;

	InputAssembler inputAssembler_;

	Asset* asset_;
};

//============================================================================
//	AnimationModel class
//============================================================================
class AnimationModel :
	public BaseModel {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	AnimationModel() = default;
	~AnimationModel() = default;

	void Init(const std::string& modelName, const std::string& animationName,
		ID3D12Device* device, ID3D12GraphicsCommandList* commandList,
		Asset* asset, class SRVManager* srvManager);

	void Skinning();

	//--------- accessor -----------------------------------------------------

	void SetAnimationName(const std::string& animationName) { animationName_ = animationName; }

	const IOVertexBuffer& GetIOVertex() { return ioVertexBuffer_; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	Asset* asset_;
	ID3D12GraphicsCommandList* commandList_;

	IOVertexBuffer ioVertexBuffer_;
	// SkinningInfo
	DxConstBuffer<uint32_t> skinningInfoDates_;

	std::string animationName_;
};

//============================================================================
//	ModelComponent structure
//============================================================================
// 描画情報
struct RenderingData {

	bool drawEnable;
	BlendMode blendMode;

	bool instancingEnable;
	std::string instancingName;
};

struct ModelComponent {

	std::unique_ptr<BaseModel> model;
	std::unique_ptr<AnimationModel> animationModel;

	bool isAnimation;

	RenderingData renderingData;

	void ImGui();
};

// 受け取るmodel情報
struct ModelReference {

	BaseModel* model;
	AnimationModel* animationModel;

	bool isAnimation;

	RenderingData renderingData;
};
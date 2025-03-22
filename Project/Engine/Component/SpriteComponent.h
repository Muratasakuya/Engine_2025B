#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/AssetStructure.h>
#include <Engine/Component/TransformComponent.h>
#include <Engine/Core/CBuffer/DxConstBuffer.h>

// c++
#include <string>
// front
class Asset;

//============================================================================
//	enum class
//============================================================================

// 描画を行う場所
enum class SpriteLayer {

	PreModel, // modelの前に描画する
	PostModel // modelの後に描画する
};

//============================================================================
//	SpriteComponent class
//============================================================================
class SpriteComponent {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	SpriteComponent(ID3D12Device* device, Asset* asset,
		const std::string& textureName, Transform2DComponent& transform);
	~SpriteComponent() = default;

	void UpdateVertex(const Transform2DComponent& transform);

	//--------- accessor -----------------------------------------------------

	void SetLayer(SpriteLayer layer) { layer_ = layer; }
	void SetPostProcessEnable(bool enable) { postProcessEnable_ = enable; }

	static uint32_t GetIndexNum() { return kIndexNum_; }

	SpriteLayer GetLayer() const { return layer_; }
	bool GetPostProcessEnable() const { return postProcessEnable_; }

	const DxConstBuffer<SpriteVertexData>& GetVertexBuffer() const { return vertexBuffer_; }
	const DxConstBuffer<uint32_t>& GetIndexBuffer() const { return indexBuffer_; }

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetTextureGPUHandle() const;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	static constexpr const uint32_t kVertexNum_ = 4;
	static constexpr const uint32_t kIndexNum_ = 6;

	Asset* asset_;

	std::string textureName_;

	SpriteLayer layer_;
	bool postProcessEnable_;

	// 頂点情報
	std::vector<SpriteVertexData> vertexData_;

	// buffer
	DxConstBuffer<SpriteVertexData> vertexBuffer_;
	DxConstBuffer<uint32_t> indexBuffer_;

	//--------- functions ----------------------------------------------------

	void InitBuffer(ID3D12Device* device);

	void SetMetaDataTextureSize(Transform2DComponent& transform);
};

//============================================================================
//	SpriteComponent structure
//============================================================================

// 受け取るsprite情報
struct SpriteReference {

	SpriteComponent* sprite;
};
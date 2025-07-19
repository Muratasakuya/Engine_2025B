#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/AssetStructure.h>
#include <Engine/Object/Data/Transform.h>
#include <Engine/Core/Graphics/GPUObject/VertexBuffer.h>
#include <Engine/Core/Graphics/GPUObject/IndexBuffer.h>

// directX
#include <Externals/DirectXTex/DirectXTex.h>
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
//	Sprite class
//============================================================================
class Sprite {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	Sprite() = default;
	Sprite(ID3D12Device* device, Asset* asset,
		const std::string& textureName, Transform2D& transform);
	~Sprite() = default;

	void UpdateVertex(const Transform2D& transform);

	void ImGui(float itemSize);

	//--------- accessor -----------------------------------------------------

	void SetTextureName(const std::string& textureName) { textureName_ = textureName; }
	void SetAlphaTextureName(const std::string& textureName) { alphaTextureName_ = textureName; }

	void SetLayer(SpriteLayer layer) { layer_ = layer; }
	void SetPostProcessEnable(bool enable) { postProcessEnable_ = enable; }

	static uint32_t GetIndexNum() { return kIndexNum_; }

	SpriteLayer GetLayer() const { return layer_; }
	bool GetPostProcessEnable() const { return postProcessEnable_; }
	bool UseAlphaTexture() const { return alphaTextureName_.has_value(); }

	const VertexBuffer<SpriteVertexData>& GetVertexBuffer() const { return vertexBuffer_; }
	const IndexBuffer& GetIndexBuffer() const { return indexBuffer_; }

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetTextureGPUHandle() const;
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetAlphaTextureGPUHandle() const;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	static constexpr const uint32_t kVertexNum_ = 4;
	static constexpr const uint32_t kIndexNum_ = 6;

	Asset* asset_;

	std::string textureName_;
	std::string preTextureName_;
	std::optional<std::string> alphaTextureName_;
	DirectX::TexMetadata metadata_;

	SpriteLayer layer_;
	bool postProcessEnable_;

	// 頂点情報
	std::vector<SpriteVertexData> vertexData_;

	// buffer
	VertexBuffer<SpriteVertexData> vertexBuffer_;
	IndexBuffer indexBuffer_;

	//--------- functions ----------------------------------------------------

	void InitBuffer(ID3D12Device* device);

	void SetMetaDataTextureSize(Transform2D& transform);
};
#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/GPUObject/DxConstBuffer.h>
#include <Engine/Core/ECS/Components/TransformComponent.h>
#include <Engine/Core/ECS/Components/MaterialComponent.h>
#include <Lib/MathUtils/MathUtils.h>

//============================================================================
//	SkyboxComponent class
//============================================================================
class SkyboxComponent {
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- structure ----------------------------------------------------

	struct SkyboxMaterial {

		Color color;
		uint32_t textureIndex;
		float pad0[3];
		Matrix4x4 uvTransform;
	};
public:
	//========================================================================
	//	public Methods
	//========================================================================

	SkyboxComponent() = default;
	~SkyboxComponent() = default;

	void Create(ID3D12Device* device, uint32_t textureIndex);

	void Update();

	void ImGui(float itemSize);

	//--------- accessor -----------------------------------------------------

	uint32_t GetIndexCount() const { return indexCount_; }
	uint32_t GetTextureIndex() const { return material_.textureIndex; }

	// buffers
	const DxConstBuffer<Vector4>& GetVertexBuffer() const { return vertexBuffer_; }
	const DxConstBuffer<uint32_t>& GetIndexBuffer() const { return indexBuffer_; }

	const DxConstBuffer<Matrix4x4>& GetMatrixBuffer() const { return matrixBuffer_; }
	const DxConstBuffer<SkyboxMaterial>& GetMaterialBuffer() const { return materialBuffer_; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// 頂点buffer
	DxConstBuffer<Vector4> vertexBuffer_;
	// 頂点インデックスbuffer
	DxConstBuffer<uint32_t> indexBuffer_;
	uint32_t indexCount_;

	BaseTransform transform_;
	SkyboxMaterial material_;

	// uv
	UVTransform uvTransform_;
	UVTransform prevUVTransform_;

	// cBuffer
	DxConstBuffer<Matrix4x4> matrixBuffer_;
	DxConstBuffer<SkyboxMaterial> materialBuffer_;

	//--------- functions ----------------------------------------------------

	void CreateVertexBuffer(ID3D12Device* device);
	void CreateCBuffer(ID3D12Device* device, uint32_t textureIndex);

	// update
	void UpdateUVTransform();
};
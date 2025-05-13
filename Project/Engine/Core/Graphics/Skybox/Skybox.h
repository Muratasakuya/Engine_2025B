#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/GPUObject/DxConstBuffer.h>
#include <Engine/Core/Component/TransformComponent.h>
#include <Engine/Editor/Base/IGameEditor.h>
#include <Lib/MathUtils/MathUtils.h>

//============================================================================
//	Skybox class
//============================================================================
class Skybox :
	public IGameEditor {
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- structure ----------------------------------------------------

	struct SkyboxMaterial {

		Color color;
		uint32_t textureIndex;
	};
public:
	//========================================================================
	//	public Methods
	//========================================================================

	Skybox() :IGameEditor("Skybox") {};
	~Skybox() = default;

	void Create(uint32_t textureIndex);

	void Update();

	void ImGui() override;

	//--------- accessor -----------------------------------------------------

	void SetDevice(ID3D12Device* device) { device_ = device; };

	bool IsCreated() const { return isCreated_; }
	uint32_t GetIndexCount() const { return indexCount_; }
	uint32_t GetTextureIndex() const { return material_.textureIndex; }

	// buffers
	const DxConstBuffer<Vector4>& GetVertexBuffer() const { return vertexBuffer_; }
	const DxConstBuffer<uint32_t>& GetIndexBuffer() const { return indexBuffer_; }

	const DxConstBuffer<Matrix4x4>& GetMatrixBuffer() const { return matrixBuffer_; }
	const DxConstBuffer<SkyboxMaterial>& GetMaterialBuffer() const { return materialBuffer_; }

	// singleton、絶対にやめる
	static Skybox* GetInstance();
	static void Finalize();
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	static Skybox* instance_;

	ID3D12Device* device_;

	// 作成されたかどうか
	bool isCreated_;

	// 頂点buffer
	DxConstBuffer<Vector4> vertexBuffer_;
	// 頂点インデックスbuffer
	DxConstBuffer<uint32_t> indexBuffer_;
	uint32_t indexCount_;

	// cBufferに渡す値
	BaseTransform transform_;
	SkyboxMaterial material_;

	// cBuffer
	DxConstBuffer<Matrix4x4> matrixBuffer_;
	DxConstBuffer<SkyboxMaterial> materialBuffer_;

	//--------- functions ----------------------------------------------------

	void CreateVertexBuffer();
	void CreateCBuffer(uint32_t textureIndex);
};
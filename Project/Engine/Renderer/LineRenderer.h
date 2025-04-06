#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Pipeline/PipelineState.h>
#include <Engine/Core/Graphics/GPUObject/DxConstBuffer.h>
#include <Engine/Core/Lib/Collision.h>
#include <Lib/MathUtils/MathUtils.h>

// c++
#include <memory>
// front
class CameraManager;

//============================================================================
//	LineRenderer class
//============================================================================
class LineRenderer {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	void Init(ID3D12Device* device, ID3D12GraphicsCommandList* commandList,
		class DxShaderCompiler* shaderCompiler, CameraManager* cameraManager);

	void DrawLine3D(const Vector3& pointA, const Vector3& pointB, const Color& color);

	void ExecuteLine(bool debugEnable);

	void ResetLine();

	void DrawGrid(int division, float gridSize, const Color& color);

	void DrawSphere(int division, float radius, const Vector3& centerPos, const Color& color);

	void DrawOBB(const CollisionShape::OBB& obb, const Color& color);

	//--------- accessor -----------------------------------------------------

	// singleton
	static LineRenderer* GetInstance();
	static void Finalize();
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- structure ----------------------------------------------------

	// 頂点情報
	struct LineVertex {

		Vector4 pos;
		Color color;
	};

	//--------- variables ----------------------------------------------------

	static LineRenderer* instance_;

	ID3D12GraphicsCommandList* commandList_;
	CameraManager* cameraManager_;

	// 線分の最大数
	static constexpr const uint32_t kMaxLineCount_ = 2048;
	// 線分の頂点数
	static constexpr const uint32_t kVertexCountLine_ = 2;

	std::unique_ptr<PipelineState> pipeline_;

	std::vector<LineVertex> lineVertices_;
	DxConstBuffer<LineVertex> vertexBuffer_;

	DxConstBuffer<Matrix4x4> viewProjectionBuffer_;
	DxConstBuffer<Matrix4x4> debugSceneViewProjectionBuffer_;
	//--------- functions ----------------------------------------------------

	LineRenderer() = default;
	~LineRenderer() = default;
	LineRenderer(const LineRenderer&) = delete;
	LineRenderer& operator=(const LineRenderer&) = delete;
};
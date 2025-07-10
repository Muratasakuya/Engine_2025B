#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Pipeline/PipelineState.h>
#include <Engine/Core/Graphics/GPUObject/DxConstBuffer.h>
#include <Engine/Collision/CollisionGeometry.h>
#include <Lib/MathUtils/MathUtils.h>

// c++
#include <memory>
// front
class SceneView;
class SRVDescriptor;
class DxShaderCompiler;

//============================================================================
//	LineRenderer class
//============================================================================
class LineRenderer {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	void Init(ID3D12Device8* device, ID3D12GraphicsCommandList* commandList,
		SRVDescriptor* srvDescriptor, DxShaderCompiler* shaderCompiler,
		SceneView* sceneView);

	void DrawLine3D(const Vector3& pointA, const Vector3& pointB, const Color& color);
	void DrawDepthIgonreLine3D(const Vector3& pointA, const Vector3& pointB, const Color& color);

	void ExecuteLine(bool debugEnable);
	void ExecuteDepthIgonreLine(bool debugEnable);

	void ResetLine();

	void DrawGrid(int division, float gridSize, const Color& color);

	//--------- shapes ------------------------------------------------------

	void DrawSphere(int division, float radius, const Vector3& centerPos, const Color& color);
	void DrawDepthIgonreSphere(int division, float radius, const Vector3& centerPos, const Color& color);

	void DrawHemisphere(int division, float radius, const Vector3& centerPos,
		const Vector3& eulerRotate, const Color& color);

	void DrawAABB(const Vector3& min, const Vector3& max, const Color& color);

	void DrawOBB(const CollisionShape::OBB& obb, const Color& color);
	void DrawOBB(const Vector3& centerPos, const Vector3& size,
		const Vector3& eulerRotate, const Color& color);

	void DrawCone(int division, float baseRadius, float topRadius, float height,
		const Vector3& centerPos, const Vector3& eulerRotate, const Color& color);

	void DrawCircle(int division, float radius, const Vector3& center, const Color& color);

	void DrawArc(int division, float radius, float halfAngle,
		const Vector3& center, const Vector3& direction, const Color& color);

	void DrawSquare(float length, const Vector3& center, const Color& color);

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

	// 線の種類
	enum class LineType {

		None,        // 通常描画
		DepthIgnore, // 深度値無視
	};

	// 描画に使うデータ
	struct RenderStructure {

		std::unique_ptr<PipelineState> pipeline;

		std::vector<LineVertex> lineVertices;
		DxConstBuffer<LineVertex> vertexBuffer;

		void Init(const std::string& pipelineFile, ID3D12Device8* device,
			SRVDescriptor* srvDescriptor, DxShaderCompiler* shaderCompiler);
	};

	//--------- variables ----------------------------------------------------

	static LineRenderer* instance_;

	ID3D12GraphicsCommandList* commandList_;
	SceneView* sceneView_;

	// 線分の最大数
	static constexpr const uint32_t kMaxLineCount_ = 8196;
	// 線分の頂点数
	static constexpr const uint32_t kVertexCountLine_ = 2;

	// カメラ視点
	DxConstBuffer<Matrix4x4> viewProjectionBuffer_;
	DxConstBuffer<Matrix4x4> debugSceneViewProjectionBuffer_;

	// 描画情報(buffer)
	std::unordered_map<LineType, RenderStructure> renderData_;

	//--------- functions ----------------------------------------------------

	LineRenderer() = default;
	~LineRenderer() = default;
	LineRenderer(const LineRenderer&) = delete;
	LineRenderer& operator=(const LineRenderer&) = delete;
};
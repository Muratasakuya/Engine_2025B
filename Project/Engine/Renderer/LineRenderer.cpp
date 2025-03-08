#include "LineRenderer.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Debug/Assert.h>
#include <Game/Camera/Manager/CameraManager.h>

//============================================================================
//	LineRenderer classMethods
//============================================================================

LineRenderer* LineRenderer::instance_ = nullptr;

LineRenderer* LineRenderer::GetInstance() {

	if (instance_ == nullptr) {
		instance_ = new LineRenderer();
	}
	return instance_;
}

void LineRenderer::Finalize() {

	if (instance_ != nullptr) {

		delete instance_;
		instance_ = nullptr;
	}
}

void LineRenderer::Init(ID3D12Device* device, ID3D12GraphicsCommandList* commandList,
	DxShaderCompiler* shaderCompiler, CameraManager* cameraManager) {

	commandList_ = nullptr;
	commandList_ = commandList;

	cameraManager_ = nullptr;
	cameraManager_ = cameraManager;

	pipeline_ = std::make_unique<PipelineState>();
	pipeline_->Create("PrimitiveLine.json", device, shaderCompiler);

	vertexBuffer_.CreateVertexBuffer(device, kMaxLineCount_ * kVertexCountLine_);

	viewProjectionBuffer_.CreateConstBuffer(device);
#ifdef _DEBUG
	debugSceneViewProjectionBuffer_.CreateConstBuffer(device);
#endif // _DEBUG
}

void LineRenderer::DrawLine3D(const Vector3& pointA, const Vector3& pointB, const Color& color) {

	ASSERT(lineVertices_.size() < kMaxLineCount_ * kVertexCountLine_, "exceeded the upper limit line");

	// 頂点追加
	lineVertices_.emplace_back(Vector4(pointA.x, pointA.y, pointA.z, 1.0f), color);
	lineVertices_.emplace_back(Vector4(pointB.x, pointB.y, pointB.z, 1.0f), color);
}

void LineRenderer::ExecuteLine(bool debugEnable) {
#ifdef _DEBUG
	if (lineVertices_.empty()) {
		return;
	}

	commandList_->SetGraphicsRootSignature(pipeline_->GetRootSignature());
	commandList_->SetPipelineState(pipeline_->GetGraphicsPipeline());
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	vertexBuffer_.TransferVectorData(lineVertices_);
	commandList_->IASetVertexBuffers(0, 1, &vertexBuffer_.GetVertexBuffer());

	if (!debugEnable) {

		viewProjectionBuffer_.TransferData(cameraManager_->GetCamera()->GetViewProjectionMatrix());
		commandList_->SetGraphicsRootConstantBufferView(0, viewProjectionBuffer_.GetResourceAdress());
	} else {

		debugSceneViewProjectionBuffer_.TransferData(cameraManager_->GetDebugCamera()->GetViewProjectionMatrix());
		commandList_->SetGraphicsRootConstantBufferView(0, debugSceneViewProjectionBuffer_.GetResourceAdress());
	}

	commandList_->DrawInstanced(static_cast<UINT>(lineVertices_.size()), 1, 0, 0);
#endif // _DEBUG
}

void LineRenderer::ResetLine() {

	lineVertices_.clear();
}

void LineRenderer::DrawGrid(int division, float gridSize, const Color& color) {

	const float kGridEvery = (gridSize * 2.0f) / float(division);

	for (int index = 0; index <= division; ++index) {

		float offset = -gridSize + index * kGridEvery;

		// 横
		Vector3 verticalStart(offset, 0.0f, gridSize);
		Vector3 verticalEnd(offset, 0.0f, -gridSize);
		DrawLine3D(verticalStart, verticalEnd, color);

		// 縦
		Vector3 horizontalStart(-gridSize, 0.0f, offset);
		Vector3 horizontalEnd(gridSize, 0.0f, offset);
		DrawLine3D(horizontalStart, horizontalEnd, color);
	}
}

void LineRenderer::DrawSphere(int division, float radius, const Vector3& centerPos, const Color& color) {

	const float kLatEvery = pi / division;        //* 緯度
	const float kLonEvery = 2.0f * pi / division; //* 経度

	auto calculatePoint = [&](float lat, float lon) -> Vector3 {
		return {
			radius * std::cos(lat) * std::cos(lon),
			radius * std::sin(lat),
			radius * std::cos(lat) * std::sin(lon)
		};
		};

	for (int latIndex = 0; latIndex < division; ++latIndex) {

		float lat = -pi / 2.0f + kLatEvery * latIndex;
		for (int lonIndex = 0; lonIndex < division; ++lonIndex) {
			float lon = lonIndex * kLonEvery;

			Vector3 pointA = calculatePoint(lat, lon);
			Vector3 pointB = calculatePoint(lat + kLatEvery, lon);
			Vector3 pointC = calculatePoint(lat, lon + kLonEvery);

			DrawLine3D(pointA + centerPos, pointB + centerPos, color);
			DrawLine3D(pointA + centerPos, pointC + centerPos, color);
		}
	}
}

void LineRenderer::DrawOBB(const CollisionShape::OBB& obb, const Color& color) {

	const uint32_t vertexNum = 8;

	Matrix4x4 rotateMatrix = Quaternion::MakeRotateMatrix(obb.rotate);

	Vector3 vertices[vertexNum];
	Vector3 halfSizeX = Vector3::Transform(Vector3(1.0f, 0.0f, 0.0f), rotateMatrix) * obb.size.x;
	Vector3 halfSizeY = Vector3::Transform(Vector3(0.0f, 1.0f, 0.0f), rotateMatrix) * obb.size.y;
	Vector3 halfSizeZ = Vector3::Transform(Vector3(0.0f, 0.0f, 1.0f), rotateMatrix) * obb.size.z;

	Vector3 offsets[vertexNum] = {
		{-1, -1, -1}, {-1,  1, -1}, {1, -1, -1}, {1,  1, -1},
		{-1, -1,  1}, {-1,  1,  1}, {1, -1,  1}, {1,  1,  1}
	};

	for (int i = 0; i < vertexNum; ++i) {

		Vector3 localVertex = offsets[i].x * halfSizeX +
			offsets[i].y * halfSizeY +
			offsets[i].z * halfSizeZ;
		vertices[i] = obb.center + localVertex;
	}

	int edges[12][2] = {
		{0, 1}, {1, 3}, {3, 2}, {2, 0},
		{4, 5}, {5, 7}, {7, 6}, {6, 4},
		{0, 4}, {1, 5}, {2, 6}, {3, 7}
	};

	for (int i = 0; i < 12; ++i) {

		int start = edges[i][0];
		int end = edges[i][1];

		DrawLine3D(vertices[start], vertices[end], color);
	}
}
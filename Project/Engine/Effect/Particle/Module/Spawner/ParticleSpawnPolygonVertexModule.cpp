#include "ParticleSpawnPolygonVertexModule.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Renderer/LineRenderer.h>

//============================================================================
//	ParticleSpawnPolygonVertexModule classMethods
//============================================================================

void ParticleSpawnPolygonVertexModule::Init() {

	// 初期化値
	ICPUParticleSpawnModule::InitCommonData();

	// xを90度回転
	emitterRotation_.x = pi / 2.0f;

	vertexCount_ = 3;
	scale_ = 1.0f;

	emitPerVertex_ = ParticleValue<uint32_t>::SetValue(4);
	prevVertices_ = CalcVertices();
}

std::vector<Vector3> ParticleSpawnPolygonVertexModule::CalcVertices() const {

	std::vector<Vector3> vertices{};
	vertices.reserve(vertexCount_);
	Matrix4x4 rotateMatrix = Matrix4x4::MakeRotateMatrix(emitterRotation_);
	for (int i = 0; i < vertexCount_; ++i) {

		float angle = 2.0f * pi * i / vertexCount_;
		Vector3 local = Vector3(std::cos(angle) * scale_, 0.0f, std::sin(angle) * scale_);

		// 回転適応後の頂点座標
		vertices.push_back(rotateMatrix.TransformPoint(local) + translation_);
	}
	return vertices;
}

void ParticleSpawnPolygonVertexModule::UpdateEmitter() {

	// 前フレーム頂点の保存
	prevVertices_ = CalcVertices();
}

void ParticleSpawnPolygonVertexModule::Execute(std::list<CPUParticle::ParticleData>& particles) {

	const std::vector<Vector3> currentVertices = CalcVertices();
	// 頂点数が変わった時は速度を0.0fにする
	if (prevVertices_.size() != currentVertices.size()) {
		prevVertices_ = currentVertices;
	}

	const uint32_t emitPerVertex = emitPerVertex_.GetValue();
	for (uint32_t cIndex = 0; cIndex < static_cast<uint32_t>(currentVertices.size()); ++cIndex) {

		// 進行方向ベクトル
		Vector3 direction = Vector3::Normalize(currentVertices[cIndex] - prevVertices_[cIndex]);
		bool isMoving = direction.Length() > std::numeric_limits<float>::epsilon();
		// 速度の設定
		Vector3 velocity = isMoving ? direction * moveSpeed_.GetValue() :
			Vector3(0.0f, 0.0f, moveSpeed_.GetValue());

		for (uint32_t pIndex = 0; pIndex < emitPerVertex; ++pIndex) {

			CPUParticle::ParticleData particle{};

			// 共通設定
			ICPUParticleSpawnModule::SetCommonData(particle);

			// 速度、発生位置
			particle.transform.translation = currentVertices[cIndex];
			particle.velocity = velocity;

			// 追加
			particles.push_back(particle);
		}
	}
}

void ParticleSpawnPolygonVertexModule::ImGui() {

	ImGui::DragFloat3("rotation", &emitterRotation_.x, 0.01f);
	ImGui::DragFloat("scale", &scale_, 0.05f);
	ImGui::DragFloat3("translation", &translation_.x, 0.05f);

	ImGui::DragInt("vertexCount", &vertexCount_, 1, 3, 16);
	emitPerVertex_.EditDragValue("emitPerVertex");
}

void ParticleSpawnPolygonVertexModule::DrawEmitter() {

	LineRenderer::GetInstance()->DrawPolygon(vertexCount_,
		translation_, scale_, emitterRotation_, emitterLineColor_);
}
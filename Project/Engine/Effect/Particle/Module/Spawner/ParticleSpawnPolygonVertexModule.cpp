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

	isInterpolate_ = false;
	notMoveEmit_ = false;

	// xを90度回転
	emitterRotation_.x = pi / 2.0f;

	vertexCount_ = 3;
	scale_ = 1.0f;

	emitPerVertex_ = ParticleValue<uint32_t>::SetValue(4);
	interpolateSpacing_ = ParticleValue<float>::SetValue(0.08f);
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

	// 発生判定
	if (!EnableEmit()) {
		return;
	}

	// 補間処理を行う場合
	if (isInterpolate_) {

		InterpolateEmit(particles);
	}
	// 補間処理をしない場合
	else {

		NoneEmit(particles);
	}
}

bool ParticleSpawnPolygonVertexModule::EnableEmit() {

	if (!notMoveEmit_) {
		return true;
	}

	bool moved = false;
	const std::vector<Vector3> currentVertices = CalcVertices();
	const size_t vertexCount = (std::min)(currentVertices.size(), prevVertices_.size());
	// 前フレームの頂点位置と比較する
	for (size_t i = 0; i < vertexCount; ++i) {
		if (std::numeric_limits<float>::epsilon() < (currentVertices[i] - prevVertices_[i]).Length()) {

			moved = true;
			break;
		}
	}
	return moved;
}

void ParticleSpawnPolygonVertexModule::InterpolateEmit(std::list<CPUParticle::ParticleData>& particles) {

	const std::vector<Vector3> currentVertices = CalcVertices();
	// 頂点数が変わった時は速度を0.0fにする
	if (prevVertices_.size() != currentVertices.size()) {
		prevVertices_ = currentVertices;
	}

	const float spacing = interpolateSpacing_.GetValue();
	const uint32_t emitPerVertex = emitPerVertex_.GetValue();
	for (uint32_t v = 0; v < static_cast<uint32_t>(currentVertices.size()); ++v) {

		const Vector3 diff = currentVertices[v] - prevVertices_[v];
		const float length = diff.Length();

		// 頂点が静止している場合は通常発生のみ
		if (length < spacing || length < std::numeric_limits<float>::epsilon()) {

			// 速度設定
			Vector3 velocity = Vector3::Normalize(diff) * moveSpeed_.GetValue();
			for (uint32_t n = 0; n < emitPerVertex; ++n) {

				CPUParticle::ParticleData particle{};

				// 共通設定
				ICPUParticleSpawnModule::SetCommonData(particle);

				// 速度、発生位置
				particle.velocity = velocity;
				particle.transform.translation = currentVertices[v];

				// 追加
				particles.push_back(particle);
			}
			continue;
		}

		// 補間個数と方向
		const uint32_t interpCount = static_cast<uint32_t>(length / spacing);
		const Vector3 direction = diff / length; // 正規化
		const float stepLen = spacing;           // 等間隔
		const Vector3 velocity = direction * moveSpeed_.GetValue();

		// パーティクル間の補間
		for (uint32_t i = 1; i <= interpCount; ++i) {

			const Vector3 pos = prevVertices_[v] + direction * stepLen * static_cast<float>(i);
			for (uint32_t n = 0; n < emitPerVertex; ++n) {

				CPUParticle::ParticleData particle{};

				// 共通設定
				ICPUParticleSpawnModule::SetCommonData(particle);

				// 速度、発生位置
				particle.velocity = velocity;
				particle.transform.translation = pos;

				// 追加
				particles.push_back(particle);
			}

		}

		// 現在のフレームの頂点位置に発生
		for (uint32_t n = 0; n < emitPerVertex; ++n) {

			CPUParticle::ParticleData particle{};

			// 共通設定
			ICPUParticleSpawnModule::SetCommonData(particle);

			// 速度、発生位置
			particle.velocity = velocity;
			particle.transform.translation = currentVertices[v];

			// 追加
			particles.push_back(particle);
		}

	}
}

void ParticleSpawnPolygonVertexModule::NoneEmit(std::list<CPUParticle::ParticleData>& particles) {

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
			particle.velocity = velocity;
			particle.transform.translation = currentVertices[cIndex];

			// 追加
			particles.push_back(particle);
		}
	}
}

void ParticleSpawnPolygonVertexModule::ImGui() {

	ImGui::Checkbox("notMoveEmit", &notMoveEmit_);
	ImGui::Checkbox("isInterpolate", &isInterpolate_);

	ImGui::DragFloat3("rotation", &emitterRotation_.x, 0.01f);
	ImGui::DragFloat("scale", &scale_, 0.05f);
	ImGui::DragFloat3("translation", &translation_.x, 0.05f);

	ImGui::DragInt("vertexCount", &vertexCount_, 1, 1, 12);
	emitPerVertex_.EditDragValue("emitPerVertex");

	if (!isInterpolate_) {
		return;
	}

	interpolateSpacing_.EditDragValue("spacing");
}

void ParticleSpawnPolygonVertexModule::DrawEmitter() {

	LineRenderer* lineRenderer = LineRenderer::GetInstance();
	Vector3 parentTranslation{};
	// 親の座標
	if (parentTransform_) {

		parentTranslation = parentTransform_->matrix.world.GetTranslationValue();
	}

	if (3 <= vertexCount_) {

		// 多角形の場合
		lineRenderer->DrawPolygon(vertexCount_,
			parentTranslation + translation_, scale_, emitterRotation_, emitterLineColor_);
	} else if (vertexCount_ == 2) {

		// 2頂点の場合
		const auto vertices = CalcVertices();
		lineRenderer->DrawLine3D(
			parentTranslation + vertices[0],
			parentTranslation + vertices[1], emitterLineColor_);
	} else {

		// 1頂点の場合
		const auto vertices = CalcVertices();
		lineRenderer->DrawSphere(4, 0.08f * scale_,
			parentTranslation + vertices[0], emitterLineColor_);
	}
}

Json ParticleSpawnPolygonVertexModule::ToJson() {

	Json data;

	// 共通設定
	ICPUParticleSpawnModule::ToCommonJson(data);

	data["isInterpolate"] = isInterpolate_;
	data["notMoveEmit"] = notMoveEmit_;
	data["vertexCount"] = vertexCount_;
	data["scale"] = scale_;
	data["emitterRotation"] = emitterRotation_.ToJson();
	data["translation"] = translation_.ToJson();

	emitPerVertex_.SaveJson(data, "emitPerVertex");
	interpolateSpacing_.SaveJson(data, "interpolateSpacing");

	return data;
}

void ParticleSpawnPolygonVertexModule::FromJson(const Json& data) {

	// 共通設定
	ICPUParticleSpawnModule::FromCommonJson(data);

	isInterpolate_ = data.value("isInterpolate", false);
	notMoveEmit_ = data.value("notMoveEmit", false);
	vertexCount_ = data.value("vertexCount", 3);
	scale_ = data.value("scale", 1.0f);

	emitterRotation_.FromJson(data["emitterRotation"]);
	translation_.FromJson(data["translation"]);

	emitPerVertex_.ApplyJson(data, "emitPerVertex");
	interpolateSpacing_.ApplyJson(data, "interpolateSpacing");
}
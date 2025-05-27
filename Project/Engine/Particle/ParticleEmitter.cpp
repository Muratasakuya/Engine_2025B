#include "ParticleEmitter.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Engine/Asset/AssetEditor.h>
#include <Engine/Core/Graphics/Mesh/MeshletBuilder.h>
#include <Engine/Renderer/LineRenderer.h>
#include <Engine/Particle/Creators/ParticleCreator.h>
#include <Engine/Particle/Updater/ParticleUpdater.h>
#include <Game/Time/GameTimer.h>

//============================================================================
//	ParticleEmitter classMethods
//============================================================================

void ParticleEmitter::Init(const std::string& name,
	Asset* asset, ID3D12Device* device) {

	asset_ = nullptr;
	asset_ = asset;

	device_ = nullptr;
	device_ = device;

	// 名前を設定
	name_ = name;

	// 初期化処理
	transform_.Init();

	// layout
	leftChildSize_ = ImVec2(320.0f, 148.0f);
	rightChildSize_ = ImVec2(380.0f, 148.0f);
	addButtonSize_ = ImVec2(208.0f, 30.0f);
	dropSize_ = ImVec2(208.0f, 30.0f);
}

void ParticleEmitter::Update(const Matrix4x4& billboardMatrix) {

	// 所持しているparticleのemitterの描画
	if (particleGroups_.empty()) {
		return;
	}

	// 各particleのemitterを描画
	DrawParticleEmitters();

	// 各particleを更新する
	UpdateParticles(billboardMatrix);
}

void ParticleEmitter::EditLayout() {

	ImGui::Begin("ParticleEmitter_EditLayout");

	ImGui::DragFloat2("leftChildSize", &leftChildSize_.x, 0.5f);
	ImGui::DragFloat2("rightChildSize", &rightChildSize_.x, 0.5f);
	ImGui::DragFloat2("addButtonSize", &addButtonSize_.x, 0.5f);
	ImGui::DragFloat2("dropSize", &dropSize_.x, 0.5f);

	ImGui::End();
}

void ParticleEmitter::CreateParticle() {

	// particle追加
	particleGroups_.emplace_back();
	ParticleGroup& group = particleGroups_.back();
	// parameterを初期化
	group.parameter.Init(addParticleNameInputText_.name, addModelName_, asset_);

	// buffer作成
	// matrix
	group.worldMatrixBuffer.CreateStructuredBuffer(device_, kMaxInstanceNum_);
	// material
	group.materialBuffer.CreateStructuredBuffer(device_, kMaxInstanceNum_);

	// 頂点、meshlet生成
	const ResourceMesh resourceMesh = CreateMeshlet(addModelName_);
	// mesh作成
	group.mesh = std::make_unique<EffectMesh>();
	group.mesh->Init(device_, resourceMesh, kMaxInstanceNum_);

	// transferData
	group.transferMaterials.resize(kMaxInstanceNum_);
	group.transferMatrices.resize(kMaxInstanceNum_);

	// particleデータの作成
	ParticleCreator::Create(group.particles, group.parameter);
}

ResourceMesh<EffectMeshVertex> ParticleEmitter::CreateMeshlet(const std::string& modelName) {

	ModelData modelData = asset_->GetModelData(modelName);

	// 頂点、meshlet生成
	MeshletBuilder meshletBuilder{};
	ResourceMesh<EffectMeshVertex> resourceMesh = meshletBuilder.ParseEffectMesh(modelData);

	return resourceMesh;
}

void ParticleEmitter::DrawParticleEmitters() {

	// 各particleのemitterを形状ごとに描画する
	LineRenderer* lineRenderer = LineRenderer::GetInstance();
	const Color& emitterColor = Color::Red(0.6f);
	for (const auto& group : particleGroups_) {
		switch (group.parameter.emitterShape) {
		case EmitterShapeType::Sphere: {

			const uint32_t kDivision = 6;
			EmitterSphere emitterSphere = group.parameter.emitterSphere;
			lineRenderer->DrawSphere(kDivision, emitterSphere.radius, emitterSphere.center, emitterColor);
			break;
		}
		case EmitterShapeType::Hemisphere: {

			const uint32_t kDivision = 8;
			EmitterHemisphere emitterHemisphere = group.parameter.emitterHemisphere;
			lineRenderer->DrawHemisphere(kDivision, emitterHemisphere.radius,
				emitterHemisphere.center, emitterHemisphere.eulerRotate, emitterColor);
			break;
		}
		case EmitterShapeType::Box: {

			EmitterBox emitterBox = group.parameter.emitterBox;
			lineRenderer->DrawOBB(emitterBox.center, emitterBox.size, emitterBox.eulerRotate, emitterColor);
			break;
		}
		case EmitterShapeType::Cone: {

			const uint32_t kDivision = 12;
			EmitterCone emitterCone = group.parameter.emitterCone;
			lineRenderer->DrawCone(kDivision, emitterCone.baseRadius, emitterCone.topRadius,
				emitterCone.height, emitterCone.center, emitterCone.eulerRotate, emitterColor);
			break;
		}
		}
	}
}

void ParticleEmitter::UpdateFrequencyEmit(ParticleGroup& group) {

	// emitCountが0の時は処理しない
	if (group.parameter.emitCount.GetValue() == 0) {
		return;
	}

	float deltaTime = 0.0f;
	if (group.parameter.useScaledTime) {
		deltaTime = GameTimer::GetScaledDeltaTime();
	} else {
		deltaTime = GameTimer::GetDeltaTime();
	}
	// 経過時間を加算
	group.parameter.frequencyTime += deltaTime;
	// 経過時間が発射時間を超えたら
	if (group.parameter.frequency <= group.parameter.frequencyTime) {

		// 時間を元に戻す
		group.parameter.frequencyTime -= group.parameter.frequency;

		// 最大インスタンス数以下の時のみ
		if (group.numInstance < kMaxInstanceNum_) {
			// particleを作成
			ParticleCreator::Create(group.particles, group.parameter);
		}
	}
}

void ParticleEmitter::UpdateParticles(const Matrix4x4& billboardMatrix) {

	// 所持しているparticleをすべて更新する
	for (auto& group : particleGroups_) {

#ifdef _DEBUG
		// emit処理
		UpdateFrequencyEmit(group);
#endif // _DEBUG

		auto& particles = group.particles;
		// 各particleの更新
		uint32_t index = 0;
		for (auto it = particles.begin(); it != particles.end();) {

			// 寿命が無くなったparticleは削除する
			if (it->parameter.lifeTime.value < it->currentTime) {
				it = particles.erase(it);
				continue;
			}

			// particleを更新する
			ParticleUpdater::Update(*it, group.parameter, billboardMatrix);

			// bufferに渡すデータを更新
			// matrix
			group.transferMatrices[index] = it->transform.matrix.world;
			// material
			group.transferMaterials[index].SetMaterial(it->material);

			// イテレータをインクリメント
			++it;
			++index;
		}

		// instance数を更新
		group.numInstance = static_cast<uint32_t>(group.particles.size());
		// bufferを更新
		group.materialBuffer.TransferVectorData(group.transferMaterials, group.numInstance);
		group.worldMatrixBuffer.TransferVectorData(group.transferMatrices, group.numInstance);
	}
}

void ParticleEmitter::ImGui() {

	EditLayout();

	// layout
	ImGui::BeginGroup();

	// emitterの名前を表示
	ImGui::SeparatorText(("EmitterName: " + name_).c_str());

	// particle追加処理
	ImGui::BeginChild("AddChild##ParticleEmitter", leftChildSize_, true);
	ImGui::SeparatorText("Add Particle");

	AddParticle();
	ImGui::EndChild();

	// 横並びにする
	ImGui::SameLine();

	// particle選択処理
	ImGui::BeginChild("SelectChild##ParticleEmitter", rightChildSize_, true);
	ImGui::SeparatorText("Select Particle");

	SelectParticle();
	ImGui::EndChild();

	// particleの値操作処理
	ImGui::BeginChild("EditChild##ParticleEmitter");
	ImGui::SeparatorText("Edit Particle");

	EditParticle();
	ImGui::EndChild();

	ImGui::EndGroup();
}

void ParticleEmitter::AddParticle() {

	ImGui::PushID("AddParticlerRow");

	if (ImGui::InputTextWithHint("##ParticleName", "ParticleName",
		addParticleNameInputText_.nameBuffer, IM_ARRAYSIZE(addParticleNameInputText_.nameBuffer))) {

		// 名前を保持
		addParticleNameInputText_.name = addParticleNameInputText_.nameBuffer;
	}

	// ドロップ処理
	if (addModelName_.empty()) {

		// modelをドラッグアンドドロップで取得
		ImGui::Button("Drop model", dropSize_);
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payloadDataId = ImGui::AcceptDragDropPayload(AssetEditor::kDragPayloadId)) {

				auto* payloadModelData = static_cast<AssetEditor::DragPayload*>(payloadDataId->Data);
				// model以外は受け付けない
				if (payloadModelData->type == AssetEditor::PendingType::Model) {

					// modelの名前を保存
					addModelName_ = payloadModelData->name;
				}
			}
			ImGui::EndDragDropTarget();
		}
	} else {

		if (ImGui::Button("Reset", ImVec2(addButtonSize_.x / 2.0f, addButtonSize_.y))) {

			// modelの名前をリセットする
			addModelName_.clear();
		}
		ImGui::SameLine();

		// model名を表示
		ImGui::Text((":" + addModelName_).c_str());
	}

	if (ImGui::Button("Add", addButtonSize_)) {

		// 入力がないときは追加不可
		if (!addParticleNameInputText_.name.empty() &&
			!addModelName_.empty()) {

			// particle作成
			CreateParticle();
			// 入力リセット
			addParticleNameInputText_.Reset();
			addModelName_.clear();
		}
	}

	ImGui::PopID();
}

void ParticleEmitter::SelectParticle() {

	if (particleGroups_.empty()) {
		ImGui::TextDisabled("Particle empty...");
		return;
	}

	for (int i = 0; i < static_cast<int>(particleGroups_.size()); ++i) {
		const bool selected = (currentSelectIndex_.has_value() && currentSelectIndex_.value() == i);
		if (ImGui::Selectable(particleGroups_[i].parameter.GetParticleName().c_str(), selected)) {

			currentSelectIndex_ = i;
		}
		if (selected) {

			ImGui::SetItemDefaultFocus();
		}
	}
}

void ParticleEmitter::EditParticle() {

	// 選択されていないときは何もしない
	if (!currentSelectIndex_.has_value()) {
		return;
	}

	// parameter操作
	ImGui::Text("numInstance: %d / %d",
		particleGroups_[currentSelectIndex_.value()].numInstance, kMaxInstanceNum_);
	ImGui::Text("particleSize: %d", particleGroups_[currentSelectIndex_.value()].particles.size());
	particleGroups_[currentSelectIndex_.value()].parameter.ImGui();
}

void ParticleEmitter::InputTextValue::Reset() {

	// 入力をリセット
	nameBuffer[0] = '\0';
	name.clear();
}

void EffectMaterial::Init() {

	color = Color::White();
	useNoiseTexture = false;
	useVertexColor = false;
	textureAlphaReference = 0.5f;
	noiseTextureAlphaReference = 0.0f;
	emissiveIntensity = 0.0f;
	emissionColor = Vector3::AnyInit(1.0f);
	uvTransform = Matrix4x4::MakeIdentity4x4();
}

void EffectMaterial::SetMaterial(const EffectMaterial& material) {

	color = material.color;
	textureIndex = material.textureIndex;
	noiseTextureIndex = material.noiseTextureIndex;
	useNoiseTexture = material.useNoiseTexture;
	useVertexColor = material.useVertexColor;
	textureAlphaReference = material.textureAlphaReference;
	noiseTextureAlphaReference = material.noiseTextureAlphaReference;
	emissiveIntensity = material.emissiveIntensity;
	emissionColor = material.emissionColor;
	uvTransform = material.uvTransform;
}
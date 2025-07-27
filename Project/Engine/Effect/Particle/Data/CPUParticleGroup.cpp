#include "CPUParticleGroup.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Effect/Particle/ParticleConfig.h>
#include <Engine/Utility/GameTimer.h>

//============================================================================
//	CPUParticleGroup classMethods
//============================================================================

void CPUParticleGroup::Create(ID3D12Device* device,
	Asset* asset, ParticlePrimitiveType primitiveType) {

	asset_ = nullptr;
	asset_ = asset;

	// 初期化値
	// 全ての形状を初期化しておく
	emitter_.Init();

	blendMode_ = kBlendModeAdd;

	// buffer作成
	BaseParticleGroup::CreatePrimitiveBuffer(device, primitiveType);
	// structuredBuffer(SAV)
	transformBuffer_.CreateSRVBuffer(device, kMaxParticles);
	materialBuffer_.CreateSRVBuffer(device, kMaxParticles);
	textureInfoBuffer_.CreateSRVBuffer(device, kMaxParticles);
}

void CPUParticleGroup::Update() {

	// フェーズの更新処理
	UpdatePhase();
}

void CPUParticleGroup::UpdatePhase() {

	const float deltaTime = GameTimer::GetDeltaTime();

	for (auto& phase : phases_) {

		// emitterの更新
		phase->UpdateEmitter();

		// 発生処理
		phase->Emit(particles_, deltaTime);

		// 全てのparticleに対して更新処理を行う
		uint32_t particleIndex = 0;

		// 転送データのリサイズ
		ResizeTransferData(static_cast<uint32_t>(particles_.size()));
		for (auto it = particles_.begin(); it != particles_.end();) {

			// 時間を進める
			it->currentTime += deltaTime;
			it->progress = it->currentTime / it->lifeTime;

			// 寿命のきれたparticleの削除
			if (it->lifeTime <= it->currentTime) {
				it = particles_.erase(it);
				continue;
			}

			// 更新処理
			phase->UpdateParticle(*it, deltaTime);

			// bufferに渡すデータの更新処理
			UpdateTransferData(particleIndex, *it);

			// indexを進める
			++it;
			++particleIndex;
		}
	}

	// instance数を更新
	numInstance_ = static_cast<uint32_t>(particles_.size());
	// buffer転送
	TransferBuffer();
}

void CPUParticleGroup::UpdateTransferData(uint32_t particleIndex,
	const CPUParticle::ParticleData& particle) {

	// transform
	transferTransforms_[particleIndex] = particle.transform;
	// material
	transferMaterials_[particleIndex] = particle.material;
	transferMaterials_[particleIndex].uvTransform = Matrix4x4::MakeIdentity4x4();
	// texture
	transferTextureInfos_[particleIndex] = particle.textureInfo;
	// primitive
	switch (primitiveBuffer_.type) {
	case ParticlePrimitiveType::Plane: {

		transferPrimitives_.plane[particleIndex] = particle.primitive.plane;
		break;
	}
	case ParticlePrimitiveType::Ring: {

		transferPrimitives_.ring[particleIndex] = particle.primitive.ring;
		break;
	}
	case ParticlePrimitiveType::Cylinder: {

		transferPrimitives_.cylinder[particleIndex] = particle.primitive.cylinder;
		break;
	}
	}
}

void CPUParticleGroup::TransferBuffer() {

	// transform
	transformBuffer_.TransferData(transferTransforms_);
	// material
	materialBuffer_.TransferData(transferMaterials_);
	// texture
	textureInfoBuffer_.TransferData(transferTextureInfos_);
	// primitive
	switch (primitiveBuffer_.type) {
	case ParticlePrimitiveType::Plane: {

		primitiveBuffer_.plane.TransferData(transferPrimitives_.plane);
		break;
	}
	case ParticlePrimitiveType::Ring: {

		primitiveBuffer_.ring.TransferData(transferPrimitives_.ring);
		break;
	}
	case ParticlePrimitiveType::Cylinder: {

		primitiveBuffer_.cylinder.TransferData(transferPrimitives_.cylinder);
		break;
	}
	}
}

void CPUParticleGroup::ResizeTransferData(uint32_t size) {

	// transform
	transferTransforms_.resize(size);
	// material
	transferMaterials_.resize(size);
	// textureInfo
	transferTextureInfos_.resize(size);
	// primitive
	switch (primitiveBuffer_.type) {
	case ParticlePrimitiveType::Plane: {

		transferPrimitives_.plane.resize(size);
		break;
	}
	case ParticlePrimitiveType::Ring: {

		transferPrimitives_.ring.resize(size);
		break;
	}
	case ParticlePrimitiveType::Cylinder: {

		transferPrimitives_.cylinder.resize(size);
		break;
	}
	}
}

void CPUParticleGroup::AddPhase() {

	// phase追加
	phases_.emplace_back(std::make_unique<ParticlePhase>());
	ParticlePhase* phase = phases_.back().get();

	phase->Init(asset_, primitiveBuffer_.type);
	phase->SetSpawner(ParticleSpawnModuleID::Sphere);
	
	selectedPhase_ = static_cast<int>(phases_.size() - 1);
}

void CPUParticleGroup::ImGui() {

	ImGui::Text("numInstance: %d / %d", numInstance_, kMaxParticles);

	ImGui::SeparatorText("Phases");

	// 追加ボタン
	if (ImGui::Button("Add Phase")) {

		AddPhase();
	}

	int eraseIndex = -1;
	for (size_t i = 0; i < phases_.size(); ++i) {

		ImGui::PushID(static_cast<int>(i));

		bool nodeOpen = ImGui::TreeNodeEx(
			("Phase " + std::to_string(i)).c_str(),
			ImGuiTreeNodeFlags_DefaultOpen |
			(selectedPhase_ == static_cast<int>(i) ? ImGuiTreeNodeFlags_Selected : 0));

		if (ImGui::IsItemClicked()) {

			selectedPhase_ = static_cast<int>(i);
		}

		ImGui::SameLine();
		if (ImGui::SmallButton("[X]")) {

			eraseIndex = static_cast<int>(i);
		}
		// 中身
		if (nodeOpen) {

			phases_[i]->ImGui();
			ImGui::TreePop();
		}
		ImGui::PopID();
	}

	// 削除処理
	if (eraseIndex != -1) {

		phases_.erase(phases_.begin() + eraseIndex);
		if (eraseIndex <= selectedPhase_) {

			--selectedPhase_;
		}
	}
}
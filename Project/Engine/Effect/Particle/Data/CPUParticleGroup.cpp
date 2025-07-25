#include "CPUParticleGroup.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Effect/Particle/ParticleConfig.h>
#include <Engine/Utility/GameTimer.h>

//============================================================================
//	CPUParticleGroup classMethods
//============================================================================

void CPUParticleGroup::Create(
	ID3D12Device* device, ParticlePrimitiveType primitiveType) {

	// 初期化値
	// 全ての形状を初期化しておく
	emitter_.Init();

	blendMode_ = kBlendModeAdd;
	// 今はとりあえず実装したいのでcircle
	textureName_ = "circle";

	// buffer作成
	BaseParticleGroup::CreatePrimitiveBuffer(device, primitiveType);
	// structuredBuffer(UAV)
	transformBuffer_.CreateUAVBuffer(device, kMaxParticles);
	materialBuffer_.CreateUAVBuffer(device, kMaxParticles);
	textureInfoBuffer_.CreateUAVBuffer(device, kMaxParticles);
}

void CPUParticleGroup::Update() {

	// フェーズの更新処理
	UpdatePhase();
}

void CPUParticleGroup::UpdatePhase() {

	const float deltaTime = GameTimer::GetDeltaTime();

	for (auto& phase : phases_) {

		// 発生処理
		phase.Emit(particles_);

		// 全てのparticleに対して更新処理を行う
		uint32_t particleIndex = 0;
		for (auto it = particles_.begin(); it != particles_.end();) {

			// 寿命のきれたparticleの削除

			// 更新処理
			phase.Update(*it, deltaTime);

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

void CPUParticleGroup::ImGui() {

}
#include "GPUParticleGroup.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Renderer/LineRenderer.h>
#include <Engine/Effect/Particle/ParticleConfig.h>
#include <Engine/Core/Debug/Assert.h>
#include <Engine/Utility/GameTimer.h>
#include <Lib/Adapter/EnumAdapter.h>

// imgui
#include <imgui.h>

//============================================================================
//	GPUParticleGroup classMethods
//============================================================================

void GPUParticleGroup::CreatePrimitiveBuffer(
	ID3D12Device* device, ParticlePrimitiveType primitiveType) {

	primitiveBuffer_.type = primitiveType;

	// 各形状で生成、この時点で転送してしまう
	switch (primitiveType) {
	case ParticlePrimitiveType::Plane: {

		PlaneForGPU plane{};
		plane.Init();

		std::vector<PlaneForGPU> planes(kMaxParticles, plane);
		primitiveBuffer_.plane.CreateSRVBuffer(device, kMaxParticles);
		primitiveBuffer_.plane.TransferData(planes);
		break;
	}
	case ParticlePrimitiveType::Ring: {

		RingForGPU ring{};
		ring.Init();

		std::vector<RingForGPU> rings(kMaxParticles, ring);
		primitiveBuffer_.ring.CreateSRVBuffer(device, kMaxParticles);
		primitiveBuffer_.ring.TransferData(rings);
		break;
	}
	case ParticlePrimitiveType::Cylinder: {

		CylinderForGPU cylinder{};
		cylinder.Init();

		std::vector<CylinderForGPU> cylinders(kMaxParticles, cylinder);
		primitiveBuffer_.cylinder.CreateSRVBuffer(device, kMaxParticles);
		primitiveBuffer_.cylinder.TransferData(cylinders);
		break;
	}
	}
}

void GPUParticleGroup::Create(ID3D12Device* device, ParticlePrimitiveType primitiveType) {

	// 初期化値
	emitter_.common.Init();
	emitter_.sphere.Init();

	frequency_ = 0.4f;
	frequencyTime_ = 0.0f;
	isInitialized_ = false;
	blendMode_ = kBlendModeAdd;
	// 今はとりあえず実装したいのでcircle
	textureName_ = "circle";

	// buffer作成
	CreatePrimitiveBuffer(device, primitiveType);
	// 球でデフォルトで作成
	emitterBuffer_.common.CreateBuffer(device);
	emitterBuffer_.sphere.CreateBuffer(device);
	// structuredBuffer(UAV)
	transformBuffer_.CreateUAVBuffer(device, kMaxParticles);
	materialBuffer_.CreateUAVBuffer(device, kMaxParticles);
	particleBuffer_.CreateUAVBuffer(device, kMaxParticles);
	// freeList
	freeListIndexBuffer_.CreateUAVBuffer(device, 1);
	freeListBuffer_.CreateUAVBuffer(device, kMaxParticles);
}

void GPUParticleGroup::Update() {

	// 現在使用中のエミッタを更新
	UpdateEmitter();
}

void GPUParticleGroup::UpdateEmitter() {

	// 時間を進める
	frequencyTime_ += GameTimer::GetDeltaTime();
	// 発生間隔を過ぎたら発生させる
	if (frequency_ <= frequencyTime_) {

		frequencyTime_ -= frequency_;
		emitter_.common.emit = true;
	} else {

		emitter_.common.emit = false;
	}
	// buffer転送
	emitterBuffer_.common.TransferData(emitter_.common);

	// 回転の更新
	switch (emitter_.shape) {
	case ParticleEmitterShape::Sphere: {

		// buffer転送
		emitterBuffer_.sphere.TransferData(emitter_.sphere);
		break;
	}
	case ParticleEmitterShape::Hemisphere: {

		emitter_.hemisphere.rotationMatrix = Matrix4x4::MakeRotateMatrix(emitterRotation_);

		// buffer転送
		emitterBuffer_.hemisphere.TransferData(emitter_.hemisphere);
		break;
	}
	case ParticleEmitterShape::Box: {

		emitter_.box.rotationMatrix = Matrix4x4::MakeRotateMatrix(emitterRotation_);

		// buffer転送
		emitterBuffer_.box.TransferData(emitter_.box);
		break;
	}
	case ParticleEmitterShape::Cone: {

		emitter_.cone.rotationMatrix = Matrix4x4::MakeRotateMatrix(emitterRotation_);

		// buffer転送
		emitterBuffer_.cone.TransferData(emitter_.cone);
		break;
	}
	}

	// emitterの描画
	DrawEmitter();
}

void GPUParticleGroup::ImGui(ID3D12Device* device) {

	EnumAdapter<BlendMode>::Combo("blendMode", &blendMode_);

	// 発生データ
	ImGui::DragInt("count", &emitter_.common.count, 1, 0, kMaxParticles);
	ImGui::DragFloat("lifeTime", &emitter_.common.lifeTime, 0.01f);
	ImGui::DragFloat("frequency", &frequency_, 0.01f);

	ImGui::DragFloat3("scale", &emitter_.common.scale.x, 0.01f);
	ImGui::ColorEdit4("color", &emitter_.common.color.r);

	// エミッタ
	SelectEmitter(device);
	EditEmitter();
}

D3D12_GPU_VIRTUAL_ADDRESS GPUParticleGroup::GetPrimitiveBufferAdress() const {

	switch (primitiveBuffer_.type) {
	case ParticlePrimitiveType::Plane: {

		return primitiveBuffer_.plane.GetResource()->GetGPUVirtualAddress();
	}
	case ParticlePrimitiveType::Ring: {

		return primitiveBuffer_.ring.GetResource()->GetGPUVirtualAddress();
	}
	case ParticlePrimitiveType::Cylinder: {

		return primitiveBuffer_.cylinder.GetResource()->GetGPUVirtualAddress();
	}
	case ParticlePrimitiveType::Count: {

		ASSERT(false, "ParticlePrimitiveType::Count is not buffer");
		return primitiveBuffer_.plane.GetResource()->GetGPUVirtualAddress();
	}
	}

	// フォロースルー
	ASSERT(false, "ParticlePrimitiveType::Count is not buffer");
	return primitiveBuffer_.plane.GetResource()->GetGPUVirtualAddress();
}

D3D12_GPU_VIRTUAL_ADDRESS GPUParticleGroup::GetEmitterShapeBufferAdress() const {

	switch (emitter_.shape) {
	case ParticleEmitterShape::Sphere: {

		return emitterBuffer_.sphere.GetResource()->GetGPUVirtualAddress();
	}
	case ParticleEmitterShape::Hemisphere: {

		return emitterBuffer_.hemisphere.GetResource()->GetGPUVirtualAddress();
	}
	case ParticleEmitterShape::Box: {

		return emitterBuffer_.box.GetResource()->GetGPUVirtualAddress();
	}
	case ParticleEmitterShape::Cone: {

		return emitterBuffer_.cone.GetResource()->GetGPUVirtualAddress();
	}
	case ParticleEmitterShape::Count: {

		ASSERT(false, "ParticleEmitterType::Count is not buffer");
		return emitterBuffer_.sphere.GetResource()->GetGPUVirtualAddress();
	}
	}

	// フォロースルー
	ASSERT(false, "ParticleEmitterType::Count is not buffer");
	return emitterBuffer_.sphere.GetResource()->GetGPUVirtualAddress();
}

void GPUParticleGroup::DrawEmitter() {

	LineRenderer* lineRenderer = LineRenderer::GetInstance();

	const uint32_t division = 4;
	const Color color = Color::Red(0.6f);

	// まだbufferが作成されていなければ作成する
	switch (emitter_.shape) {
	case ParticleEmitterShape::Sphere: {

		lineRenderer->DrawSphere(division, emitter_.sphere.radius,
			emitter_.sphere.translation, color);
		break;
	}
	case ParticleEmitterShape::Hemisphere: {

		lineRenderer->DrawHemisphere(division, emitter_.hemisphere.radius,
			emitter_.hemisphere.translation, emitter_.hemisphere.rotationMatrix, color);
		break;
	}
	case ParticleEmitterShape::Box: {

		lineRenderer->DrawOBB(emitter_.box.translation,
			emitter_.box.size, emitter_.box.rotationMatrix, color);
		break;
	}
	case ParticleEmitterShape::Cone: {

		lineRenderer->DrawCone(division, emitter_.cone.baseRadius, emitter_.cone.topRadius,
			emitter_.cone.height, emitter_.cone.translation, emitter_.cone.rotationMatrix, color);
		break;
	}
	}
}

void GPUParticleGroup::SelectEmitter(ID3D12Device* device) {

	if (EnumAdapter<ParticleEmitterShape>::Combo("emitterShape", &emitter_.shape)) {

		// まだbufferが作成されていなければ作成する
		switch (emitter_.shape) {
		case ParticleEmitterShape::Hemisphere: {
			if (!emitterBuffer_.hemisphere.IsCreatedResource()) {

				emitter_.hemisphere.Init();
				emitterBuffer_.hemisphere.CreateBuffer(device);
			}
			break;
		}
		case ParticleEmitterShape::Box: {
			if (!emitterBuffer_.box.IsCreatedResource()) {

				emitter_.box.Init();
				emitterBuffer_.box.CreateBuffer(device);
			}
			break;
		}
		case ParticleEmitterShape::Cone: {
			if (!emitterBuffer_.cone.IsCreatedResource()) {

				emitter_.cone.Init();
				emitterBuffer_.cone.CreateBuffer(device);
			}
			break;
		}
		}
	}
}

void GPUParticleGroup::EditEmitter() {

	switch (emitter_.shape) {
	case ParticleEmitterShape::Sphere: {

		ImGui::DragFloat("radius", &emitter_.sphere.radius, 0.01f);
		ImGui::DragFloat3("translation", &emitter_.sphere.translation.x, 0.1f);
		break;
	}
	case ParticleEmitterShape::Hemisphere: {

		ImGui::DragFloat("radius", &emitter_.hemisphere.radius, 0.01f);
		ImGui::DragFloat3("rotation", &emitterRotation_.x, 0.1f);
		ImGui::DragFloat3("translation", &emitter_.hemisphere.translation.x, 0.1f);
		break;
	}
	case ParticleEmitterShape::Box: {

		ImGui::DragFloat3("size", &emitter_.box.size.x, 0.1f);
		ImGui::DragFloat3("rotation", &emitterRotation_.x, 0.1f);
		ImGui::DragFloat3("translation", &emitter_.box.translation.x, 0.1f);
		break;
	}
	case ParticleEmitterShape::Cone: {

		ImGui::DragFloat("baseRadius", &emitter_.cone.baseRadius, 0.01f);
		ImGui::DragFloat("topRadius", &emitter_.cone.topRadius, 0.01f);
		ImGui::DragFloat("height", &emitter_.cone.height, 0.01f);
		ImGui::DragFloat3("rotation", &emitterRotation_.x, 0.1f);
		ImGui::DragFloat3("translation", &emitter_.cone.translation.x, 0.1f);
		break;
	}
	}
}
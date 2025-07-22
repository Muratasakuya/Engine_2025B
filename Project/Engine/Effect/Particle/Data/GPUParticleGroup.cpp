#include "GPUParticleGroup.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Effect/Particle/ParticleConfig.h>
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

	// buffer転送
	emitterBuffer_.common.TransferData(emitter_.common);
	emitterBuffer_.sphere.TransferData(emitter_.sphere);
}

void GPUParticleGroup::UpdateEmitter() {

	// 回転の更新
	switch (emitter_.shape) {
	case ParticleEmitterShape::Hemisphere: {

		emitter_.hemiSphere.rotationMatrix = Matrix4x4::MakeRotateMatrix(emitterRotation_);
		break;
	}
	case ParticleEmitterShape::Box: {

		emitter_.box.rotationMatrix = Matrix4x4::MakeRotateMatrix(emitterRotation_);
		break;
	}
	case ParticleEmitterShape::Cone: {

		emitter_.cone.rotationMatrix = Matrix4x4::MakeRotateMatrix(emitterRotation_);
		break;
	}
	}
}

void GPUParticleGroup::ImGui(ID3D12Device* device) {

	EnumAdapter<BlendMode>::Combo("blendMode", &blendMode_);

	// 発生データ
	ImGui::DragInt("count", &emitter_.common.count, 1, 0, kMaxParticles);
	ImGui::DragFloat("lifeTime", &emitter_.common.lifeTime, 0.01f);
	ImGui::DragFloat("frequency", &frequency_, 0.01f);

	ImGui::DragFloat3("scale", &emitter_.common.scale.x, 0.01f);
	ImGui::ColorEdit4("color", &emitter_.common.color.a);

	// エミッタ
	SelectEmitter(device);
	EditEmitter();
}

const D3D12_GPU_VIRTUAL_ADDRESS& GPUParticleGroup::GetPrimitiveBufferAdress() const {

	switch (primitiveBuffer_.type) {
	case ParticlePrimitiveType::Plane: {

		return primitiveBuffer_.plane.GetResource()->GetGPUVirtualAddress();
	}
	case ParticlePrimitiveType::Ring: {

		return primitiveBuffer_.plane.GetResource()->GetGPUVirtualAddress();
	}
	case ParticlePrimitiveType::Cylinder: {

		return primitiveBuffer_.plane.GetResource()->GetGPUVirtualAddress();
	}
	case ParticlePrimitiveType::Count: {

		assert(false, " ParticlePrimitiveType::Count is not buffer");
		return primitiveBuffer_.plane.GetResource()->GetGPUVirtualAddress();
	}
	}

	// フォロースルー
	assert(false);
	return primitiveBuffer_.plane.GetResource()->GetGPUVirtualAddress();
}

void GPUParticleGroup::SelectEmitter(ID3D12Device* device) {

	if (EnumAdapter<ParticleEmitterShape>::Combo("emitterShape", &emitter_.shape)) {

		// まだbufferが作成されていなければ作成する
		switch (emitter_.shape) {
		case ParticleEmitterShape::Hemisphere: {
			if (!emitterBuffer_.hemisphere.IsCreatedResource()) {

				emitter_.hemiSphere.Init();
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

		ImGui::DragFloat("radius", &emitter_.hemiSphere.radius, 0.01f);
		ImGui::DragFloat3("rotation", &emitterRotation_.x, 0.1f);
		ImGui::DragFloat3("translation", &emitter_.hemiSphere.translation.x, 0.1f);
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
#include "BaseParticleGroup.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Renderer/LineRenderer.h>
#include <Engine/Core/Debug/Assert.h>
#include <Engine/Effect/Particle/ParticleConfig.h>

// imgui
#include <imgui.h>

//============================================================================
//	BaseParticleGroup classMethods
//============================================================================

D3D12_GPU_VIRTUAL_ADDRESS BaseParticleGroup::GetPrimitiveBufferAdress() const {

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
	case ParticlePrimitiveType::Crescent: {

		return primitiveBuffer_.crescent.GetResource()->GetGPUVirtualAddress();
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

void BaseParticleGroup::CreatePrimitiveBuffer(
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
	case ParticlePrimitiveType::Crescent: {

		CrescentForGPU crescent{};
		crescent.Init();

		std::vector<CrescentForGPU> crescents(kMaxParticles, crescent);
		primitiveBuffer_.crescent.CreateSRVBuffer(device, kMaxParticles);
		primitiveBuffer_.crescent.TransferData(crescents);
		break;
	}
	}
}

void BaseParticleGroup::DrawEmitter() {

	LineRenderer* lineRenderer = LineRenderer::GetInstance();

	const uint32_t division = 8;
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

void BaseParticleGroup::EditEmitter() {

	switch (emitter_.shape) {
	case ParticleEmitterShape::Sphere: {

		ImGui::DragFloat("radius", &emitter_.sphere.radius, 0.01f);
		ImGui::DragFloat3("translation", &emitter_.sphere.translation.x, 0.05f);

		// 動いていない間も座標は共有する
		emitter_.hemisphere.translation = emitter_.sphere.translation;
		emitter_.box.translation = emitter_.sphere.translation;
		emitter_.cone.translation = emitter_.sphere.translation;
		break;
	}
	case ParticleEmitterShape::Hemisphere: {

		ImGui::DragFloat("radius", &emitter_.hemisphere.radius, 0.01f);
		ImGui::DragFloat3("rotation", &emitterRotation_.x, 0.01f);
		ImGui::DragFloat3("translation", &emitter_.hemisphere.translation.x, 0.05f);

		// 動いていない間も座標は共有する
		emitter_.sphere.translation = emitter_.hemisphere.translation;
		emitter_.box.translation = emitter_.hemisphere.translation;
		emitter_.cone.translation = emitter_.hemisphere.translation;
		break;
	}
	case ParticleEmitterShape::Box: {

		ImGui::DragFloat3("size", &emitter_.box.size.x, 0.1f);
		ImGui::DragFloat3("rotation", &emitterRotation_.x, 0.01f);
		ImGui::DragFloat3("translation", &emitter_.box.translation.x, 0.05f);

		// 動いていない間も座標は共有する
		emitter_.sphere.translation = emitter_.box.translation;
		emitter_.hemisphere.translation = emitter_.box.translation;
		emitter_.cone.translation = emitter_.box.translation;
		break;
	}
	case ParticleEmitterShape::Cone: {

		ImGui::DragFloat("baseRadius", &emitter_.cone.baseRadius, 0.01f);
		ImGui::DragFloat("topRadius", &emitter_.cone.topRadius, 0.01f);
		ImGui::DragFloat("height", &emitter_.cone.height, 0.01f);
		ImGui::DragFloat3("rotation", &emitterRotation_.x, 0.01f);
		ImGui::DragFloat3("translation", &emitter_.cone.translation.x, 0.05f);

		// 動いていない間も座標は共有する
		emitter_.sphere.translation = emitter_.cone.translation;
		emitter_.hemisphere.translation = emitter_.cone.translation;
		emitter_.box.translation = emitter_.cone.translation;
		break;
	}
	}
}
#include "ICPUParticleSpawnModule.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Lib/Adapter/EnumAdapter.h>

//============================================================================
//	ICPUParticleSpawnModule classMethods
//============================================================================

void ICPUParticleSpawnModule::InitCommonData() {

	// 初期化値
	// Emit
	emitCount_ = ParticleValue<uint32_t>::SetValue(4);
	lifeTime_ = ParticleValue<float>::SetValue(0.8f);

	// Material
	color_.start = ParticleValue<Color>::SetValue(Color::White());
	color_.target = ParticleValue<Color>::SetValue(Color::White(0.0f));

	// TextureInfo
	textureInfo_.samplerType = 0;
	textureInfo_.useNoiseTexture = false;
	// デフォルトのテクスチャで初期化
	textureInfo_.colorTextureIndex = asset_->GetTextureGPUIndex("circle");
	textureInfo_.noiseTextureIndex = asset_->GetTextureGPUIndex("noise");

	// Transform
	billboardType_ = ParticleBillboardType::All;
	scale_.start = ParticleValue<Vector3>::SetValue(Vector3::AnyInit(1.0f));
	scale_.target = ParticleValue<Vector3>::SetValue(Vector3::AnyInit(0.0f));
	moveSpeed_ = ParticleValue<float>::SetValue(1.6f);
}

void ICPUParticleSpawnModule::SetCommonData(CPUParticle::ParticleData& particle) {

	// 生存時間
	particle.lifeTime = lifeTime_.GetValue();

	// 色
	particle.color.start = color_.start.GetValue();
	particle.color.target = color_.target.GetValue();

	// テクスチャ情報
	particle.textureInfo = textureInfo_;

	// ビルボード
	particle.transform.billboardMode = static_cast<uint32_t>(billboardType_);
	// 拡縮
	particle.scale.start = scale_.start.GetValue();
	particle.scale.target = scale_.target.GetValue();

	// プリミティブ
	particle.primitive = primitive_;
}

void ICPUParticleSpawnModule::SetPrimitiveType(ParticlePrimitiveType type) {

	primitive_.type = type;
	// 形状で初期化
	switch (type) {
	case ParticlePrimitiveType::Plane:

		primitive_.plane.Init();
		break;
	case ParticlePrimitiveType::Ring:

		primitive_.ring.Init();
		break;
	case ParticlePrimitiveType::Cylinder:

		primitive_.cylinder.Init();
		break;
	}
}

void ICPUParticleSpawnModule::ImGuiCommon() {

	//============================================================================
	//	Emit
	//============================================================================

	emitCount_.EditDragValue("emitCount");
	lifeTime_.EditDragValue("lifeTime");

	//============================================================================
	//	Material
	//============================================================================

	color_.start.EditColor("startColor");
	color_.target.EditColor("targetColor");

	//============================================================================
	//	TextureInfo
	//============================================================================

	ImGui::DragInt("samplerType", &textureInfo_.samplerType, 1, 0, 1);
	ImGui::Text("samplerType: %s", EnumAdapter<
		ParticleCommon::SamplerType>::GetEnumName(textureInfo_.samplerType));
	ImGui::DragInt("useNoiseTexture", &textureInfo_.useNoiseTexture, 1, 0, 1);

	//============================================================================
	//	Transform
	//============================================================================

	EnumAdapter<ParticleBillboardType>::Combo("billboardType", &billboardType_);

	scale_.start.EditDragValue("startScale");
	scale_.target.EditDragValue("targetScale");

	moveSpeed_.EditDragValue("moveSpeed");

	//============================================================================
	//	Primitive
	//============================================================================

	switch (primitive_.type) {
	case ParticlePrimitiveType::Plane:

		ImGui::DragFloat2("size", &primitive_.plane.size.x, 0.01f);
		break;
	case ParticlePrimitiveType::Ring:

		ImGui::DragInt("outerRadius", &primitive_.ring.divide, 1, 3, 32);
		ImGui::DragFloat("outerRadius", &primitive_.ring.outerRadius, 0.01f);
		ImGui::DragFloat("innerRadius", &primitive_.ring.innerRadius, 0.01f);
		break;
	case ParticlePrimitiveType::Cylinder:

		ImGui::DragInt("outerRadius", &primitive_.cylinder.divide, 1, 3, 32);
		ImGui::DragFloat("topRadius", &primitive_.cylinder.topRadius, 0.01f);
		ImGui::DragFloat("bottomRadius", &primitive_.cylinder.bottomRadius, 0.01f);
		ImGui::DragFloat("height", &primitive_.cylinder.height, 0.01f);
		break;
	}
}
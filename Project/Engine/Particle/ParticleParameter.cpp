#include "ParticleParameter.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>

// imgui
#include <imgui.h>

//============================================================================
//	ParticleParameter classMethods
//============================================================================

void ParticleParameter::Init(std::string name,
	const std::string& modelName, Asset* asset) {

	asset_ = nullptr;
	asset_ = asset;

	// 名前を設定
	name_ = name;
	// modelを設定、途中変更不可
	modelName_ = modelName;
	// textureをdefaultで設定しておく
	textureName_ = "circle";
	noiseTextureName_ = "noise";
	// indexも取得
	textureIndex = asset_->GetTextureGPUIndex(textureName_);
	noiseTextureIndex = asset_->GetTextureGPUIndex(noiseTextureName_);

	// 初期定数値、ランダム値の設定
	// コメントは.h参照
	blendMode = BlendMode::kBlendModeAdd;
	emitterShape = EmitterShapeType::Sphere;
	billboardType = ParticleBillboardType::All;
	easingType = EasingType::EaseInSine;

	emitCount = ParticleValue<uint32_t>::SetValue(1);
	frequency = ParticleValue<float>::SetValue(0.08f);

	lifeTime = ParticleValue<float>::SetValue(1.6f);
	moveSpeed = ParticleValue<float>::SetValue(1.0f);

	startScale = ParticleValue<Vector3>::SetValue(Vector3::AnyInit(1.0f));
	targetScale = ParticleValue<Vector3>::SetValue(Vector3::AnyInit(1.0f));

	startRotationMultiplier = ParticleValue<Vector3>::SetValue(Vector3::AnyInit(0.0f));
	targetRotationMultiplier = ParticleValue<Vector3>::SetValue(Vector3::AnyInit(0.0f));

	startColor = ParticleValue<Color>::SetValue(Color::White(1.0f));
	targetColor = ParticleValue<Color>::SetValue(Color::White(0.0f));

	emissiveIntensity = ParticleValue<float>::SetValue(0.0f);
	emissionColor = ParticleValue<Vector3>::SetValue(Vector3::AnyInit(1.0f));

	isLoop = true;
	useScaledTime = false;
	moveToDirection = false;
	reflectGround = false;
}

void ParticleParameter::ImGui() {

	ImGui::PushItemWidth(itemWidth_);
	// 文字サイズを設定
	ImGui::SetWindowFontScale(0.8f);

	// 値の操作
	if (ImGui::BeginTabBar("ParticleParameter")) {

		// 描画関係の値操作
		if (ImGui::BeginTabItem("Render")) {

			EditRender();
			ImGui::EndTabItem();
		}
		// 生成処理の値操作
		if (ImGui::BeginTabItem("Emit")) {

			EditEmit();
			ImGui::EndTabItem();
		}
		// SRT関係の値操作
		if (ImGui::BeginTabItem("Transform")) {

			EditTransform();
			ImGui::EndTabItem();
		}
		// 移動処理の値操作
		if (ImGui::BeginTabItem("Move")) {

			EditMove();
			ImGui::EndTabItem();
		}
		// material関係の値操作
		if (ImGui::BeginTabItem("Material")) {

			EditMaterial();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::SetWindowFontScale(1.0f);
	ImGui::PopItemWidth();
}

void ParticleParameter::EditRender() {

	// 使用しているtextureの名前を表示
	// 貼るtexture
	ImGui::Text(("textureName:  " + textureName_).c_str());
	ImGui::Text("textureIndex: %d", textureIndex);

	// noiseTexture
	ImGui::Text(("noiseTextureName:  " + noiseTextureName_).c_str());
	ImGui::Text("noiseTextureIndex: %d", noiseTextureIndex);
	ImGui::Separator();

	// blendModeの選択
	Blend::SelectBlendMode(blendMode);
	// billboardTypeの選択
	ParticleBillboard::SelectBillboardType(billboardType);
}

void ParticleParameter::EditEmit() {

	// emitterの形状、値の操作
	EmitterShape::SelectEmitterShape(emitterShape);
	EmitterShape::EditEmitterShape(emitterShape, emitterSphere,
		emitterHemisphere, emitterBox, emitterCone);
	ImGui::Separator();
	// 発生
	ImGui::Checkbox("isLoop", &isLoop);
	ImGui::Separator();
	emitCount.EditDragValue("emitCount");
	ImGui::Separator();
	frequency.EditDragValue("frequency");
	ImGui::Separator();
	// 寿命
	lifeTime.EditDragValue("lifeTime");
}

void ParticleParameter::EditTransform() {

	// スケール
	startScale.EditDragValue("startScale");
	targetScale.EditDragValue("targetScale");
	ImGui::Separator();

	// 回転
	startRotationMultiplier.EditDragValue("startRotationMultiplier");
	targetRotationMultiplier.EditDragValue("targetRotationMultiplier");
}

void ParticleParameter::EditMove() {

	// 移動
	ImGui::Checkbox("useScaledTime", &useScaledTime);
	ImGui::Checkbox("moveToDirection", &moveToDirection);
	ImGui::Separator();
	// 速さ
	moveSpeed.EditDragValue("moveSpeed");
	ImGui::Separator();
	// イージング処理
	Easing::SelectEasingType(easingType);
}

void ParticleParameter::EditMaterial() {

	// flags
	ImGui::Checkbox("useVertexColor", &useVertexColor);
	ImGui::Checkbox("useNoiseTexture", &useNoiseTexture);
	ImGui::Separator();
	// 色
	startColor.EditColor("startColor");
	targetColor.EditColor("targetColor");
	ImGui::Separator();
	// 頂点色
	startVertexColor.EditColor("startVertexColor");
	targetVertexColor.EditColor("targetVertexColor");
	ImGui::Separator();
	// 発光
	emissiveIntensity.EditDragValue("emissiveIntensity");
	emissionColor.EditColor("emissionColor");
	ImGui::Separator();

	// alphaReference
	startTextureAlphaReference.EditDragValue("startTextureAlphaReference");
	targetTextureAlphaReference.EditDragValue("targetTextureAlphaReference");
	ImGui::Separator();
	startNoiseTextureAlphaReference.EditDragValue("startNoiseTextureAlphaReference");
	targetNoiseTextureAlphaReference.EditDragValue("targetNoiseTextureAlphaReference");
	ImGui::Separator();
}
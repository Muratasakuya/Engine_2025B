#include "ParticleParameter.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Engine/Asset/AssetEditor.h>
#include <Lib/Adapter/JsonAdapter.h>

// imgui
#include <imgui.h>

//============================================================================
//	ParticleParameter classMethods
//============================================================================

void ParticleParameter::Init(std::string name,
	const std::string& modelName, Asset* asset) {

	asset_ = nullptr;
	asset_ = asset;

	// editor内で使用
	useGame_ = false;

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
	uvAdressMode = UVAddressMode::WRAP;

	emitCount = ParticleValue<uint32_t>::SetValue(1);
	frequency = 1.0f;

	lifeTime = ParticleValue<float>::SetValue(0.8f);
	moveSpeed = ParticleValue<float>::SetValue(0.0f);

	startScale = ParticleValue<Vector3>::SetValue(Vector3::AnyInit(1.0f));
	targetScale = ParticleValue<Vector3>::SetValue(Vector3::AnyInit(1.0f));

	startRotationMultiplier = ParticleValue<Vector3>::SetValue(Vector3::AnyInit(0.0f));
	targetRotationMultiplier = ParticleValue<Vector3>::SetValue(Vector3::AnyInit(0.0f));

	startColor = ParticleValue<Color>::SetValue(Color::White(1.0f));
	targetColor = ParticleValue<Color>::SetValue(Color::White(1.0f));

	startEmissiveIntensity = ParticleValue<float>::SetValue(0.0f);
	targetEmissiveIntensity = ParticleValue<float>::SetValue(0.0f);
	startEmissionColor = ParticleValue<Vector3>::SetValue(Vector3::AnyInit(1.0f));
	targetEmissionColor = ParticleValue<Vector3>::SetValue(Vector3::AnyInit(1.0f));

	startEdgeSize = ParticleValue<float>::SetValue(0.8f);
	targetEdgeSize = ParticleValue<float>::SetValue(0.8f);
	startEdgeColor = ParticleValue<Color>::SetValue(Color::White(1.0f));
	targetEdgeColor = ParticleValue<Color>::SetValue(Color::White(1.0f));

	startEdgeEmissiveIntensity = ParticleValue<float>::SetValue(0.0f);
	targetEdgeEmissiveIntensity = ParticleValue<float>::SetValue(0.0f);
	startEdgeEmissionColor = ParticleValue<Vector3>::SetValue(Vector3::AnyInit(1.0f));
	targetEdgeEmissionColor = ParticleValue<Vector3>::SetValue(Vector3::AnyInit(1.0f));

	startUVScale = ParticleValue<Vector3>::SetValue(Vector3::AnyInit(1.0f));
	targetUVScale = ParticleValue<Vector3>::SetValue(Vector3::AnyInit(1.0f));
	startUVRotationZ = ParticleValue<float>::SetValue(0.0f);
	targetUVRotationZ = ParticleValue<float>::SetValue(0.0f);
	startUVTranslation = ParticleValue<Vector3>::SetValue(Vector3::AnyInit(0.0f));
	targetUVTranslation = ParticleValue<Vector3>::SetValue(Vector3::AnyInit(0.0f));

	reflectFace = Vector3(0.0f, 0.0f, 0.0f);
	restitution = ParticleValue<float>::SetValue(0.0f);
	gravityStrength = ParticleValue<float>::SetValue(0.0f);
	gravityDirection = ParticleValue<Vector3>::SetValue(Vector3(0.0f, -1.0f, 0.0f));

	isLoop = true;
	interpolateEmit = false;
	useScaledTime = false;
	moveToDirection = false;
	reflectGround = false;
	useUVTransform = false;
}

void ParticleParameter::Init(const Json& data, Asset* asset) {

	asset_ = nullptr;
	asset_ = asset;

	// game内で使用
	useGame_ = true;

	//============================================================================
	//	private値

	std::string key = "Private";

	name_ = data[key]["name_"];
	modelName_ = data[key]["modelName_"];
	textureName_ = data[key]["textureName_"];
	noiseTextureName_ = data[key]["noiseTextureName_"];

	// textureIndexを設定
	textureIndex = asset_->GetTextureGPUIndex(textureName_);
	noiseTextureIndex = asset_->GetTextureGPUIndex(noiseTextureName_);

	//============================================================================
	//	enum値

	key = "Enum";

	blendMode = data[key]["blendMode"];
	billboardType = data[key]["billboardType"];
	uvAdressMode = data[key]["uvAdressMode"];

	// easing
	moveEasingType = data[key]["moveEasingType"];
	scaleEasingType = data[key]["scaleEasingType"];
	rotationEasingType = data[key]["rotationEasingType"];
	colorEasingType = data[key]["colorEasingType"];
	emissionEasingType = data[key]["emissionEasingType"];
	alphaReferenceEasingType = data[key]["alphaReferenceEasingType"];
	edgeEasingType = data[key]["edgeEasingType"];
	edgeEmissionEasingType = data[key]["edgeEmissionEasingType"];
	uvScaleEasingType = data[key]["uvScaleEasingType"];
	uvRotationZEasingType = data[key]["uvRotationZEasingType"];
	uvTranslationEasingType = data[key]["uvTranslationEasingType"];

	//============================================================================
	//	flag値

	key = "Flag";

	isLoop = data[key]["isLoop"];
	interpolateEmit = data[key]["interpolateEmit"];
	useScaledTime = data[key]["useScaledTime"];
	moveToDirection = data[key]["moveToDirection"];
	reflectGround = data[key]["reflectGround"];
	useNoiseTexture = data[key]["useNoiseTexture"];
	useUVTransform = data[key]["useUVTransform"];

	//============================================================================
	//	emitter値

	key = "Emitter";

	emitterShape = data[key]["emitterShape"];
	EmitterShape::Aplly(data[key], emitterSphere, emitterHemisphere,
		emitterBox, emitterCone);

	//============================================================================
	//	各parameter値

	key = "Parameter";

	frequency = data[key]["frequency"];
	emitCount.ApplyJson(data[key], "emitCount");
	lifeTime.ApplyJson(data[key], "lifeTime");
	moveSpeed.ApplyJson(data[key], "moveSpeed");
	// scale
	startScale.ApplyJson(data[key], "startScale");
	targetScale.ApplyJson(data[key], "targetScale");
	// rotate
	startRotationMultiplier.ApplyJson(data[key], "startRotationMultiplier");
	targetRotationMultiplier.ApplyJson(data[key], "targetRotationMultiplier");
	// color
	startColor.ApplyJson(data[key], "startColor");
	targetColor.ApplyJson(data[key], "targetColor");
	// emission
	startEmissiveIntensity.ApplyJson(data[key], "startEmissiveIntensity");
	targetEmissiveIntensity.ApplyJson(data[key], "targetEmissiveIntensity");
	startEmissionColor.ApplyJson(data[key], "startEmissionColor");
	targetEmissionColor.ApplyJson(data[key], "targetEmissionColor");
	// alpha
	startTextureAlphaReference.ApplyJson(data[key], "startTextureAlphaReference");
	targetTextureAlphaReference.ApplyJson(data[key], "targetTextureAlphaReference");
	startNoiseTextureAlphaReference.ApplyJson(data[key], "startNoiseTextureAlphaReference");
	targetNoiseTextureAlphaReference.ApplyJson(data[key], "targetNoiseTextureAlphaReference");
	// noiseDiscard
	startEdgeSize.ApplyJson(data[key], "startEdgeSize");
	targetEdgeSize.ApplyJson(data[key], "targetEdgeSize");
	startEdgeColor.ApplyJson(data[key], "startEdgeColor");
	targetEdgeColor.ApplyJson(data[key], "targetEdgeColor");
	startEdgeEmissiveIntensity.ApplyJson(data[key], "startEdgeEmissiveIntensity");
	targetEdgeEmissiveIntensity.ApplyJson(data[key], "targetEdgeEmissiveIntensity");
	startEdgeEmissionColor.ApplyJson(data[key], "startEdgeEmissionColor");
	targetEdgeEmissionColor.ApplyJson(data[key], "targetEdgeEmissionColor");
	// uv
	startUVScale.ApplyJson(data[key], "startUVScale");
	targetUVScale.ApplyJson(data[key], "targetUVScale");
	startUVRotationZ.ApplyJson(data[key], "startUVRotationZ");
	targetUVRotationZ.ApplyJson(data[key], "targetUVRotationZ");
	startUVTranslation.ApplyJson(data[key], "startUVTranslation");
	targetUVTranslation.ApplyJson(data[key], "targetUVTranslation");
	// physics
	reflectFace = JsonAdapter::ToObject<Vector3>(data[key]["reflectFace"]);
	restitution.ApplyJson(data[key], "restitution");
	gravityStrength.ApplyJson(data[key], "gravityStrength");
	gravityDirection.ApplyJson(data[key], "gravityDirection");
}

void ParticleParameter::SaveJson(const std::string& saveName) {

	Json data;

	std::string key = "";

	/// メモ
	// textureIndexは毎回同じじゃないのでここで保存はしない

	//============================================================================
	//	private値の保存

	data["FileType"] = "Particle";

	key = "Private";

	data[key]["name_"] = name_;
	data[key]["modelName_"] = modelName_;
	data[key]["textureName_"] = textureName_;
	data[key]["noiseTextureName_"] = noiseTextureName_;

	//============================================================================
	//	enum値の保存

	key = "Enum";

	data[key]["blendMode"] = static_cast<int>(blendMode);
	data[key]["billboardType"] = static_cast<int>(billboardType);
	data[key]["uvAdressMode"] = static_cast<int>(uvAdressMode);

	// easing
	data[key]["moveEasingType"] = static_cast<int>(moveEasingType);
	data[key]["scaleEasingType"] = static_cast<int>(scaleEasingType);
	data[key]["rotationEasingType"] = static_cast<int>(rotationEasingType);
	data[key]["colorEasingType"] = static_cast<int>(colorEasingType);
	data[key]["emissionEasingType"] = static_cast<int>(emissionEasingType);
	data[key]["alphaReferenceEasingType"] = static_cast<int>(alphaReferenceEasingType);
	data[key]["edgeEasingType"] = static_cast<int>(edgeEasingType);
	data[key]["edgeEmissionEasingType"] = static_cast<int>(edgeEmissionEasingType);
	data[key]["uvScaleEasingType"] = static_cast<int>(uvScaleEasingType);
	data[key]["uvRotationZEasingType"] = static_cast<int>(uvRotationZEasingType);
	data[key]["uvTranslationEasingType"] = static_cast<int>(uvTranslationEasingType);

	//============================================================================
	//	flag値の保存

	key = "Flag";

	data[key]["isLoop"] = isLoop;
	data[key]["interpolateEmit"] = interpolateEmit;
	data[key]["useScaledTime"] = useScaledTime;
	data[key]["moveToDirection"] = moveToDirection;
	data[key]["reflectGround"] = reflectGround;
	data[key]["useNoiseTexture"] = useNoiseTexture;
	data[key]["useUVTransform"] = useUVTransform;

	//============================================================================
	//	emitter値の保存

	key = "Emitter";

	data[key]["emitterShape"] = static_cast<int>(emitterShape);
	EmitterShape::Save(data[key], emitterSphere, emitterHemisphere,
		emitterBox, emitterCone);

	//============================================================================
	//	各parameter値の保存

	key = "Parameter";

	data[key]["frequency"] = frequency;
	lifeTime.SaveJson(data[key], "lifeTime");
	emitCount.SaveJson(data[key], "emitCount");
	moveSpeed.SaveJson(data[key], "moveSpeed");
	// scale
	startScale.SaveJson(data[key], "startScale");
	targetScale.SaveJson(data[key], "targetScale");
	// rotate
	startRotationMultiplier.SaveJson(data[key], "startRotationMultiplier");
	targetRotationMultiplier.SaveJson(data[key], "targetRotationMultiplier");
	// color
	startColor.SaveJson(data[key], "startColor");
	targetColor.SaveJson(data[key], "targetColor");
	// emission
	startEmissiveIntensity.SaveJson(data[key], "startEmissiveIntensity");
	targetEmissiveIntensity.SaveJson(data[key], "targetEmissiveIntensity");
	startEmissionColor.SaveJson(data[key], "startEmissionColor");
	targetEmissionColor.SaveJson(data[key], "targetEmissionColor");
	// alpha
	startTextureAlphaReference.SaveJson(data[key], "startTextureAlphaReference");
	targetTextureAlphaReference.SaveJson(data[key], "targetTextureAlphaReference");
	startNoiseTextureAlphaReference.SaveJson(data[key], "startNoiseTextureAlphaReference");
	targetNoiseTextureAlphaReference.SaveJson(data[key], "targetNoiseTextureAlphaReference");
	// noiseDiscard
	startEdgeSize.SaveJson(data[key], "startEdgeSize");
	targetEdgeSize.SaveJson(data[key], "targetEdgeSize");
	startEdgeColor.SaveJson(data[key], "startEdgeColor");
	targetEdgeColor.SaveJson(data[key], "targetEdgeColor");
	startEdgeEmissiveIntensity.SaveJson(data[key], "startEdgeEmissiveIntensity");
	targetEdgeEmissiveIntensity.SaveJson(data[key], "targetEdgeEmissiveIntensity");
	startEdgeEmissionColor.SaveJson(data[key], "startEdgeEmissionColor");
	targetEdgeEmissionColor.SaveJson(data[key], "targetEdgeEmissionColor");
	// uv
	startUVScale.SaveJson(data[key], "startUVScale");
	targetUVScale.SaveJson(data[key], "targetUVScale");
	startUVRotationZ.SaveJson(data[key], "startUVRotationZ");
	targetUVRotationZ.SaveJson(data[key], "targetUVRotationZ");
	startUVTranslation.SaveJson(data[key], "startUVTranslation");
	targetUVTranslation.SaveJson(data[key], "targetUVTranslation");
	// physics
	data[key]["reflectFace"] = JsonAdapter::FromObject<Vector3>(reflectFace);
	restitution.SaveJson(data[key], "restitution");
	gravityStrength.SaveJson(data[key], "gravityStrength");
	gravityDirection.SaveJson(data[key], "gravityDirection");

	JsonAdapter::Save(saveName, data);
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
		// physics関係の値操作
		if (ImGui::BeginTabItem("Physics")) {

			EditPhysics();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::SetWindowFontScale(1.0f);
	ImGui::PopItemWidth();
}

void ParticleParameter::ImageButtonWithLabel(const char* id,
	const std::string& label, ImTextureID textureId, const ImVec2& size) {

	ImGui::PushID(id);
	ImGui::BeginGroup();

	// テキストサイズ計算
	ImVec2 textSize = ImGui::CalcTextSize(label.c_str());

	// 現在の位置取得
	ImVec2 pos = ImGui::GetCursorScreenPos();

	// 上にテキストの高さ分スペースを確保
	float labelSpacing = 2.0f;
	ImGui::Dummy(ImVec2(size.x, textSize.y + labelSpacing));

	// 画像ボタンの描画
	ImGui::SetCursorScreenPos(ImVec2(pos.x, pos.y + textSize.y + labelSpacing));
	ImGui::ImageButton(label.c_str(), textureId, size);

	// テキストをボタンの上に中央揃えで描画
	ImVec2 textPos = ImVec2(
		pos.x + (size.x - textSize.x) * 0.5f,
		pos.y);
	ImGui::GetWindowDrawList()->AddText(
		textPos,
		ImGui::GetColorU32(ImGuiCol_Text),
		label.c_str());

	ImGui::EndGroup();
	ImGui::PopID();
}

void ParticleParameter::EditRender() {

	// 表示サイズ
	const float imageSize = 88.0f;

	// 使用しているtextureの名前を表示
	// 貼るtexture
	ImageButtonWithLabel("texture", textureName_, (ImTextureID)asset_->GetGPUHandle(textureName_).ptr, { imageSize, imageSize });
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payloadDataId = ImGui::AcceptDragDropPayload(AssetEditor::kDragPayloadId)) {

			auto* payloadTextureData = static_cast<AssetEditor::DragPayload*>(payloadDataId->Data);
			// texture以外は受け付けない
			if (payloadTextureData->type == AssetEditor::PendingType::Texture) {

				// textureを設定
				textureName_ = payloadTextureData->name;
				// indexを設定
				textureIndex = asset_->GetTextureGPUIndex(textureName_);
			}
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::SameLine();

	// noiseTexture
	ImageButtonWithLabel("noiseTexture", noiseTextureName_, (ImTextureID)asset_->GetGPUHandle(noiseTextureName_).ptr, { imageSize, imageSize });
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payloadDataId = ImGui::AcceptDragDropPayload(AssetEditor::kDragPayloadId)) {

			auto* payloadTextureData = static_cast<AssetEditor::DragPayload*>(payloadDataId->Data);
			// texture以外は受け付けない
			if (payloadTextureData->type == AssetEditor::PendingType::Texture) {

				// textureを設定
				noiseTextureName_ = payloadTextureData->name;
				// indexを設定
				noiseTextureIndex = asset_->GetTextureGPUIndex(noiseTextureName_);
			}
		}
		ImGui::EndDragDropTarget();
	}

	// blendModeの選択
	Blend::SelectBlendMode(blendMode);
	// uvAdressModeの選択
	UVAddress::SelectUVAddressMode(uvAdressMode);
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
	ImGui::Checkbox("interpolateEmit", &interpolateEmit);
	ImGui::Separator();
	emitCount.EditDragValue("emitCount");
	ImGui::Separator();
	ImGui::DragFloat("frequency", &frequency, 0.01f);
	ImGui::Separator();
	// 寿命
	lifeTime.EditDragValue("lifeTime");
}

void ParticleParameter::EditTransform() {

	// スケール
	Easing::SelectEasingType(scaleEasingType, "scaleEasingType");
	startScale.EditDragValue("startScale");
	targetScale.EditDragValue("targetScale");
	ImGui::Separator();

	// 回転
	Easing::SelectEasingType(rotationEasingType, "rotationEasingType");
	startRotationMultiplier.EditDragValue("startRotationMultiplier");
	targetRotationMultiplier.EditDragValue("targetRotationMultiplier");
}

void ParticleParameter::EditMove() {

	// 移動
	ImGui::Checkbox("useScaledTime", &useScaledTime);
	ImGui::Checkbox("moveToDirection", &moveToDirection);
	ImGui::Separator();
	// 速さ
	Easing::SelectEasingType(moveEasingType, "moveEasingType");
	moveSpeed.EditDragValue("moveSpeed");
	ImGui::Separator();
}

void ParticleParameter::EditMaterial() {

	// flags
	ImGui::Checkbox("useNoiseTexture", &useNoiseTexture);
	ImGui::Checkbox("useUVTransform", &useUVTransform);
	ImGui::Separator();
	// 色
	Easing::SelectEasingType(colorEasingType, "colorEasingType");
	startColor.EditColor("startColor");
	targetColor.EditColor("targetColor");
	ImGui::Separator();
	// 発光
	Easing::SelectEasingType(emissionEasingType, "emissionEasingType");
	startEmissiveIntensity.EditDragValue("startEmissiveIntensity");
	targetEmissiveIntensity.EditDragValue("targetEmissiveIntensity");
	startEmissionColor.EditColor("startEmissionColor");
	targetEmissionColor.EditColor("targetEmissionColor");
	ImGui::Separator();

	// alphaReference
	Easing::SelectEasingType(alphaReferenceEasingType, "alphaReferenceEasingType");
	startTextureAlphaReference.EditDragValue("startTextureAlphaReference");
	targetTextureAlphaReference.EditDragValue("targetTextureAlphaReference");
	ImGui::Separator();
	startNoiseTextureAlphaReference.EditDragValue("startNoiseTextureAlphaReference");
	targetNoiseTextureAlphaReference.EditDragValue("targetNoiseTextureAlphaReference");
	ImGui::Separator();

	// noiseDiscard
	if (useNoiseTexture) {

		Easing::SelectEasingType(edgeEasingType, "edgeEasingType");
		startEdgeSize.EditDragValue("startEdgeSize");
		targetEdgeSize.EditDragValue("targetEdgeSize");
		ImGui::Separator();
		startEdgeColor.EditColor("startEdgeColor");
		targetEdgeColor.EditColor("targetEdgeColor");
		ImGui::Separator();

		Easing::SelectEasingType(edgeEmissionEasingType, "edgeEmissionEasingType");
		startEdgeEmissiveIntensity.EditDragValue("startEdgeEmissiveIntensity");
		targetEdgeEmissiveIntensity.EditDragValue("targetEdgeEmissiveIntensity");
		ImGui::Separator();
		startEdgeEmissionColor.EditColor("startEdgeEmissionColor");
		targetEdgeEmissionColor.EditColor("targetEdgeEmissionColor");
		ImGui::Separator();
	}

	// uv
	if (useUVTransform) {

		Easing::SelectEasingType(uvScaleEasingType, "uvScaleEasingType");
		startUVScale.EditDragValue("startUVScale");
		targetUVScale.EditDragValue("targetUVScale");
		ImGui::Separator();
		Easing::SelectEasingType(uvRotationZEasingType, "uvRotationZEasingType");
		startUVRotationZ.EditDragValue("startUVRotationZ");
		targetUVRotationZ.EditDragValue("targetUVRotationZ");
		ImGui::Separator();
		Easing::SelectEasingType(uvTranslationEasingType, "uvTranslationEasingType");
		startUVTranslation.EditDragValue("startUVTranslation");
		targetUVTranslation.EditDragValue("targetUVTranslation");
	}
}

void ParticleParameter::EditPhysics() {

	// flags
	ImGui::Checkbox("reflectGround", &reflectGround);
	// 反射
	ImGui::DragFloat3("reflectFace", &reflectFace.x, 0.01f);
	restitution.EditDragValue("restitution");
	gravityStrength.EditDragValue("gravityStrength");
	gravityDirection.EditDragValue("gravityDirection");
}
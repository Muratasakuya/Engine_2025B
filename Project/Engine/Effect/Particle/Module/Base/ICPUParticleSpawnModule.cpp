#include "ICPUParticleSpawnModule.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Engine/Asset/AssetEditor.h>
#include <Lib/Adapter/EnumAdapter.h>

//============================================================================
//	ICPUParticleSpawnModule classMethods
//============================================================================

void ICPUParticleSpawnModule::InitCommonData() {

	// 初期化値
	// Emit
	emitCount_ = ParticleValue<uint32_t>::SetValue(4);
	lifeTime_ = ParticleValue<float>::SetValue(0.8f);

	// TextureInfo
	textureInfo_.samplerType = 0;
	textureInfo_.useNoiseTexture = false;
	// デフォルトのテクスチャで初期化
	textureName_ = "circle";
	noiseTextureName_ = "noise";
	textureInfo_.colorTextureIndex = asset_->GetTextureGPUIndex(textureName_);
	textureInfo_.noiseTextureIndex = asset_->GetTextureGPUIndex(noiseTextureName_);

	// 移動速度
	moveSpeed_ = ParticleValue<float>::SetValue(1.6f);
}

void ICPUParticleSpawnModule::SetCommonData(CPUParticle::ParticleData& particle) {

	// 生存時間
	particle.lifeTime = lifeTime_.GetValue();

	// テクスチャ情報
	particle.textureInfo = textureInfo_;

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

void ICPUParticleSpawnModule::ShareCommonParam(ICPUParticleSpawnModule* other) {

	// Emit
	emitCount_ = other->emitCount_;

	// TextureInfo
	textureName_ = other->textureName_;
	noiseTextureName_ = other->noiseTextureName_;
	textureInfo_ = other->textureInfo_;

	// 移動速度
	moveSpeed_ = other->moveSpeed_;

	// Primitive
	primitive_ = other->primitive_;
}

void ICPUParticleSpawnModule::ImGuiRenderParam() {

	ImGui::SeparatorText("Texture");

	// ドラッグアンドドロップ処理
	DragAndDropTexture();

	ImGui::DragInt("useNoiseTexture", &textureInfo_.useNoiseTexture, 1, 0, 1);
	ImGui::SameLine();
	bool useNoiseTexture = textureInfo_.useNoiseTexture;
	ImGui::Text(std::format(": {}", useNoiseTexture).c_str());

	ImGui::DragInt("samplerType", &textureInfo_.samplerType, 1, 0, 1);
	ImGui::SameLine();
	ImGui::Text("    : %s", EnumAdapter<ParticleCommon::SamplerType>::GetEnumName(textureInfo_.samplerType));

	ImGui::SeparatorText("Primitive");

	switch (primitive_.type) {
	case ParticlePrimitiveType::Plane:

		ImGui::DragFloat2("size", &primitive_.plane.size.x, 0.01f);
		break;
	case ParticlePrimitiveType::Ring:

		ImGui::DragInt("divide", &primitive_.ring.divide, 1, 3, 32);
		ImGui::DragFloat("outerRadius", &primitive_.ring.outerRadius, 0.01f);
		ImGui::DragFloat("innerRadius", &primitive_.ring.innerRadius, 0.01f);
		break;
	case ParticlePrimitiveType::Cylinder:

		ImGui::DragInt("divide", &primitive_.cylinder.divide, 1, 3, 32);
		ImGui::DragFloat("topRadius", &primitive_.cylinder.topRadius, 0.01f);
		ImGui::DragFloat("bottomRadius", &primitive_.cylinder.bottomRadius, 0.01f);
		ImGui::DragFloat("height", &primitive_.cylinder.height, 0.01f);
		break;
	}
}

void ICPUParticleSpawnModule::ImGuiEmitParam() {

	emitCount_.EditDragValue("emitCount");
	lifeTime_.EditDragValue("lifeTime");

	moveSpeed_.EditDragValue("moveSpeed");
}

void ICPUParticleSpawnModule::DragAndDropTexture() {

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
				textureInfo_.colorTextureIndex = asset_->GetTextureGPUIndex(textureName_);
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
				textureInfo_.noiseTextureIndex = asset_->GetTextureGPUIndex(noiseTextureName_);
			}
		}
		ImGui::EndDragDropTarget();
	}
}

void ICPUParticleSpawnModule::ImageButtonWithLabel(const char* id,
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
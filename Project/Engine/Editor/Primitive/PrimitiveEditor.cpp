#include "PrimitiveEditor.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Engine/Core/Component/ComponentHelper.h>
#include <Lib/Adapter/JsonAdapter.h>

//============================================================================
//	PrimitiveEditor classMethods
//============================================================================

void PrimitiveEditor::Init(Asset* asset) {

	asset_ = nullptr;
	asset_ = asset;

	primitiveRegistry_ = std::make_unique<PrimitiveRegistry>();
	primitiveRegistry_->Init(asset);

	primitiveTool_ = std::make_unique<PrimitiveMeshTool>();

	// 初期化値
	editLayoutEnable_ = false;

	// json適応
	ApplyJson();
}

void PrimitiveEditor::ImGui() {

	// editorの関数群

	// editor内のlayoutを調整
	EditLayout();

	// 作成するprimitiveを選択して作成する
	primitiveRegistry_->RegistryPrimitiveMesh();

	// 作成されたprimitiveを選択
	primitiveRegistry_->SelectPrimiviveMesh();

	// obj出力
	OutputPrimitive();

	// 選択されたprimitiveの操作
	primitiveTool_->EditPrimitiveMesh(primitiveRegistry_->GetSelectIndex());
}

void PrimitiveEditor::EditLayout() {

	ImGui::Checkbox("editLayout", &editLayoutEnable_);

	if (!editLayoutEnable_) {
		return;
	}

	ImGui::Begin("primitiveEditorLayout");

	if (ImGui::Button("Save File")) {

		SaveEditLayoutParameter();
	}

	primitiveRegistry_->EditLayout();

	ImGui::End();
}

void PrimitiveEditor::ApplyJson() {

	// editor内のlayoutParameterを設定
	ApplyEditLayoutParameter();
}

void PrimitiveEditor::ApplyEditLayoutParameter() {

	if (!JsonAdapter::LoadAssert(baseJsonPath_ + "editorLayoutParameter.json")) {
		return;
	}
	// 読み込み
	Json data = JsonAdapter::Load(baseJsonPath_ + "editorLayoutParameter.json");

	// 適応処理
	primitiveRegistry_->ApplyEditLayoutParameter(data);
}

void PrimitiveEditor::SaveEditLayoutParameter() {

	Json data;

	// dataに入れる
	primitiveRegistry_->SaveEditLayoutParameter(data);

	// 保存処理
	JsonAdapter::Save(baseJsonPath_ + "editorLayoutParameter.json", data);
}

void PrimitiveEditor::OutputPrimitive() {

	const auto& primitiveIndex = primitiveRegistry_->GetSelectIndex();
	if (!primitiveIndex.has_value()) {
		return;
	}
	
	ImGui::PushItemWidth(itemWidth_);

	// 保存するfile(.json)の名前
	inputExportObjText_.InputText("FileName##Save");

	ImGui::PopItemWidth();

	if (ImGui::Button("Output Primitive", ImVec2(224.0f, 32.0f))) {

		// 現在の状態のprimitveを.obj出力する
		PrimitiveMeshComponent* primitive = Component::GetComponent<PrimitiveMeshComponent>(*primitiveIndex);
		const std::string filePath = "Primitives/" + inputExportObjText_.name + ".obj";
		asset_->Export(*primitive->GetResourceMesh(), filePath);
		inputExportObjText_.Reset();
	}
}

void PrimitiveEditor::InputTextValue::InputText(const std::string& label) {

	// 名前の入力
	if (ImGui::InputText(label.c_str(),
		nameBuffer,
		IM_ARRAYSIZE(nameBuffer))) {

		name = nameBuffer;
	}
}

void PrimitiveEditor::InputTextValue::Reset() {

	// 入力をリセット
	nameBuffer[0] = '\0';
	name.clear();
}
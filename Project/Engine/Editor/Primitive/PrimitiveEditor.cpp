#include "PrimitiveEditor.h"

//============================================================================
//	include
//============================================================================
#include <Lib/Adapter/JsonAdapter.h>

//============================================================================
//	PrimitiveEditor classMethods
//============================================================================

void PrimitiveEditor::Init(Asset* asset) {

	primitiveRegistry_ = std::make_unique<PrimitiveRegistry>();
	primitiveRegistry_->Init(asset);

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

	// 選択されたprimitiveの操作
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
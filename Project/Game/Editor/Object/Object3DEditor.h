#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Editor/Base/IGameEditor.h>
#include <Lib/MathUtils/MathUtils.h>

// c++
#include <string>
#include <optional>
#include <unordered_map>
// front
class Asset;

//============================================================================
//	Object3DEditor class
//============================================================================
class Object3DEditor :
	public IGameEditor {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	Object3DEditor() : IGameEditor("object3DEditor") {};
	~Object3DEditor() = default;

	void Init(Asset* asset);

	void ImGui() override;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- structure ----------------------------------------------------

	struct InputTextValue {

		char nameBuffer[128]; // imgui入力用
		std::string name;     // 入力した文字を取得する用

		void InputText(const std::string& label);

		void Reset();
	};

	//--------- variables ----------------------------------------------------

	Asset* asset_;

	// json
	const std::string baseJsonPath_ = "Object3DEditor/";

	// editorParameter
	bool editLayoutEnable_;   // editor内のlayoutを調整するかどうか
	float addParameterWidth_; // 追加選択の横幅
	float upLayoutHeight_;    // 上のlayoutの高さ

	// addObject
	int addSelectedObjectIndex_;   // modelのindex
	std::string addModelName_;     // modelの名前
	InputTextValue addObjectName_; // objectの名前
	InputTextValue addGroupName_;  // groupに入れる名前

	// selecrObject
	std::unordered_map<std::string, int> selectObjects_; // 追加されたobject
	std::optional<int> selectObjectIndex_;               // 選択したobjectのindex
	int selectedMaterialIndex_;                          // materialのindex

	// editObject
	float itemWidth_; // 表示するparameterの操作objectの横幅

	//--------- functions ----------------------------------------------------

	// json適応、設定
	void ApplyJson();
	// editorLayout
	void SaveEditLayoutParameter();
	void ApplyEditLayoutParameter();

	// editor内のlayoutを調整
	void EditLayout();

	// objectの追加
	void AddObject();
	// objectの選択
	void SelectObject();
	// objectの操作
	void EditObject();
	// ↑の操作
	void EditTransform();
	void EditMaterial();
};
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

	//--------- accessor -----------------------------------------------------

	uint32_t GetObjectId(const std::string& name, const std::string& groupName) const;
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

	// save
	InputTextValue saveFileName_; // 保存するjsonの名前
	int saveSelectedGroupIndex_;  // groupのindex
	std::string saveGroupName_;   // groupの名前
	// load
	InputTextValue loadFileName_; // 読み込み用のjsonの名前
	std::vector<std::string> loadFileNames_; // 読みこまれたjsonの名前

	// selecrObject
	// 追加されたobject
	std::unordered_map<std::string, std::unordered_map<std::string, int>> groupedSelectObjects_;
	std::optional<int> selectObjectIndex_; // 選択したobjectのindex
	int selectedMaterialIndex_;            // materialのindex

	// editObject
	float itemWidth_; // 表示するparameterの操作objectの横幅

	// 名前の登録
	std::unordered_map<std::string, int> nameCounts_;

	//--------- functions ----------------------------------------------------

	// json適応、設定
	void ApplyJson();
	// editorLayout
	void SaveEditLayoutParameter();
	void ApplyEditLayoutParameter();
	// object
	void SaveObjectJson();
	void ApplyObjectJson(const std::string& fileName);
	void LoadJson();

	// editor内のlayoutを調整
	void EditLayout();

	// objectの追加
	void AddObject();
	// objectの保存
	void SaveObjects();
	// objectの選択
	void SelectObject();
	// objectの操作
	void EditObject();
	void RemoveObject();
	// ↑の操作
	void EditMesh();
	void EditTransform();
	void EditMaterial();

	// 名前が重複しないようにする
	std::string CheckName(const std::string& name);
	std::string SplitBaseNameAndNumber(const std::string& name, int& number);
};
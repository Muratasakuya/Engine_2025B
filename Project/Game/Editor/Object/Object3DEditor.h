#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Editor/Base/IGameEditor.h>

// c++
#include <string>
#include <optional>
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
	};

	//--------- variables ----------------------------------------------------

	Asset* asset_;

	// addObject
	int addSelectedObjectIndex_;   // modelのindex
	std::string addModelName_;     // modelの名前
	InputTextValue addObjectName_; // objectの名前
	InputTextValue addGroupName_;  // groupに入れる名前

	//--------- functions ----------------------------------------------------

	// objectの追加
	void AddObject();
};
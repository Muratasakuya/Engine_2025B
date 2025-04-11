#pragma once

//============================================================================
//	include
//============================================================================
#include <Lib/MathUtils/MathUtils.h>

// c++
#include <vector>
#include <string>
#include <unordered_map>

//============================================================================
//	PrimitiveRegistry class
//============================================================================
class PrimitiveRegistry {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PrimitiveRegistry() = default;
	~PrimitiveRegistry() = default;

	void Init();

	void EditLayout();

	// primitiveの登録
	void RegistryPrimitiveMesh();
	// primitiveの選択
	void SelectPrimiviveMesh();

	// json適応、設定
	// editor
	void ApplyEditLayoutParameter(const Json& data);
	void SaveEditLayoutParameter(Json& data);

	//--------- accessor -----------------------------------------------------

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

	// editorParameter
	float parameterWidth_; // 追加選択の横幅
	float upLayoutHeight_; // 上のlayoutの高さ

	// create
	int createSelectedPrimitiveIndex_; // 作成するprimitiveのindex
	std::string createPrimitiveName_;  // 作成するprimitiveの名前
	InputTextValue createObjectName_;  // 保存するときのobjectの名前

	// select
	std::unordered_map<std::string, int> selectPrimitives_; // 追加されたprimitive
	int testPrimitiveIndex_;                                // とりあえずのindex
	std::optional<int> selectPrimitiveIndex_;               // 選択したprimitiveのindex

	// 名前の登録
	std::unordered_map<std::string, int> nameCounts_;

	//--------- functions ----------------------------------------------------

	// 名前が重複しないようにする
	std::string CheckName(const std::string& name);
	std::string SplitBaseNameAndNumber(const std::string& name, int& number);
};
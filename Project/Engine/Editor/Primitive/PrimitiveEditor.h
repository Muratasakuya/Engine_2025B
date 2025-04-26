#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Editor/Base/IGameEditor.h>
#include <Engine/Editor/Primitive/PrimitiveRegistry.h>
#include <Engine/Editor/Primitive/PrimitiveMeshTool.h>

// front
class Asset;

//============================================================================
//	PrimitiveEditor class
//============================================================================
class PrimitiveEditor :
	public IGameEditor {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PrimitiveEditor() : IGameEditor("primitiveEditor") {};
	~PrimitiveEditor() = default;

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

	// json
	const std::string baseJsonPath_ = "primitiveEditor/";
	InputTextValue inputExportObjText_;

	Asset* asset_;

	// editor操作
	bool editLayoutEnable_;

	// primitiveの登録
	std::unique_ptr<PrimitiveRegistry> primitiveRegistry_;
	// primitiveの操作
	std::unique_ptr<PrimitiveMeshTool> primitiveTool_;

	//--------- functions ----------------------------------------------------

	// json適応、設定
	void ApplyJson();
	// editorLayout
	void ApplyEditLayoutParameter();
	void SaveEditLayoutParameter();

	// obj出力
	void OutputPrimitive();

	// editor内のlayoutを調整
	void EditLayout();
};
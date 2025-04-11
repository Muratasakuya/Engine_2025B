#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Editor/Base/IGameEditor.h>
#include <Engine/Editor/Primitive/PrimitiveRegistry.h>

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

	void Init();

	void ImGui() override;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// json
	const std::string baseJsonPath_ = "primitiveEditor/";

	// editor操作
	bool editLayoutEnable_;

	// primitiveの登録
	std::unique_ptr<PrimitiveRegistry> primitiveRegistry_;

	//--------- functions ----------------------------------------------------

	// json適応、設定
	void ApplyJson();
	// editorLayout
	void ApplyEditLayoutParameter();
	void SaveEditLayoutParameter();

	// editor内のlayoutを調整
	void EditLayout();
};
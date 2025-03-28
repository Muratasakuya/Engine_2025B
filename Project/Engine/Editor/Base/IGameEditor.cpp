#include "IGameEditor.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Editor/Manager/GameEditorManager.h>
#include <Lib/MathUtils/Algorithm.h>

//============================================================================
//	IGameEditor classMethods
//============================================================================

IGameEditor::IGameEditor(const std::string& name) {

	name_ = name;
	GameEditorManager::GetInstance()->AddEditor(this);
}

IGameEditor::~IGameEditor() {

	GameEditorManager::GetInstance()->RemoveEditor(this);
}
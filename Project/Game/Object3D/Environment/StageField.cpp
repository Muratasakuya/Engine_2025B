#include "StageField.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Component/ComponentHelper.h>

//============================================================================
//	StageField classMethods
//============================================================================

StageField::~StageField() {

	GameObjectHelper::RemoveObject3D(objectId_);
}

void StageField::Init() {

	// field作成
	objectId_ = GameObjectHelper::CreateObject3D("stageField", "stageField", "Environment");
}
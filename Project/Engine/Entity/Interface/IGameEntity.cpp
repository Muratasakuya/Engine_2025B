#include "IGameEntity.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/ECS/Core/ECSManager.h>

//============================================================================
//	IGameEntity classMethods
//============================================================================

IGameEntity::IGameEntity() {

	ecsManager_ = ECSManager::GetInstance();
}

IGameEntity::~IGameEntity() {

	// entity削除
	ecsManager_->Destroy(entityId_);
}
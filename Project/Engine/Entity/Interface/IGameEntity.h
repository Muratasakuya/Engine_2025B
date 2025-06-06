#pragma once

//============================================================================
//	include
//============================================================================

// components
#include <Engine/Core/ECS/Components/TransformComponent.h>
#include <Engine/Core/ECS/Components/MaterialComponent.h>

// c++
#include <string>
#include <optional>
#include <cstdint>
// front
class ECSManager;

//============================================================================
//	IGameEntity class
//============================================================================
class IGameEntity {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	IGameEntity();
	virtual ~IGameEntity();

	//--------- accessor -----------------------------------------------------

protected:
	//========================================================================
	//	protected Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	ECSManager* ecsManager_;

	// 自身のentityId
	uint32_t entityId_;
};
#pragma once

//============================================================================
//	include
//============================================================================

// components
#include <Engine/Core/ECS/Components/TransformComponent.h>
#include <Engine/Core/ECS/Components/MaterialComponent.h>
#include <Engine/Core/ECS/Components/TagComponent.h>

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

	virtual void DerivedImGui() = 0;

	//--------- accessor -----------------------------------------------------

	/*---------- getter ----------*/

	const TagComponent& GetTag() const { return *tag_; }
protected:
	//========================================================================
	//	protected Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	ECSManager* ecsManager_;

	// 自身のentityId
	uint32_t entityId_;
	// tag
	TagComponent* tag_;

	// imgui
	const float itemWidth_ = 224.0f;
};
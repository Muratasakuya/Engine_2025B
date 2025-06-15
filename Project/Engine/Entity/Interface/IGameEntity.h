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

	/*---------- setter ----------*/

	void SetIdentifier(const std::string& identifier) { identifier_ = identifier; }

	/*---------- getter ----------*/

	const TagComponent& GetTag() const { return *tag_; }

	const std::string& GetIdentifier() const { return identifier_; }
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

	// 序列関係なしの名前
	std::string identifier_;

	// imgui
	const float itemWidth_ = 224.0f;
};
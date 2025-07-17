#pragma once

//============================================================================
//	include
//============================================================================

// data
#include <Engine/Object/Data/Transform.h>
#include <Engine/Object/Data/Material.h>
#include <Engine/Object/Data/ObjectTag.h>

// c++
#include <string>
#include <optional>
#include <cstdint>
// front
class ObjectManager;

//============================================================================
//	IGameObject class
//============================================================================
class IGameObject {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	IGameObject();
	virtual ~IGameObject();

	virtual void DerivedInit() = 0;

	virtual void ImGui() = 0;
	virtual void DerivedImGui() = 0;

	//--------- accessor -----------------------------------------------------

	/*---------- setter ----------*/

	void SetIdentifier(const std::string& identifier) { identifier_ = identifier; }

	/*---------- getter ----------*/

	const ObjectTag& GetTag() const { return *tag_; }

	const std::string& GetIdentifier() const { return identifier_; }
protected:
	//========================================================================
	//	protected Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	ObjectManager* objectManager_;

	// 自身のobjectId
	uint32_t objectId_;
	// tag
	ObjectTag* tag_;

	// 序列関係なしの名前
	std::string identifier_;

	// imgui
	const float itemWidth_ = 224.0f;
};
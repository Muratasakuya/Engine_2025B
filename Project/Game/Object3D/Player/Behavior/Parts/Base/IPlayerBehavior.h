#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Utility/SimpleAnimation.h>
#include <Game/Object3D/Player/Behavior/PlayerBehaviorType.h>
#include <Lib/Adapter/JsonAdapter.h>

// c++
#include <memory>
// front
class BasePlayerParts;
class FollowCamera;

//============================================================================
//	IPlayerBehavior class
//============================================================================
class IPlayerBehavior {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	IPlayerBehavior() = default;
	virtual ~IPlayerBehavior() = default;

	// 処理実行
	virtual void Execute(BasePlayerParts* parts) = 0;
	// リセット
	virtual void Reset() = 0;

	// imgui
	virtual void ImGui() = 0;

	// json
	virtual void SaveJson(Json& data) = 0;
protected:
	//========================================================================
	//	protected Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	const float itemWidth_ = 224.0f;

	FollowCamera* followCamera_;

	//--------- functions ----------------------------------------------------

	void InputKey(Vector2& inputValue);
};
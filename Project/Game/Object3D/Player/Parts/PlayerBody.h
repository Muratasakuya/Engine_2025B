#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Object3D/Player/Parts/Base/BasePlayerParts.h>

// front
class FollowCamera;

//============================================================================
//	PlayerBody class
//============================================================================
class PlayerBody :
	public BasePlayerParts {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PlayerBody() = default;
	~PlayerBody() = default;

	void Init(FollowCamera* followCamera);

	void ImGui();

	// json
	void SaveJson();
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	FollowCamera* followCamera_;

	//--------- functions ----------------------------------------------------

	// json
	void ApplyJson();

	// init
	void InitBehaviors(const Json& data);
};
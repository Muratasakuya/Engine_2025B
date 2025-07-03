#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Camera/Follow/State/Interface/FollowCameraIState.h>

//============================================================================
//	FollowCameraReturnState class
//============================================================================
class FollowCameraReturnState :
	public FollowCameraIState {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	FollowCameraReturnState() = default;
	~FollowCameraReturnState() = default;

	void Enter() override;

	void Update(FollowCamera& followCamera)  override;

	void Exit()  override;

	// imgui
	void ImGui(const FollowCamera& followCamera) override;

	// json
	void ApplyJson(const Json& data) override;
	void SaveJson(Json& data)  override;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------



	//--------- functions ----------------------------------------------------

};
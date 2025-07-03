#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Camera/Follow/State/Interface/FollowCameraIState.h>

//============================================================================
//	FollowCameraStunAttackState class
//============================================================================
class FollowCameraStunAttackState :
	public FollowCameraIState {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	FollowCameraStunAttackState() = default;
	~FollowCameraStunAttackState() = default;

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
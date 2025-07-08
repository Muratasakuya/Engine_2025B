#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Camera/Follow/State/Interface/FollowCameraIState.h>
#include <Game/Camera/Follow/Input/FollowCameraInputMapper.h>
#include <Game/Camera/Follow/Structures/FollowCameraStructures.h>

// c++
#include <memory>
#include <optional>
#include <unordered_map>

//============================================================================
//	FollowCameraStateController class
//============================================================================
class FollowCameraStateController {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	FollowCameraStateController() = default;
	~FollowCameraStateController() = default;

	void Init(FollowCamera& owner);

	void Update(FollowCamera& owner);

	void ImGui(FollowCamera& owner);

	//--------- accessor -----------------------------------------------------

	void SetTarget(FollowCameraTargetType type, const Transform3DComponent& target);
	void SetState(FollowCameraState state) { requested_ = state; }
	void SetOverlayState(FollowCameraOverlayState state);

	FollowCameraState GetCurrentState() const { return current_; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// 入力
	std::unique_ptr<FollowCameraInputMapper> inputMapper_;

	std::unordered_map<FollowCameraState, std::unique_ptr<FollowCameraIState>> states_;
	std::unordered_map<FollowCameraOverlayState, std::unique_ptr<FollowCameraIState>> overlayStates_;

	FollowCameraState current_;                  // 現在の状態
	std::optional<FollowCameraState> requested_; // 次の状態

	// 上から被せる形で行う処理
	std::optional<FollowCameraOverlayState>  overlayState_;

	// editor
	int editingStateIndex_;
	int editingOverlayStateIndex_;

	//--------- functions ----------------------------------------------------

	// json
	void ApplyJson();
	void SaveJson();

	// helper
	void SetInputMapper();
	bool Request(FollowCameraState state);
	void ChangeState();
	void CheckExitOverlayState();
};
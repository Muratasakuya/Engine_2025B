#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Editor/Base/IGameEditor.h>

// scene
#include <Game/Scene/GameState/GameSceneState.h>
// cameras
#include <Game/Camera/Follow/FollowCamera.h>
#include <Game/Camera/BeginGame/BeginGameCamera.h>
// front
class Player;
class SceneView;

//============================================================================
//	CameraManager class
//============================================================================
class CameraManager :
	public IGameEditor {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	CameraManager() :IGameEditor("CameraManager") {};
	~CameraManager() = default;

	void Init(SceneView* sceneView);

	void Update(GameSceneState sceneState);

	void ImGui() override;

	//--------- accessor -----------------------------------------------------

	void SetTarget(const Player* Player);

	FollowCamera* GetFollowCamera() const { return followCamera_.get(); }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	SceneView* sceneView_;
	const Player* player_;
	GameSceneState preSceneState_;

	// 追従カメラ
	std::unique_ptr<FollowCamera> followCamera_;
	// ゲーム開始時のカメラ
	std::unique_ptr<BeginGameCamera> beginGameCamera_;

	//--------- functions ----------------------------------------------------

	// update
	void CheckSceneState(GameSceneState sceneState);
};
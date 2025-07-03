#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Editor/Base/IGameEditor.h>

// cameras
#include <Game/Camera/FollowCamera.h>
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

	void Update();

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

	// 追従カメラ
	std::unique_ptr<FollowCamera> followCamera_;
};
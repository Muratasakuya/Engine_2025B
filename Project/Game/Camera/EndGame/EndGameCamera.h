#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Scene/Camera/BaseCamera.h>
#include <Engine/Utility/StateTimer.h>

//============================================================================
//	EndGameCamera class
//============================================================================
class EndGameCamera :
	public BaseCamera {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	EndGameCamera() = default;
	~EndGameCamera() = default;

	void Init();

	void Update() override;

	void ImGui() override;

	//--------- accessor -----------------------------------------------------

	bool IsFinished() const;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- structure ----------------------------------------------------

	// 現在の状態
	enum class State {

		Update,
		Finished
	};

	//--------- variables ----------------------------------------------------

	// 現在の状態
	State currentState_;
	bool disableTransition_;

	// 時間管理
	StateTimer animationTimer_;

	// parameters... 一旦簡易アニメーションで作成する
	Vector3 startPos_;
	Vector3 targetPos_;

	//--------- functions ----------------------------------------------------

	// json
	void ApplyJson();
	void SaveJson();

	// update
	void UpdateAnimation();
};
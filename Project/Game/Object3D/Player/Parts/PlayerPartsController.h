#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Editor/Base/IGameEditor.h>

// 各parts
#include <Game/Object3D/Player/Parts/PlayerBody.h>
#include <Game/Object3D/Player/Parts/PlayerRightHand.h>
#include <Game/Object3D/Player/Parts/PlayerLeftHand.h>
#include <Game/Object3D/Player/Parts/PlayerSword.h>

//============================================================================
//	PlayerPartsController class
//============================================================================
class PlayerPartsController :
	public IGameEditor {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PlayerPartsController() :IGameEditor("PlayerPartsController") {};
	~PlayerPartsController() = default;

	void Init();

	void Update();

	void ImGui() override;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// 体、全てのpartsの親
	std::unique_ptr<PlayerBody> body_;

	// 手、体が親
	std::unique_ptr<PlayerRightHand> rightHand_; // 右
	std::unique_ptr<PlayerLeftHand> leftHand_;   // 左

	// 武器(剣)、右手が親
	std::unique_ptr<PlayerSword> sword_;

	// parameter
	BasePlayerParts::PartsParameter rightHandParam_;
	BasePlayerParts::PartsParameter leftHandParam_;
	BasePlayerParts::PartsParameter swordParam_;

	//--------- functions ----------------------------------------------------

	// json
	void ApplyJson();
	void SaveJson();

	// update
	void SetUpdateParam();

	// init
	void InitParts();
};
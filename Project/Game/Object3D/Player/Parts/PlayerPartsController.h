#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Editor/Base/IGameEditor.h>
#include <Game/Object3D/Player/Behavior/PlayerBehaviorType.h>

// 各parts
#include <Game/Object3D/Player/Parts/PlayerBody.h>
#include <Game/Object3D/Player/Parts/PlayerRightHand.h>
#include <Game/Object3D/Player/Parts/PlayerLeftHand.h>
#include <Game/Object3D/Player/Parts/PlayerRightSword.h>
#include <Game/Object3D/Player/Parts/PlayerLeftSword.h>

// c++
#include <unordered_set>

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

	void Init(FollowCamera* followCamera);

	void Update(const std::unordered_set<PlayerBehaviorType>& behaviors);

	void ImGui() override;

	//--------- accessor -----------------------------------------------------

	const Transform3DComponent& GetTransform() const { return body_->GetTransform(); }
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
	std::unique_ptr<PlayerRightSword> rightSword_; // 右
	std::unique_ptr<PlayerLeftSword> leftSword_;   // 左

	//--------- functions ----------------------------------------------------

	// json
	void ApplyJson();
	void SaveJson();

	// init
	void InitParts(FollowCamera* followCamera);

	// update
	void UpdateBehavior(const std::unordered_set<PlayerBehaviorType>& behaviors);

	// 現在のbehaviourに含まれているか
	bool CheckCurrentBehaviors(const std::unordered_set<PlayerBehaviorType>& currentBehaviors,
		const std::initializer_list<PlayerBehaviorType> behaviours);

	// 各partsの関数呼び出し
	template <typename T>
	void ForEachParts(T function);
};

//============================================================================
//	PlayerPartsController templateMethods
//============================================================================

template<typename T>
inline void PlayerPartsController::ForEachParts(T function) {

	// 関数の呼び出し
	function(body_.get());
	function(rightHand_.get());
	function(leftHand_.get());
	function(rightSword_.get());
	function(leftSword_.get());
}
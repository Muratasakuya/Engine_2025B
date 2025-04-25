#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Editor/Base/IGameEditor.h>
#include <Game/Object3D/Player/Behavior/PlayerBehaviorType.h>

// c++
#include <optional>
#include <unordered_set>
#include <initializer_list>
// front
class Input;

//============================================================================
//	PlayerBehaviorController class
//============================================================================
class PlayerBehaviorController :
	public IGameEditor {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PlayerBehaviorController() :IGameEditor("PlayerBehaviorController") {};
	~PlayerBehaviorController() = default;

	void Init();

	void Update();

	void ImGui() override;
	//--------- accessor -----------------------------------------------------

	const std::unordered_set<PlayerBehaviorType>& GetCurrentBehaviours() const;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	Input* input_;

	std::optional<PlayerBehaviorType> moveBehaviour_;              // 依頼移動behaviour
	std::unordered_set<PlayerBehaviorType> currentMoveBehaviours_; // 現在の移動behaviour

	//--------- functions ----------------------------------------------------

	// update
	void MoveDash();
	void BehaviourRequest();

	// 現在のbehaviourに含まれているか
	bool CheckCurrentBehaviors(const std::initializer_list<PlayerBehaviorType> behaviours);
};
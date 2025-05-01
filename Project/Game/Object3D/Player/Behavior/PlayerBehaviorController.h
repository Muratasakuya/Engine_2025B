#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Editor/Base/IGameEditor.h>
#include <Game/Object3D/Player/Behavior/PlayerBehaviorType.h>
#include <Lib/Adapter/JsonAdapter.h>

// c++
#include <optional>
#include <string>
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

	//--------- structure ----------------------------------------------------

	struct LimitTime {

		float elapsed;   // 現在の経過時間
		float limit;     // behaviourにかけられる上限時間
		bool isReserve;  // 行動予約
		bool isUpdating; // 経過時間を更新中かどうか
		bool isReached;  // 上限時間に達したかどうか

		void UpdateElapseTime(); // 時間を進める
		void Reset();            // リセット

		void ImGui(const std::string& label);

		void ApplyJson(const Json& data, const std::string& key);
		void SaveJson(Json& data, const std::string& key);
	};

	enum class MatchType {

		All, // 全部一致
		Any  // どれか一致
	};

	//--------- variables ----------------------------------------------------

	Input* input_;

	std::optional<PlayerBehaviorType> moveBehaviour_;              // 依頼移動behaviour
	std::unordered_set<PlayerBehaviorType> currentMoveBehaviours_; // 現在の移動behaviour

	std::unordered_map<PlayerBehaviorType, LimitTime> limits_; // behaviour管理時間

	// editor
	bool isAcceptedMode_; // behaviourを設定できないようにする

	//--------- functions ----------------------------------------------------

	// json
	void ApplyJson();
	void SaveJson();

	// update
	void UpdateMove();    // 移動系の行動を更新する
	void UpdateAttack();  // 攻撃系の行動を更新する
	void MoveWalk();      // 歩き
	void MoveDash();      // ダッシュ
	void FirstAttack();   // ダッシュ状態以外の状態から攻撃
	void SecondAttack();  // 1段目の攻撃からの攻撃
	void CheckWait();     // 待機

	// behaviourを設定する
	void BehaviourRequest();

	// 現在のbehaviourに含まれているか
	bool CheckCurrentBehaviors(const std::initializer_list<PlayerBehaviorType> behaviours, MatchType matchType);
};
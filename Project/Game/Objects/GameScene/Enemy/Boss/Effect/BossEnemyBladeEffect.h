#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Effect/Game/GameEffect.h>
#include <Engine/Object/Data/Transform.h>
#include <Engine/Utility/StateTimer.h>

// c++
#include <memory>

//============================================================================
//	BossEnemySingleBladeEffect class
//============================================================================
class BossEnemySingleBladeEffect {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	BossEnemySingleBladeEffect() = default;
	~BossEnemySingleBladeEffect() = default;

	void Init(const BaseTransform& transform, const std::string& typeName);

	void Update();

	// editor
	void ImGui();

	//--------- accessor -----------------------------------------------------

	// 発生させる
	void EmitEffect(const BaseTransform& transform, float scalingValue = 1.0f);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- stricture ----------------------------------------------------

	// 現在の状態
	enum class State {

		None,  // 何もない
		Emited // 発生中
	};

	// 発生
	struct Emit {

		Vector3 translation;
		std::unique_ptr<GameEffect> effect;
	};

	//--------- variables ----------------------------------------------------

	std::string fileName_;

	// 状態
	State currentState_;
	// 発生時間
	StateTimer emitTimer_;

	// エフェクト
	Emit slash_;    // 真ん中の刃
	Emit plane_;    // 光る部分
	Emit particle_; // 後ろのパーティクル
	float scalingValue_;

	//--------- functions ----------------------------------------------------

	// json
	void ApplyJson();
	void SaveJson();
};
#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Editor/Base/IGameEditor.h>
#include <Game/Object3D/Player/Effect/PlayerBurstHitEffect.h>
#include <Game/Object3D/Player/Effect/PlayerCircleHitEffect.h>
#include <Game/Object3D/Player/Behavior/PlayerBehaviorType.h>

// c++
#include <unordered_set>
#include <initializer_list>

//============================================================================
//	PlayerEffectController class
//============================================================================
class PlayerEffectController :
	public IGameEditor {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PlayerEffectController() :IGameEditor("PlayerEffectController") {};
	~PlayerEffectController() = default;

	void Init();

	void Update(const std::unordered_set<PlayerBehaviorType>& behaviors);

	void ImGui() override;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// 広がる円のeffect
	std::unique_ptr<PlayerCircleHitEffect> circleHitEffect_;
	// 爆ぜるようなeffect
	std::unique_ptr<PlayerBurstHitEffect> burstHitEffect_;

	//--------- functions ----------------------------------------------------

	// json
	void ApplyJson();
	void SaveJson();

	// update
	void UpdateBehavior(const std::unordered_set<PlayerBehaviorType>& behaviors);

	// 現在のbehaviourに含まれているか
	bool CheckCurrentBehaviors(const std::unordered_set<PlayerBehaviorType>& currentBehaviors,
		const std::initializer_list<PlayerBehaviorType> behaviours);
};
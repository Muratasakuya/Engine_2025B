#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Editor/Base/IGameEditor.h>
#include <Game/Object3D/Player/Behavior/PlayerBehaviorType.h>

// effect
#include <Game/Object3D/Player/Effect/PlayerLightningHitEffect.h>

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

	// 稲妻のようなeffect
	std::unique_ptr<PlayerLightningHitEffect> lightningHitEffect_;

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
#include "BossEnemyStructures.h"

//============================================================================
//	BossEnemyStructures classMethods
//============================================================================
#include <Engine/Utility/JsonAdapter.h>

//============================================================================
//	BossEnemyStructures classMethods
//============================================================================

void BossEnemyCombo::FromJson(const Json& data) {

	sequence.clear();
	for (const auto& state : data["sequence"]) {

		sequence.emplace_back(static_cast<BossEnemyState>(state.get<int>()));
	}
	allowRepeat = data.value("allowRepeat", true);
	teleportType = static_cast<BossEnemyTeleportType>(data.value("teleportType", 0));
}

void BossEnemyCombo::ToJson(Json& data) {

	data["sequence"].clear();
	for (auto state : sequence) {

		data["sequence"].push_back(static_cast<int>(state));
	}
	data["allowRepeat"] = allowRepeat;
	data["teleportType"] = static_cast<int>(teleportType);
}

void BossEnemyPhase::FromJson(const Json& data) {

	nextStateDuration = data.value("nextStateDuration", 1.0f);
	comboIndices = JsonAdapter::ToVector<int>(data["comboIndices"]);
	autoIdleAfterAttack = data.value("autoIdleAfterAttack", false);
}

void BossEnemyPhase::ToJson(Json& data) {

	data["nextStateDuration"] = nextStateDuration;
	data["comboIndices"] = JsonAdapter::FromVector<int>(comboIndices);
	data["autoIdleAfterAttack"] = autoIdleAfterAttack;
}

void BossEnemyStateTable::FromJson(const Json& data) {

	combos.clear();
	phases.clear();
	for (const auto& combo : data["combos"]) {

		BossEnemyCombo  enemyCombo;
		enemyCombo.FromJson(combo);
		combos.push_back(enemyCombo);
	}
	for (const auto& phase : data["phases"]) {

		BossEnemyPhase enemyPhase;
		enemyPhase.FromJson(phase);
		phases.push_back(enemyPhase);
	}
}

void BossEnemyStateTable::ToJson(Json& data) {

	data["combos"].clear();
	data["phases"].clear();
	for (auto& combo : combos) {

		Json comboData;
		combo.ToJson(comboData);
		data["combos"].push_back(comboData);
	}
	for (auto& phase : phases) {

		Json phaseData;
		phase.ToJson(phaseData);
		data["phases"].push_back(phaseData);
	}
}
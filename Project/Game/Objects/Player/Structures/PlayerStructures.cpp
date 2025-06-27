#include "PlayerStructures.h"

//============================================================================
//	include
//============================================================================
#include <Lib/Adapter/JsonAdapter.h>

//============================================================================
//	PlayerStructures Methods
//============================================================================

void PlayerStateCondition::FromJson(const Json& data) {

	coolTime = JsonAdapter::GetValue<float>(data, "coolTime");
	requireSkillPoint = JsonAdapter::GetValue<int>(data, "requireSkillPoint");
	chainInputTime = JsonAdapter::GetValue<float>(data, "chainInputTime");

	std::vector<int> states = JsonAdapter::ToVector<int>(data["allowedPreState"]);
	for (int state : states) {

		allowedPreState.emplace_back(static_cast<PlayerState>(state));
	}
}

void PlayerStateCondition::ToJson(Json& data) {

	data["coolTime"] = coolTime;
	data["requireSkillPoint"] = requireSkillPoint;
	data["chainInputTime"] = chainInputTime;

	std::vector<int> states;
	for (auto state : allowedPreState) {

		states.emplace_back(static_cast<int>(state));
	}
	data["allowedPreState"] = JsonAdapter::FromVector<int>(states);
}
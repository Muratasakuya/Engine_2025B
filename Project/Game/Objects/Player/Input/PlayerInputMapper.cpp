#include "PlayerInputMapper.h"

//============================================================================
//	PlayerInputMapper classMethods
//============================================================================

void PlayerInputMapper::AddDevice(std::unique_ptr<PlayerIInputDevice> device) {

	devices_.emplace_back(std::move(device));
}

float PlayerInputMapper::GetVector(PlayerAction action) const {

	float vector = 0.0f;
	// 各入力から値を入れる
	for (const auto& device : devices_) {

		vector += device->GetVector(action);
	}
	vector = std::clamp(vector, -1.0f, 1.0f);
	return vector;
}

bool PlayerInputMapper::IsPressed(PlayerAction button) const {

	// 入力結果を取得
	for (const auto& device : devices_) {
		if (device->IsPressed(button)) {
			
			return true;
		}
	}
	return false;
}

bool PlayerInputMapper::IsTriggered(PlayerAction button) const {

	// 入力結果を取得
	for (const auto& device : devices_) {
		if (device->IsTriggered(button)) {

			return true;
		}
	}
	return false;
}
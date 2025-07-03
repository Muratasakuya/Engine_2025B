#include "FollowCameraInputMapper.h"

//============================================================================
//	FollowCameraInputMapper classMethods
//============================================================================

void FollowCameraInputMapper::AddDevice(std::unique_ptr<FollowCameraIInputDevice> device) {

	devices_.emplace_back(std::move(device));
}

float FollowCameraInputMapper::GetVector(FollowCameraAction action) const {

	float vector = 0.0f;
	// 各入力から値を入れる
	for (const auto& device : devices_) {

		vector += device->GetVector(action);
	}
	vector = std::clamp(vector, -1.0f, 1.0f);
	return vector;
}

bool FollowCameraInputMapper::IsPressed(FollowCameraAction button) const {

	// 入力結果を取得
	for (const auto& device : devices_) {
		if (device->IsPressed(button)) {

			return true;
		}
	}
	return false;
}

bool FollowCameraInputMapper::IsTriggered(FollowCameraAction button) const {

	// 入力結果を取得
	for (const auto& device : devices_) {
		if (device->IsTriggered(button)) {

			return true;
		}
	}
	return false;
}

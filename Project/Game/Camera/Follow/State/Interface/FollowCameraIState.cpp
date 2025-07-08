#include "FollowCameraIState.h"

//============================================================================
//	FollowCameraIState classMethods
//============================================================================

void FollowCameraIState::SetTarget(FollowCameraTargetType type,
	const Transform3DComponent& target) {

	targets_[type] = &target;
}
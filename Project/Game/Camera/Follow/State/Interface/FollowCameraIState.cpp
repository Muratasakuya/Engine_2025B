#include "FollowCameraIState.h"

//============================================================================
//	FollowCameraIState classMethods
//============================================================================

void FollowCameraIState::SetTarget(FollowCameraTargetType type,
	const Transform3D& target) {

	targets_[type] = &target;
}
//============================================================================
//	include
//============================================================================

#include "ShadowRay.hlsli"

//============================================================================
//	AnyHit
//============================================================================

[shader("anyhit")]
void AnyHit(inout ShadowPayload payload, in BuiltInTriangleIntersectionAttributes attributes) {
	
	// è’ìÀÇµÇΩ
	payload.occluded = true;
	AcceptHitAndEndSearch();
}
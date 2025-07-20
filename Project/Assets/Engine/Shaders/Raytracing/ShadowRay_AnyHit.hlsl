//============================================================================
//	include
//============================================================================

#include "ShadowRay.hlsli"

//============================================================================
//	AnyHit
//============================================================================

[shader("anyhit")]
void AnyHit(inout ShadowPayload payload, in BuiltInTriangleIntersectionAttributes attributes) {
	
	// �Փ˂���
	payload.occluded = true;
	AcceptHitAndEndSearch();
}
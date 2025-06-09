//============================================================================
//	include
//============================================================================

#include "ShadowRay.hlsli"

//============================================================================
//	Closesthit
//============================================================================
[shader("closesthit")]
void ShadowClosestHit(inout ShadowPayload payload, in BuiltInTriangleIntersectionAttributes attributes) {
	
	// è’ìÀÇµÇΩ
	payload.isHit = 1;
}
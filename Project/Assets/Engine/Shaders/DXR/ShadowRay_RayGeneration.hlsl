//============================================================================
//	include
//============================================================================

#include "ShadowRay.hlsli"

//============================================================================
//	define
//============================================================================

#define RAY_TYPE_SHADOW 0
#define RAY_TYPE_COUNT  1

//============================================================================
//	RayReneration
//============================================================================
[shader("raygeneration")]
void RayGeneration() {

	uint2 index = DispatchRaysIndex().xy;
	uint2 dimensions = DispatchRaysDimensions().xy;
	
	RayDesc ray;
	
	// レイ開始位置を計算
	ray.Origin = cameraPos * 1.0e-3f;
	
	// レイ方向を計算
	float dist = 1.0e38f;
	ray.Direction = -lightDirection;

	// レイ区間
	ray.TMin = 1.0e-3f;
	ray.TMax = dist;

	ShadowPayload payload = (ShadowPayload) 0;
	
	// レイ追跡処理
	TraceRay(gSceneBVH, 0, 0xFF, RAY_TYPE_SHADOW, RAY_TYPE_COUNT, RAY_TYPE_SHADOW, ray, payload);
	
	gShadowMask[index] = payload.isHit ? 0.0f : 1.0f;
}
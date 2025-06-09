//============================================================================
//	include
//============================================================================

#include "ShadowRay.hlsli"

//============================================================================
//	CBuffer
//============================================================================

cbuffer Scene : register(b0) {
	
	// camera
	float3 cameraPos;
	float4x4 inverseViewPro;

	// light
	float3 lightDirection;
};

//============================================================================
//	RayReneration
//============================================================================
[shader("raygeneration")]
void RayGeneration() {

	uint2 index = DispatchRaysIndex().xy;
	uint2 dimensions = DispatchRaysDimensions().xy;
	
	//============================================================================
	//	1次レイ
	//============================================================================

	float2 ndc = (float2(index) + 0.5f) / float2(dimensions) * 2.0f - 1.0f;
	float4 posH = mul(float4(ndc, 0, 1), inverseViewPro);
	float3 direction = normalize(posH.xyz / posH.w - cameraPos);
	
	RayDesc ray;
	ray.Origin = cameraPos;
	ray.Direction = direction;
	ray.TMin = 0.0f;
	ray.TMax = 1e38f;
	
	RadiancePayload rPayload;
	rPayload.color = float3(0.0f, 0.0f, 0.0f);
	rPayload.hitT = 1e38f;
	
	// レイ追跡
	TraceRay(gScene, RAY_FLAG_NONE, 0xFF,
		RAY_TYPE_RADIANCE, 2, RAY_TYPE_RADIANCE, ray, rPayload);

	 // 衝突なしなら影無し
	if (rPayload.hitT >= 1e38f) {
		
		gShadowMask[index] = 1.0f;
		return;
	}
	
	//============================================================================
	//	シャドウレイ
	//============================================================================
	
	ShadowPayload sPayload;
	sPayload.occluded = false;
	
	RayDesc shadowRay;
	const float EPS = 1e-3f;
	shadowRay.Origin = rPayload.worldPos + rPayload.worldNormal * EPS;
	shadowRay.Direction = -lightDirection;
	shadowRay.TMin = 0.0f;
	shadowRay.TMax = 1e38;
	
	// レイ追跡
	TraceRay(gScene,
		D3D12_RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH |
		D3D12_RAY_FLAG_SKIP_CLOSEST_HIT_SHADER,
		0xFF, RAY_TYPE_SHADOW, 2, RAY_TYPE_SHADOW, shadowRay, sPayload);
	
	// 結果を書き込む
	gShadowMask[index] = sPayload.occluded ? 0.0f : 1.0f;
}

//============================================================================
//	ShadowRayQuery
//============================================================================

//============================================================================
//	CBuffer
//============================================================================

cbuffer Scene : register(b0) {
	
	// window
	uint2 dimensions;
	
	// light
	float3 lightDirection;
	float shadowBias;
	float shadowFar;
	
	// camera
	float4x4 inverseView;
	float4x4 inverseProjection;
};

//============================================================================
//	Buffer
//============================================================================

RaytracingAccelerationStructure gScene : register(t0);
Texture2D<float> gDepthTexture : register(t1);
RWTexture2D<float> gShadowMask : register(u0);

//============================================================================
//	function
//============================================================================

bool IsShadowed(float3 origin, float3 direction, float tMin, float tMax) {
	
	// rayDesc‚ğİ’è
	RayDesc ray;
	ray.Origin = origin;
	ray.Direction = direction;
	ray.TMin = tMin;
	ray.TMax = tMax;
	
	// RayQueryObject‚ğì¬‚µAÀs‚·‚é
	RayQuery <
		RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH |
		RAY_FLAG_CULL_NON_OPAQUE |
		RAY_FLAG_SKIP_PROCEDURAL_PRIMITIVES > rayQuery;
	
	rayQuery.TraceRayInline(gScene, 0, 0xFF, ray);
	rayQuery.Proceed();
	
	return rayQuery.CommittedStatus() != COMMITTED_NOTHING;
}

//============================================================================
//	main
//============================================================================
[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {

	uint2 pixel = DTid.xy;
	if (pixel.x >= dimensions.x || pixel.y >= dimensions.y) {
		return;
	}
	
	// ‰æ–ÊÀ•W ¨ NDC ¨ view ¨ world‚É•ÏŠ·
	float2 uv = (pixel + 0.5f) / dimensions;
	float2 ndc = uv * 2.0f - 1.0f;

	float depth = gDepthTexture.Load(int3(pixel, 0));
	float4 clip = float4(ndc, depth, 1.0f);
	float4 view = mul(inverseProjection, clip);
	view /= view.w;
	
	// ƒ[ƒ‹ƒhÀ•W‚ğæ“¾
	float3 worldPos = mul(inverseView, view).xyz;
	// ŒõŒ¹‚Ì‹t‚ğ’H‚é
	float3 reverseLightDirection = -lightDirection;
	// ‰e”»’è‚ğs‚¤
	// true:  ‰e
	// false: ‰e‚¶‚á‚È‚¢
	bool occluded = IsShadowed(worldPos + reverseLightDirection * shadowBias,
	reverseLightDirection, 0.0f, shadowFar);
	
	// ”»’èŒ‹‰Ê‚É‰‚¶‚ÄŒ‹‰Ê‚ğ‘‚«‚Ş
	if (occluded == 1) {
		
		gShadowMask[pixel] = 0.0f;
	} else if (occluded == 0) {
		
		gShadowMask[pixel] = 1.0f;
	}
}
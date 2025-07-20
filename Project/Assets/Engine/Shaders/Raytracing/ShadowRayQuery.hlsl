
//============================================================================
//	ShadowRayQuery
//============================================================================

//============================================================================
//	CBuffer
//============================================================================

cbuffer Scene : register(b0) {

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
	
	// rayDescを設定
	RayDesc rayDesc;
	rayDesc.Origin = origin;
	rayDesc.Direction = direction;
	rayDesc.TMin = tMin;
	rayDesc.TMax = tMax;
	
	// RayQueryObjectを作成し、実行する
	RayQuery < 0 > rayQuery;
	rayQuery.TraceRayInline(gScene, 0, 0xFF, rayDesc);
	
	while (rayQuery.Proceed()) {
	}

	return rayQuery.CommittedStatus() == COMMITTED_TRIANGLE_HIT;
}

//============================================================================
//	main
//============================================================================
[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {

	// pixel位置とサイズを取得する
	uint2 pixel = DTid.xy;
	uint width, height;
	gDepthTexture.GetDimensions(width, height);
	if (pixel.x >= width || pixel.y >= height) {
		return;
	}
	
	// 画面座標 → NDC → view → worldに変換
	float2 uv = (pixel + 0.5f) / float2(width, height);
	float2 ndc = uv * 2.0f - 1.0f;

	float depth = gDepthTexture.Load(int3(pixel, 0));
	float z = depth * 2.0f - 1.0f;
	float4 clip = float4(ndc, z, 1.0f);
	float4 view = mul(inverseProjection, clip);
	view /= view.w;
	
	// ワールド座標を取得
	float3 worldPos = mul(inverseView, view).xyz;
	// 光源の逆を辿る
	float3 reverseLightDirection = -lightDirection;
	// 影判定を行う
	// true:  影
	// false: 影じゃない
	bool occluded = IsShadowed(worldPos, reverseLightDirection, 0.01f, shadowFar);
	
	// 判定結果に応じて結果を書き込む
	if (occluded) {
		
		gShadowMask[pixel] = 0.2f;
	} else {
		
		gShadowMask[pixel] = 1.0f;
	}
}
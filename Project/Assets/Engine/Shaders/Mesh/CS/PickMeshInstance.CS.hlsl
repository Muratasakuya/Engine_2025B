//============================================================================
//	CBuffer
//============================================================================

// ピッキング処理時に判定を行う用のデータ
cbuffer PickingBuffer : register(b0) {

	uint inputPixelX;
	uint inputPixelY;
	uint textureWidth;
	uint textureHeight;
	float4x4 inverseViewProjection;
	float3 cameraWorldPos;
	float rayMax;
};

//============================================================================
//	StructuredBuffer
//============================================================================

// ピッキング結果を格納する構造体
struct Pick {

	int id;
};

RaytracingAccelerationStructure gScene : register(t0);
RWStructuredBuffer<Pick> gOutput : register(u0);

//============================================================================
//	Functions
//============================================================================

// NDCへ変換
float3 NDCFromPixel(uint2 pixel, uint2 size, float depth) {

	float2 uv = (float2(pixel) + 0.5f) / float2(size);
	float2 ndc = float2(uv.x * 2.0f - 1.0f, 1.0f - uv.y * 2.0f);
	return float3(ndc, depth);
}

//============================================================================
//	Main
//============================================================================
[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	
	// クリックピクセルの近、遠点をワールドに変換する
	uint2 textureSize = uint2(textureWidth, textureHeight);
	uint2 inputPixel = uint2(inputPixelX, inputPixelY);
	float4 nearPos = mul(float4(NDCFromPixel(inputPixel, textureSize, 0.0f), 1.0f), inverseViewProjection);
	float4 farPos = mul(float4(NDCFromPixel(inputPixel, textureSize, 1.0f), 1.0f), inverseViewProjection);
	nearPos /= nearPos.w;
	farPos /= farPos.w;
	
	// デスク設定
	RayDesc rayDesc;
	// カメラ座標を始点にしてレイを飛ばす
	rayDesc.Origin = cameraWorldPos;
	rayDesc.Direction = normalize(farPos.xyz - nearPos.xyz);
	rayDesc.TMin = 0.0f;
	rayDesc.TMax = rayMax;
	
	RayQuery < 0 > rayQuery;
	rayQuery.TraceRayInline(gScene, 0, 0xFF, rayDesc);

	while (rayQuery.Proceed()) {
		// 透明のオブジェクトは処理しない
		if (rayQuery.CandidateType() == CANDIDATE_NON_OPAQUE_TRIANGLE) {

			// ヒットした三角形を処理
			rayQuery.CommitNonOpaqueTriangleHit();
		}
	}
	
	gOutput[0].id = -1;
	// ヒットしたインスタンスのIDを取得する
	if (rayQuery.CommittedStatus() == COMMITTED_TRIANGLE_HIT) {
		
		gOutput[0].id = rayQuery.CommittedInstanceID();
	}
}
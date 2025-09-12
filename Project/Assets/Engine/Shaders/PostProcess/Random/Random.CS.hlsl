//============================================================================
//	include
//============================================================================

#include "../../Math/Math.hlsli"
#include "../../../../../Engine/Core/Graphics/PostProcess/PostProcessConfig.h"

//============================================================================
//	CBuffer
//============================================================================

struct RandomMaterial {
	
	float time;
	uint enable;
};
ConstantBuffer<RandomMaterial> gMaterial : register(b0);

//============================================================================
//	buffer
//============================================================================

RWTexture2D<float4> gOutputTexture : register(u0);
Texture2D<float4> gRenderTexture : register(t0);

//============================================================================
//	main
//============================================================================
[numthreads(THREAD_POSTPROCESS_GROUP, THREAD_POSTPROCESS_GROUP, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	
	uint width, height;
	gRenderTexture.GetDimensions(width, height);
	
	 // ピクセル位置
	uint2 pixelPos = DTid.xy;

	// 画像範囲外チェック
	if (pixelPos.x >= width || pixelPos.y >= height) {
		return;
	}
	
	// 有効でないなら入力画像の色をそのまま返す
	if (gMaterial.enable == 0) {
		
		gOutputTexture[pixelPos] = gRenderTexture.Load(int3(pixelPos, 0));
		return;
	}
	
	// 乱数、時間で変化させる
	float random = Rand2DTo1D(pixelPos * gMaterial.time);
	gOutputTexture[pixelPos] = float4(random, random, random, 1.0f);
}
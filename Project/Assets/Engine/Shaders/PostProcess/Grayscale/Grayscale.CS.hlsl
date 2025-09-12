//============================================================================
//	include
//============================================================================

#include "../../../../../Engine/Core/Graphics/PostProcess/PostProcessConfig.h"

//============================================================================
//	buffer
//============================================================================

RWTexture2D<float4> gOutputTexture : register(u0);
Texture2D<float4> gTexture : register(t0);

//============================================================================
//	Main
//============================================================================
[numthreads(THREAD_POSTPROCESS_GROUP, THREAD_POSTPROCESS_GROUP, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	
	uint width, height;
	gTexture.GetDimensions(width, height);

	// ピクセル位置
	uint2 pixelPos = DTid.xy;

	// 範囲外
	if (pixelPos.x >= width || pixelPos.y >= height) {
		return;
	}

	// テクスチャのサンプル
	float4 color = gTexture.Load(int3(pixelPos, 0));
	// グレースケール変換
	float grayscale = dot(color.rgb, float3(0.2125f, 0.7154f, 0.0721f));
	float3 finalColor = float3(grayscale, grayscale, grayscale);

	gOutputTexture[pixelPos] = float4(finalColor, color.a);
}
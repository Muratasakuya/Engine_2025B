//============================================================================
//	include
//============================================================================

#include "../../../../../Engine/Core/Graphics/PostProcess/PostProcessConfig.h"

//============================================================================
//	CBuffer
//============================================================================

struct VignetteParameter {
	
	float scale;  // スケール調整
	float power;  // 乗算パラメータ
	float3 color; // 色
};
ConstantBuffer<VignetteParameter> gVignette : register(b0);

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

	// UV座標
	float2 uv = (float2(pixelPos) + 0.5f) / float2(width, height);

	// ビネット効果の計算
	float2 correct = uv * (1.0f - uv.yx);
	float vignette = correct.x * correct.y * gVignette.scale;
	vignette = saturate(pow(vignette, gVignette.power));

	// テクスチャのサンプル
	float4 color = gTexture.Load(int3(pixelPos, 0));
	// ビネット適用
	color.rgb = lerp(color.rgb, gVignette.color, 1.0f - vignette);

	gOutputTexture[pixelPos] = color;
}
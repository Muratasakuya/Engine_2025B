//============================================================================
//	include
//============================================================================

#include "../../Math/Math.hlsli"

//============================================================================
//	CBuffer
//============================================================================

struct GlitchMaterial {
	
	float time; // 経過秒
	float intensity; // グリッチ全体の強さ
	float blockSize; // 横ずれブロックの太さ
	float colorOffset; // RGBずれ距離
	float noiseStrength; // ノイズ混合率
	float lineSpeed; // スキャンライン走査速度
};
ConstantBuffer<GlitchMaterial> gMaterial : register(b0);

//============================================================================
//	RWStructuredBuffer
//============================================================================

RWTexture2D<float4> gOutputTexture : register(u0);
Texture2D<float4> gRenderTexture : register(t0);
Texture2D<float> gNoiseTexture : register(t1);

SamplerState gSampler : register(s0);

//============================================================================
//	Functions
//============================================================================

float Hash12(float2 p) {
	
	p = frac(p * 0.3183099f + float2(0.71f, 0.113f));
	return frac(23.357f * dot(p, p + float2(21.17f, 37.95f)));
}

//============================================================================
//	main
//============================================================================
[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	
	uint width, height;
	gRenderTexture.GetDimensions(width, height);
	
	// 範囲外アクセス回避
	if (DTid.x >= width || DTid.y >= height) {
		return;
	}
	
	float2 uv = (float2) DTid.xy / float2(width, height);
	
	// 横スキャンラインずらし
	float linePhase = Hash12(float2(0, DTid.y * 7.13) + gMaterial.time * gMaterial.lineSpeed);
	// たまに大きく跳ねるブロックグリッチ
	float lineAmp = smoothstep(0.8f, 1.0f, linePhase);
	// ブロック単位で段差状にしたい場合
	float blockStep = round(DTid.x / gMaterial.blockSize) * gMaterial.blockSize;
	float2 glitchOffs = float2(lineAmp * gMaterial.intensity * gMaterial.blockSize, 0);

	// 元ピクセルとずらしたピクセル取得
	int2 srcPos = int2(clamp(int(DTid.x + glitchOffs.x), 0, int(width - 1)),DTid.y);
	float4 srcColor = gRenderTexture.Load(int3(srcPos, 0));
	
	// Chromatic Aberration風
	float2 rgbShift = float2((Hash12(float2(gMaterial.time, srcPos.y)) - 0.5f) * 2.0f,
	(Hash12(float2(gMaterial.time + 11.3f, srcPos.y)) - 0.5f) * 2.0f) *
	gMaterial.colorOffset * gMaterial.intensity;

	// r g,bそれぞれ別座標からサンプル
	int2 offR = clamp(srcPos + int2(rgbShift), int2(0, 0), int2(width - 1, height - 1));
	int2 offB = clamp(srcPos + int2(-rgbShift), int2(0, 0), int2(width - 1, height - 1));

	float3 col;
	col.r = gRenderTexture.Load(int3(offR, 0)).r;
	col.g = srcColor.g;
	col.b = gRenderTexture.Load(int3(offB, 0)).b;

	// ホワイトノイズオーバーレイ
	float noise = gNoiseTexture.SampleLevel(gSampler, uv * 512.0f, 0.0f).r;
	col = lerp(col, float3(noise, noise, noise), gMaterial.noiseStrength * gMaterial.intensity);

	// 最終的な色を出力
	gOutputTexture[DTid.xy] = float4(col, 1.0f);
}

//============================================================================*/
//	Lut.CS
//============================================================================*/

//============================================================================*/
//	Buffer
//============================================================================*/

cbuffer Params : register(b0) {
	
	float lerpRate;
	float lutSize;
};

//============================================================================*/
//	Resource
//============================================================================*/

RWTexture2D<float4> gOutputTexture : register(u0);
Texture2D<float4> gInputTexture : register(t0);
Texture3D<float4> gLutTexture : register(t1);
SamplerState gSampler : register(s0);

//============================================================================*/
//	Main
//============================================================================*/
[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	
	uint width, height;
	gInputTexture.GetDimensions(width, height);
	
	// 範囲外アクセス防止
	if (DTid.x >= width || DTid.y >= height) {
		return;
	}
	
	// input取得
	float3 inputColor = gInputTexture.Load(int3(DTid.xy, 0)).rgb;
	// clamp処理しないと帯域外アクセス
	float3 graded = gLutTexture.SampleLevel(gSampler, inputColor, 0.0f).rgb;
	
	// outputに書き込み
	float3 final = lerp(inputColor, graded, lerpRate);
	gOutputTexture[DTid.xy] = float4(final, 1.0f);
}
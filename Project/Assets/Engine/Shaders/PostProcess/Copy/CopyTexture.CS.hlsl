
//============================================================================*/
//	CopyTexture.CS
//============================================================================*/

RWTexture2D<float4> gOutputTexture : register(u0);
Texture2D<float4> gTexture : register(t0);

[numthreads(8, 8, 1)]
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
	// α値を1.0fに固定
	gOutputTexture[pixelPos] = float4(color.rgb, 1.0f);
}
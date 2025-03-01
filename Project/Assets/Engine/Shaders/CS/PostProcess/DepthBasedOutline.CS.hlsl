
//============================================================================*/
//	DepthBasedOutline.CS
//============================================================================*/

struct OutlineMaterial {
	
	float4x4 projectionInverse;
};

RWTexture2D<float4> gOutputTexture : register(u0);
Texture2D<float4> gRenderTexture : register(t0);
Texture2D<float> gDepthTexture : register(t1);
SamplerState gSamplerLinear : register(s0);
ConstantBuffer<OutlineMaterial> gMaterial : register(b0);

// 3x3のオフセット
static const int2 kIndex3x3[3][3] = {
	{ { -1, -1 }, { 0, -1 }, { 1, -1 } },
	{ { -1, 0 }, { 0, 0 }, { 1, 0 } },
	{ { -1, 1 }, { 0, 1 }, { 1, 1 } }
};

// Prewitt フィルタ (横方向)
static const float kPrewittHorizontalKernel[3][3] = {
	{ -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
	{ -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
	{ -1.0f / 6.0f, 0.0f, 1.0f / 6.0f }
};

// Prewitt フィルタ (縦方向)
static const float kPrewittVerticalKernel[3][3] = {
	{ -1.0f / 6.0f, -1.0f / 6.0f, -1.0f / 6.0f },
	{ 0.0f, 0.0f, 0.0f },
	{ 1.0f / 6.0f, 1.0f / 6.0f, 1.0f / 6.0f }
};

// RGB → 輝度変換
float Luminance(float3 v) {
	
	return dot(v, float3(0.2125f, 0.7154f, 0.0721f));
}

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {

	uint width, height;
	gRenderTexture.GetDimensions(width, height);
	
	 // ピクセル位置
	uint2 pixelPos = DTid.xy;

	// 画像範囲外チェック
	if (pixelPos.x >= width || pixelPos.y >= height) {
		return;
	}
	
	// UVステップサイズ (ピクセル間隔)
	float2 uvStepSize = float2(1.0f / width, 1.0f / height);

	// Prewitt フィルタ用の勾配
	float2 difference = float2(0.0f, 0.0f);

	// 3x3 畳み込み処理
	for (int x = 0; x < 3; ++x) {
		for (int y = 0; y < 3; ++y) {
			int2 offset = kIndex3x3[x][y];

			// ピクセル座標を計算 (範囲外処理)
			int2 samplePos = clamp(pixelPos + offset, int2(0, 0), int2(width - 1, height - 1));

			// 深度テクスチャから NDC 深度取得
			float ndcDepth = gDepthTexture.Load(int3(samplePos, 0)).r;

			// NDC → View 空間変換 (逆射影行列)
			float4 viewSpace = mul(float4(0.0f, 0.0f, ndcDepth, 1.0f), gMaterial.projectionInverse);
			float viewZ = viewSpace.z / viewSpace.w; // 同次座標変換

			// Prewitt フィルタ適用
			difference.x += viewZ * kPrewittHorizontalKernel[x][y];
			difference.y += viewZ * kPrewittVerticalKernel[x][y];
		}
	}

	// 変化の大きさを計算
	float weight = length(difference);
	weight = saturate(weight);

	// 出力カラーの計算 (元画像 + エッジ強調)
	float4 originalColor = gRenderTexture.SampleLevel(gSamplerLinear, (float2(pixelPos) + 0.5f) / float2(width, height), 0);
	float3 finalColor = (1.0f - weight) * originalColor.rgb;

	gOutputTexture[pixelPos] = float4(finalColor, 1.0f);
}
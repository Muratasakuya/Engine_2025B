//============================================================================
//	include
//============================================================================

#include "../../../../../Engine/Core/Graphics/PostProcess/PostProcessConfig.h"

//============================================================================
//	Constant
//============================================================================

// 3x3�̃I�t�Z�b�g
static const int2 kIndex3x3[3][3] = {
	{ { -1, -1 }, { 0, -1 }, { 1, -1 } },
	{ { -1, 0 }, { 0, 0 }, { 1, 0 } },
	{ { -1, 1 }, { 0, 1 }, { 1, 1 } }
};

// Prewitt �t�B���^ (������)
static const float kPrewittHorizontalKernel[3][3] = {
	{ -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
	{ -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
	{ -1.0f / 6.0f, 0.0f, 1.0f / 6.0f }
};

// Prewitt �t�B���^ (�c����)
static const float kPrewittVerticalKernel[3][3] = {
	{ -1.0f / 6.0f, -1.0f / 6.0f, -1.0f / 6.0f },
	{ 0.0f, 0.0f, 0.0f },
	{ 1.0f / 6.0f, 1.0f / 6.0f, 1.0f / 6.0f }
};

//============================================================================
//	CBuffer
//============================================================================

struct OutlineMaterial {
	
	float4x4 projectionInverse;
	
	float edgeScale;
	float threshold;
	
	float3 color;
};

ConstantBuffer<OutlineMaterial> gMaterial : register(b0);

//============================================================================
//	RWStructuredBuffer
//============================================================================

RWTexture2D<float4> gOutputTexture : register(u0);

//============================================================================
//	Texture
//============================================================================

Texture2D<float4> gRenderTexture : register(t0);
Texture2D<float> gDepthTexture : register(t1);
SamplerState gSamplerLinear : register(s0);
SamplerState gSamplerPoint : register(s1);

//============================================================================
//	main
//============================================================================
[numthreads(THREAD_POSTPROCESS_GROUP, THREAD_POSTPROCESS_GROUP, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {

	uint width, height;
	gRenderTexture.GetDimensions(width, height);
	
	 // �s�N�Z���ʒu
	uint2 pixelPos = DTid.xy;

	// �摜�͈͊O�`�F�b�N
	if (pixelPos.x >= width || pixelPos.y >= height) {
		return;
	}
	
	float2 gradient = float2(0.0f, 0.0f);

	// �T���v�����O����
	for (int y = 0; y < 3; ++y) {
		for (int x = 0; x < 3; ++x) {
			
			int2 offset = kIndex3x3[x][y];
			int2 sampleP = clamp(int2(pixelPos) + offset, int2(0, 0), int2(width - 1, height - 1));

			float2 uv = (float2(sampleP) + 0.5f) / float2(width, height);
			float ndcDepth = gDepthTexture.SampleLevel(gSamplerPoint, uv, 0).r;

			// NDC��clip
			float2 clipXY = uv * 2.0f - 1.0f;
			clipXY.y *= -1.0f;
			float4 clipPos = float4(clipXY, ndcDepth, 1.0f);

			// clip��view
			float4 viewPos = mul(clipPos, gMaterial.projectionInverse);
			viewPos /= viewPos.w;
			float viewZ = viewPos.z;

			gradient.x += viewZ * kPrewittHorizontalKernel[x][y];
			gradient.y += viewZ * kPrewittVerticalKernel[x][y];
		}
	}

	float weight = saturate(max(length(gradient) - gMaterial.threshold, 0.0f) * gMaterial.edgeScale);

	// ���J���[�ƍ������ďo��
	float2 uv = (float2(pixelPos) + 0.5f) / float2(width, height);
	float4 srcColor = gRenderTexture.SampleLevel(gSamplerLinear, uv, 0);

	float3 finalColor = lerp(gMaterial.color, srcColor.rgb, 1.0f - weight);

	gOutputTexture[pixelPos] = float4(finalColor, 1.0f);
}
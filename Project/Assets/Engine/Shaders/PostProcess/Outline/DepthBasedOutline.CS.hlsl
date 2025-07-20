
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

// RGB �� �P�x�ϊ�
float Luminance(float3 v) {
	
	return dot(v, float3(0.2125f, 0.7154f, 0.0721f));
}

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {

	uint width, height;
	gRenderTexture.GetDimensions(width, height);
	
	 // �s�N�Z���ʒu
	uint2 pixelPos = DTid.xy;

	// �摜�͈͊O�`�F�b�N
	if (pixelPos.x >= width || pixelPos.y >= height) {
		return;
	}
	
	// UV�X�e�b�v�T�C�Y (�s�N�Z���Ԋu)
	float2 uvStepSize = float2(1.0f / width, 1.0f / height);

	// Prewitt �t�B���^�p�̌��z
	float2 difference = float2(0.0f, 0.0f);

	// 3x3 ��ݍ��ݏ���
	for (int x = 0; x < 3; ++x) {
		for (int y = 0; y < 3; ++y) {
			
			int2 offset = kIndex3x3[x][y];

			// �s�N�Z�����W���v�Z (�͈͊O����)
			int2 samplePos = clamp(pixelPos + offset, int2(0, 0), int2(width - 1, height - 1));

			// �[�x�e�N�X�`������ NDC �[�x�擾
			float ndcDepth = gDepthTexture.Load(int3(samplePos, 0)).r;

			// NDC �� View ��ԕϊ� (�t�ˉe�s��)
			float4 viewSpace = mul(float4(0.0f, 0.0f, ndcDepth, 1.0f), gMaterial.projectionInverse);
			float viewZ = viewSpace.z / viewSpace.w; // �������W�ϊ�

			// Prewitt �t�B���^�K�p
			difference.x += viewZ * kPrewittHorizontalKernel[x][y];
			difference.y += viewZ * kPrewittVerticalKernel[x][y];
		}
	}

	// �ω��̑傫�����v�Z
	float weight = length(difference);
	weight = saturate(weight);

	// �o�̓J���[�̌v�Z (���摜 + �G�b�W����)
	float4 originalColor = gRenderTexture.SampleLevel(gSamplerLinear, (float2(pixelPos) + 0.5f) / float2(width, height), 0);
	float3 finalColor = (1.0f - weight) * originalColor.rgb;

	gOutputTexture[pixelPos] = float4(finalColor, 1.0f);
}
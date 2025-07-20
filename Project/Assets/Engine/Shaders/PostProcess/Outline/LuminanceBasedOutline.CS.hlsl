
//============================================================================*/
//	LuminanceBasedOutline.CS
//============================================================================*/

struct EdgeParameter {
	
	float strength; // �G�b�W�̋���
};

RWTexture2D<float4> gOutputTexture : register(u0);
Texture2D<float4> gTexture : register(t0);
ConstantBuffer<EdgeParameter> gEdge : register(b0);

// 3x3 �̃I�t�Z�b�g
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
	return dot(v, float3(0.2125f, 0.7145f, 0.0721f));
}

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	
	uint width, height;
	gTexture.GetDimensions(width, height);

	// �s�N�Z���ʒu
	uint2 pixelPos = DTid.xy;

	// �͈͊O
	if (pixelPos.x >= width || pixelPos.y >= height) {
		return;
	}

	// UV�X�e�b�v�T�C�Y
	float2 uvStepSize = float2(1.0f / width, 1.0f / height);

	// Prewitt �t�B���^�p�̌��z
	float2 difference = float2(0.0f, 0.0f);

	// 3x3 ��ݍ��ݏ���
	for (int x = 0; x < 3; ++x) {
		for (int y = 0; y < 3; ++y) {
			
			int2 offset = kIndex3x3[x][y];

			// �s�N�Z���ʒu���v�Z
			int2 samplePos = clamp(pixelPos + offset, int2(0, 0), int2(width - 1, height - 1));

			// �e�N�X�`���̃T���v��
			float3 sampleColor = gTexture.Load(int3(samplePos, 0)).rgb;

			// �P�x�v�Z
			float luminance = Luminance(sampleColor);

			// Prewitt �t�B���^�K�p
			difference.x += luminance * kPrewittHorizontalKernel[x][y];
			difference.y += luminance * kPrewittVerticalKernel[x][y];
		}
	}

	// �ω��̑傫�����v�Z
	float weight = length(difference) * gEdge.strength;
	weight = saturate(weight);
	// ���̃e�N�X�`���J���[���擾
	float4 originalColor = gTexture.Load(int3(pixelPos, 0));
	// �G�b�W��K�p
	float3 finalColor = (1.0f - weight) * originalColor.rgb;

	gOutputTexture[pixelPos] = float4(finalColor, 1.0f);
}
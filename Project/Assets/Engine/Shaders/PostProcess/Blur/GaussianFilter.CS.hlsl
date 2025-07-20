
//============================================================================*/
//	GaussianFilter.CS
//============================================================================*/

struct GaussParameter {
	
	float sigma; // �W���΍� �ڂ����̋���
};

RWTexture2D<float4> gOutputTexture : register(u0);
Texture2D<float4> gTexture : register(t0);
ConstantBuffer<GaussParameter> gGauss : register(b0);

// 3x3 �̃I�t�Z�b�g
static const int2 kIndex3x3[3][3] = {
	{ { -1, -1 }, { 0, -1 }, { 1, -1 } },
	{ { -1, 0 }, { 0, 0 }, { 1, 0 } },
	{ { -1, 1 }, { 0, 1 }, { 1, 1 } }
};

// ��
static const float PI = 3.14159265f;

// �K�E�X�֐�
float Gauss(float x, float y, float sigma) {
	
	float exponent = -(x * x + y * y) / (2.0f * sigma * sigma);
	float denominator = 2.0f * PI * sigma * sigma;
	return exp(exponent) / denominator;
}

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	
	uint width, height;
	gTexture.GetDimensions(width, height);

	// �s�N�Z���ʒu
	uint2 pixelPos = DTid.xy;

	// �͈͊O�Ȃ�X�L�b�v
	if (pixelPos.x >= width || pixelPos.y >= height) {
		return;
	}

	// UV�X�e�b�v�T�C�Y
	float2 uvStepSize = float2(1.0f / width, 1.0f / height);

	// �J�[�l���ƃE�F�C�g�̌v�Z
	float kernel3x3[3][3];
	float weight = 0.0f;

	for (int x = 0; x < 3; ++x) {
		for (int y = 0; y < 3; ++y) {
			
			kernel3x3[x][y] = Gauss(kIndex3x3[x][y].x, kIndex3x3[x][y].y, gGauss.sigma);
			weight += kernel3x3[x][y];
		}
	}

	// ��ݍ��ݏ���
	float3 blurredColor = float3(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			
			int2 offset = kIndex3x3[i][j];

			// �s�N�Z���ʒu���v�Z
			int2 samplePos = clamp(pixelPos + offset, int2(0, 0), int2(width - 1, height - 1));

			// �e�N�X�`���̃T���v��
			float3 sampleColor = gTexture.Load(int3(samplePos, 0)).rgb;

			// �J�[�l���l���|����
			blurredColor += sampleColor * kernel3x3[i][j];
		}
	}

	// ���K��
	blurredColor *= rcp(weight);

	gOutputTexture[pixelPos] = float4(blurredColor, 1.0f);
}
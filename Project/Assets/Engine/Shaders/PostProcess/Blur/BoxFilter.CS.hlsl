
//============================================================================*/
//	BoxFilter.CS
//============================================================================*/

RWTexture2D<float4> gOutputTexture : register(u0);
Texture2D<float4> gInputTexture : register(t0);

// 5x5��Index
static const int2 kIndex5x5[5][5] = {
	{ { -2, -2 }, { -1, -2 }, { 0, -2 }, { 1, -2 }, { 2, -2 } },
	{ { -2, -1 }, { -1, -1 }, { 0, -1 }, { 1, -1 }, { 2, -1 } },
	{ { -2, 0 }, { -1, 0 }, { 0, 0 }, { 1, 0 }, { 2, 0 } },
	{ { -2, 1 }, { -1, 1 }, { 0, 1 }, { 1, 1 }, { 2, 1 } },
	{ { -2, 2 }, { -1, 2 }, { 0, 2 }, { 1, 2 }, { 2, 2 } }
};

// 5x5�J�[�l��
static const float kKernel5x5[5][5] = {
	{ 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f },
	{ 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f },
	{ 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f },
	{ 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f },
	{ 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f }
};

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	
	uint width, height;
	gInputTexture.GetDimensions(width, height);
	
	 // �s�N�Z���ʒu
	uint2 pixelPos = DTid.xy;

	// �摜�͈͊O�Ȃ珈�����Ȃ�
	if (pixelPos.x >= width || pixelPos.y >= height) {
		return;
	}

	float3 color = float3(0.0f, 0.0f, 0.0f);

	// 5x5 Box Filter �K�p
	for (int x = 0; x < 5; ++x) {
		for (int y = 0; y < 5; ++y) {
			
			int2 offset = kIndex5x5[x][y];

			// �e�N�X�`�����W�̌v�Z
			int2 samplePos = clamp(pixelPos + offset, int2(0, 0), int2(width - 1, height - 1));
			// �T���v�����O
			float3 sampleColor = gInputTexture.Load(int3(samplePos, 0)).rgb;
			// �J�[�l���K�p
			color += sampleColor * kKernel5x5[x][y];
		}
	}

	gOutputTexture[pixelPos] = float4(color, 1.0f);
}
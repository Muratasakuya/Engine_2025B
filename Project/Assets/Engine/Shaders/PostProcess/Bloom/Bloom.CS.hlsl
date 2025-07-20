//============================================================================
//	Bloom.CS
//============================================================================

//============================================================================
//	CBuffer
//============================================================================

cbuffer Parameter : register(b0) {
	
	float threshold;
	int radius;
	float sigma;
};

//============================================================================
//	buffer
//============================================================================

Texture2D<float4> gInputTexture : register(t0);
RWTexture2D<float4> gOutputTexture : register(u0);

//============================================================================
//	Function
//============================================================================

// �K�E�X�֐�
float Gaussian(float x, float s) {
	
	return exp(-(x * x) / (2.0f * s * s));
}

//============================================================================
//	Main
//============================================================================
[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	
	uint width, height;
	gInputTexture.GetDimensions(width, height);

	// ���ݏ������̃s�N�Z��
	int2 pixelPos = int2(DTid.xy);

	// ���J���[�ێ�
	float4 sceneColor = gInputTexture.Load(int3(pixelPos, 0));
	// �͈͊O
	if (pixelPos.x >= width || pixelPos.y >= height) {
		return;
	}

	// �T���v�����O����
	float3 bloomAccum = 0.0f;
	float weightSum = 0.0f;
	
	for (int y = -radius; y <= radius; ++y) {
		
		float wy = Gaussian((float) y, sigma);
		for (int x = -radius; x <= radius; ++x) {
			
			float wx = Gaussian((float) x, sigma);
			float w = wx * wy;

			int2 samplePos = pixelPos + int2(x, y);
			samplePos = clamp(samplePos, int2(0, 0), int2(int(width) - 1, int(height) - 1));

			float4 color = gInputTexture.Load(int3(samplePos, 0));

			// �P�x���o
			float luminance = dot(color.rgb, float3(0.2125f, 0.7154f, 0.0721f));
			if (luminance < threshold) {
				color = float4(0.0f, 0.0f, 0.0f, 0.0f);
			}

			bloomAccum += color.rgb * w;
			weightSum += w;
		}
	}
	
	// ��������
	float3 bloomColor = (weightSum > 0.0f) ? bloomAccum / weightSum : 0.0f;
	float3 finalColor = sceneColor.rgb + bloomColor;
	gOutputTexture[pixelPos] = float4(finalColor, 1.0f);
}
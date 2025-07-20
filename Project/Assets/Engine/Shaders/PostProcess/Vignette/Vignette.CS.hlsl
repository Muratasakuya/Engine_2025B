
//============================================================================*/
//	Vignette.CS
//============================================================================*/

struct VignetteParameter {
	
	float scale;  // �X�P�[������
	float power;  // ��Z�p�����[�^
	float3 color; // �F
};

RWTexture2D<float4> gOutputTexture : register(u0);
Texture2D<float4> gTexture : register(t0);
ConstantBuffer<VignetteParameter> gVignette : register(b0);

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

	// UV���W
	float2 uv = (float2(pixelPos) + 0.5f) / float2(width, height);

	// �r�l�b�g���ʂ̌v�Z
	float2 correct = uv * (1.0f - uv.yx);
	float vignette = correct.x * correct.y * gVignette.scale;
	vignette = saturate(pow(vignette, gVignette.power));

	// �e�N�X�`���̃T���v��
	float4 color = gTexture.Load(int3(pixelPos, 0));
	// �r�l�b�g�K�p
	color.rgb = lerp(color.rgb, gVignette.color, 1.0f - vignette);

	gOutputTexture[pixelPos] = color;
}
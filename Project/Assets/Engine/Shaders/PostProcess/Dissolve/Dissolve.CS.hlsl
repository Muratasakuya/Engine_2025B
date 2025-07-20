
//============================================================================*/
//	Dissolve.CS
//============================================================================*/

struct Material {
	
	float threshold;
	float edgeSize;
	float3 edgeColor;
	float3 thresholdColor;
};

RWTexture2D<float4> gOutputTexture : register(u0);
Texture2D<float4> gTexture : register(t0);
Texture2D<float> gMaskTexture : register(t1);
SamplerState gSampler : register(s0);
ConstantBuffer<Material> gMaterial : register(b0);

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	
	uint width, height;
	gTexture.GetDimensions(width, height);

	// �s�N�Z���ʒu
	uint2 pixelPos = DTid.xy;

	// �͈͊O�Ȃ牽�����Ȃ�
	if (pixelPos.x >= width || pixelPos.y >= height) {
		return;
	}
	// �}�X�N�e�N�X�`���̎擾
	float mask = gMaskTexture.SampleLevel(gSampler, (float2(pixelPos) + 0.5f) / float2(width, height), 0).r;
	
	// 臒l�ȉ��Ȃ�F������p�̐F�ɕύX����ɂ���
	if (mask <= gMaterial.threshold) {
		
		gOutputTexture[pixelPos].rgb = gMaterial.thresholdColor;
		gOutputTexture[pixelPos].a = 1.0f;
		return;
	}

	// Edge �̌v�Z
	float edge = 1.0f - smoothstep(gMaterial.threshold, gMaterial.threshold + gMaterial.edgeSize, mask);
	// ���̃e�N�X�`���J���[���擾
	float4 baseColor = gTexture.SampleLevel(gSampler, (float2(pixelPos) + 0.5f) / float2(width, height), 0);
	// Edge ��K�p
	float3 finalColor = lerp(baseColor.rgb, gMaterial.edgeColor, edge);

	gOutputTexture[pixelPos] = float4(finalColor, baseColor.a);
}
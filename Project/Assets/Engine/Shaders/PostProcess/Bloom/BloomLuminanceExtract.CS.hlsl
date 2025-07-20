
//============================================================================*/
//	BloomLuminanceExtract.CS
//============================================================================*/

struct LuminanceExtractParam {
	
	float threshold;
};

RWTexture2D<float4> gOutputTexture : register(u0);
Texture2D<float4> gInputTexture : register(t0);
ConstantBuffer<LuminanceExtractParam> gLuminanceExtract : register(b0);

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	
	float4 color = gInputTexture.Load(int3(DTid.xy, 0));
	float luminance = dot(color.rgb, float3(0.2125f, 0.7154f, 0.0721f));
	
	// �P�x��臒l�����Ȃ疳����
	if (luminance < gLuminanceExtract.threshold) {
		color = float4(0.0f, 0.0f, 0.0f, 0.0f);
	}

	// �o�̓e�N�X�`���ɏ�������
	gOutputTexture[DTid.xy] = color;
}
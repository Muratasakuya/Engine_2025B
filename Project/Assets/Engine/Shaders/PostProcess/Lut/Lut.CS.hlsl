
//============================================================================*/
//	Lut.CS
//============================================================================*/

//============================================================================*/
//	Buffer
//============================================================================*/

cbuffer Params : register(b0) {
	
	float lerpRate;
	float lutSize;
};

//============================================================================*/
//	Resource
//============================================================================*/

RWTexture2D<float4> gOutputTexture : register(u0);
Texture2D<float4> gInputTexture : register(t0);
Texture3D<float4> gLutTexture : register(t1);
SamplerState gSampler : register(s0);

//============================================================================*/
//	Main
//============================================================================*/
[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	
	uint width, height;
	gInputTexture.GetDimensions(width, height);
	
	// �͈͊O�A�N�Z�X�h�~
	if (DTid.x >= width || DTid.y >= height) {
		return;
	}
	
	// input�擾
	float3 inputColor = gInputTexture.Load(int3(DTid.xy, 0)).rgb;
	// clamp�������Ȃ��Ƒш�O�A�N�Z�X
	float3 graded = gLutTexture.SampleLevel(gSampler, inputColor, 0.0f).rgb;
	
	// output�ɏ�������
	float3 final = lerp(inputColor, graded, lerpRate);
	gOutputTexture[DTid.xy] = float4(final, 1.0f);
}
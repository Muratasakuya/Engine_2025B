
//============================================================================*/
//	Random.CS
//============================================================================*/

RWTexture2D<float4> gOutputTexture : register(u0);
Texture2D<float4> gTexture : register(t0);

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	
	// �܂��������Ă��Ȃ��̂ŐF���R�s�[���邾��
	float4 color = gTexture.Load(int3(DTid.xy, 0));
	gOutputTexture[DTid.xy] = color;
}
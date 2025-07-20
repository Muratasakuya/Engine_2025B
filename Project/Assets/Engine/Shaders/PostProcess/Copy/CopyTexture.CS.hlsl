
//============================================================================*/
//	CopyTexture.CS
//============================================================================*/

RWTexture2D<float4> gOutputTexture : register(u0);
Texture2D<float4> gTexture : register(t0);

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

	// �e�N�X�`���̃T���v��
	float4 color = gTexture.Load(int3(pixelPos, 0));
	// ���l��1.0f�ɌŒ�
	gOutputTexture[pixelPos] = float4(color.rgb, 1.0f);
}
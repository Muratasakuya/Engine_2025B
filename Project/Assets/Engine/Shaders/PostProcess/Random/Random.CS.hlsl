//============================================================================
//	include
//============================================================================

#include "../../Math/Math.hlsli"
#include "../../../../../Engine/Core/Graphics/PostProcess/PostProcessConfig.h"

//============================================================================
//	CBuffer
//============================================================================

struct RandomMaterial {
	
	float time;
	uint enable;
};
ConstantBuffer<RandomMaterial> gMaterial : register(b0);

//============================================================================
//	buffer
//============================================================================

RWTexture2D<float4> gOutputTexture : register(u0);
Texture2D<float4> gRenderTexture : register(t0);

//============================================================================
//	main
//============================================================================
[numthreads(THREAD_POSTPROCESS_GROUP, THREAD_POSTPROCESS_GROUP, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	
	uint width, height;
	gRenderTexture.GetDimensions(width, height);
	
	 // �s�N�Z���ʒu
	uint2 pixelPos = DTid.xy;

	// �摜�͈͊O�`�F�b�N
	if (pixelPos.x >= width || pixelPos.y >= height) {
		return;
	}
	
	// �L���łȂ��Ȃ���͉摜�̐F�����̂܂ܕԂ�
	if (gMaterial.enable == 0) {
		
		gOutputTexture[pixelPos] = gRenderTexture.Load(int3(pixelPos, 0));
		return;
	}
	
	// �����A���Ԃŕω�������
	float random = Rand2DTo1D(pixelPos * gMaterial.time);
	gOutputTexture[pixelPos] = float4(random, random, random, 1.0f);
}
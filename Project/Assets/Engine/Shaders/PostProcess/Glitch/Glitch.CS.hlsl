//============================================================================
//	include
//============================================================================

#include "../../Math/Math.hlsli"

//============================================================================
//	CBuffer
//============================================================================

struct GlitchMaterial {
	
	float time; // �o�ߕb
	float intensity; // �O���b�`�S�̂̋���
	float blockSize; // ������u���b�N�̑���
	float colorOffset; // RGB���ꋗ��
	float noiseStrength; // �m�C�Y������
	float lineSpeed; // �X�L�������C���������x
};
ConstantBuffer<GlitchMaterial> gMaterial : register(b0);

//============================================================================
//	RWStructuredBuffer
//============================================================================

RWTexture2D<float4> gOutputTexture : register(u0);
Texture2D<float4> gRenderTexture : register(t0);
Texture2D<float> gNoiseTexture : register(t1);

SamplerState gSampler : register(s0);

//============================================================================
//	Functions
//============================================================================

float Hash12(float2 p) {
	
	p = frac(p * 0.3183099f + float2(0.71f, 0.113f));
	return frac(23.357f * dot(p, p + float2(21.17f, 37.95f)));
}

//============================================================================
//	main
//============================================================================
[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	
	uint width, height;
	gRenderTexture.GetDimensions(width, height);
	
	// �͈͊O�A�N�Z�X���
	if (DTid.x >= width || DTid.y >= height) {
		return;
	}
	
	float2 uv = (float2) DTid.xy / float2(width, height);
	
	// ���X�L�������C�����炵
	float linePhase = Hash12(float2(0, DTid.y * 7.13) + gMaterial.time * gMaterial.lineSpeed);
	// ���܂ɑ傫�����˂�u���b�N�O���b�`
	float lineAmp = smoothstep(0.8f, 1.0f, linePhase);
	// �u���b�N�P�ʂŒi����ɂ������ꍇ
	float blockStep = round(DTid.x / gMaterial.blockSize) * gMaterial.blockSize;
	float2 glitchOffs = float2(lineAmp * gMaterial.intensity * gMaterial.blockSize, 0);

	// ���s�N�Z���Ƃ��炵���s�N�Z���擾
	int2 srcPos = int2(clamp(int(DTid.x + glitchOffs.x), 0, int(width - 1)),DTid.y);
	float4 srcColor = gRenderTexture.Load(int3(srcPos, 0));
	
	// Chromatic Aberration��
	float2 rgbShift = float2((Hash12(float2(gMaterial.time, srcPos.y)) - 0.5f) * 2.0f,
	(Hash12(float2(gMaterial.time + 11.3f, srcPos.y)) - 0.5f) * 2.0f) *
	gMaterial.colorOffset * gMaterial.intensity;

	// r g,b���ꂼ��ʍ��W����T���v��
	int2 offR = clamp(srcPos + int2(rgbShift), int2(0, 0), int2(width - 1, height - 1));
	int2 offB = clamp(srcPos + int2(-rgbShift), int2(0, 0), int2(width - 1, height - 1));

	float3 col;
	col.r = gRenderTexture.Load(int3(offR, 0)).r;
	col.g = srcColor.g;
	col.b = gRenderTexture.Load(int3(offB, 0)).b;

	// �z���C�g�m�C�Y�I�[�o�[���C
	float noise = gNoiseTexture.SampleLevel(gSampler, uv * 512.0f, 0.0f).r;
	col = lerp(col, float3(noise, noise, noise), gMaterial.noiseStrength * gMaterial.intensity);

	// �ŏI�I�ȐF���o��
	gOutputTexture[DTid.xy] = float4(col, 1.0f);
}
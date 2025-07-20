//============================================================================
//	include
//============================================================================

#include "Skybox.hlsli"

//============================================================================
//	Output
//============================================================================

struct PSOutput {
	
	float4 color : SV_TARGET0;
};

//============================================================================
//	CBuffer
//============================================================================

cbuffer Material : register(b0) {
	
	float4 color;
	uint textureIndex;
	float4x4 uvTransform;
};

//============================================================================
//	texture Sampler
//============================================================================

TextureCube<float4> gTextures[] : register(t0, space0);
SamplerState gSampler : register(s0);

//============================================================================
//	Main
//============================================================================
PSOutput main(VSOutput input) {

	PSOutput output;

	float3 transformUV = mul(uvTransform, float4(input.texcoord, 1.0f)).xyz;
	float4 textureColor = gTextures[textureIndex].Sample(gSampler, transformUV);
	output.color = textureColor * color;
	
	return output;
}
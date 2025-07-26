//============================================================================
//	include
//============================================================================

#include "../../Common/ParticleCommonSturctures.hlsli"
#include "../../Common/ParticleOutput.hlsli"

//============================================================================
//	Output
//============================================================================

struct PSOutput {
	
	float4 color : SV_TARGET0;
};

//============================================================================
//	StructuredBuffer
//============================================================================

StructuredBuffer<Material> gMaterials : register(t0);

//============================================================================
//	texture Sampler
//============================================================================

Texture2D<float4> gTexture : register(t1);
SamplerState gSampler : register(s0);

//============================================================================
//	Main
//============================================================================
PSOutput main(MSOutput input) {
	
	PSOutput output;
	
	// instanceIdÅApixelÇ≤Ç∆ÇÃèàóù
	uint id = input.instanceID;
	Material material = gMaterials[id];
	
	// uv
	float4 textureColor = gTexture.Sample(gSampler, input.texcoord);
	
	// êF
	output.color.rgb = material.color.rgb * textureColor.rgb;
	// Éøíl
	output.color.a = material.color.a * textureColor.a;
	
	return output;
}
//============================================================================
//	include
//============================================================================

#include "EffectMesh.hlsli"

//============================================================================
//	Output
//============================================================================

struct PSOutput {
	
	float4 color : SV_TARGET0;
};

//============================================================================
//	structure
//============================================================================

struct Material {
	
	// color
	float4 color;
};

//============================================================================
//	buffer
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
	
	// instanceId�Apixel���Ƃ̏���
	uint id = input.instanceID;
	Material material = gMaterials[id];
	
	// uv
	float4 textureColor = gTexture.Sample(gSampler, input.texcoord);
	
	// �F
	output.color.rgb = material.color.rgb * textureColor.rgb;
	// ���l
	output.color.a = material.color.a * textureColor.a;
	
	return output;
}
//============================================================================
//	include
//============================================================================

#include "NewEffectMeshFunction.hlsli"

//============================================================================
//	Output
//============================================================================

struct PSOutput {
	
	float4 color : SV_TARGET0;
};

//============================================================================
//	Main
//============================================================================
PSOutput main(MSOutput input) {
	
	PSOutput output;
	
	// instanceIdÅApixelÇ≤Ç∆ÇÃèàóù
	uint id = input.instanceID;
	Material material = gMaterials[id];
	
	// uv
	float4 transformUV = mul(float4(input.texcoord, 0.0f, 1.0f), material.uvTransform);
	float4 textureColor = gTexture.Sample(gSampler, transformUV.xy);
	
	// êF
	output.color.rgb = material.color.rgb * textureColor.rgb;
	// Éøíl
	output.color.a = material.color.a * textureColor.a;
	
	return output;
}
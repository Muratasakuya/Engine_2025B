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
	
	// instanceId�Apixel���Ƃ̏���
	uint id = input.instanceID;
	Material material = gMaterials[id];
	
	// uv
	float4 transformUV = mul(float4(input.texcoord, 0.0f, 1.0f), material.uvTransform);
	float4 textureColor = gTexture.Sample(gSampler, transformUV.xy);
	
	// �F
	output.color.rgb = material.color.rgb * textureColor.rgb;
	// ���l
	output.color.a = material.color.a * textureColor.a;
	
	return output;
}
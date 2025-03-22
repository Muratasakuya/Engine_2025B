//============================================================================
//	include
//============================================================================

#include "Object2D.hlsli"

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
	float4x4 uvTransform;
};

//============================================================================
//	Texture Sampler
//============================================================================

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

//============================================================================
//	Main
//============================================================================
PSOutput main(VSOutput input) {
	
	PSOutput output;
	
	float4 transformUV = mul(float4(input.texcoord, 0.0f, 1.0f), uvTransform);
	float4 textureColor = gTexture.Sample(gSampler, transformUV.xy);
	
	if (textureColor.a <= 0.4f) {
		discard;
	}

	output.color.rgb = color.rgb * textureColor.rgb;
	output.color.a = color.a * textureColor.a;
	
	if (output.color.a == 0.0f) {
		discard;
	}
	
	return output;
}
//============================================================================
//	include
//============================================================================

#include "EffectMesh.hlsli"

//============================================================================
//	buffer
//============================================================================

cbuffer Material : register(b0) {

	float4 color;
	uint textureIndex;
	uint useVertexColor;
	float alphaReference;
	float emissiveIntensity;
	float3 emissionColor;
	float4x4 uvTransform;
};

//============================================================================
//	texture Sampler
//============================================================================

Texture2D<float4> gTextures[] : register(t0, space0);
SamplerState gSampler : register(s0);

//============================================================================
//	functions
//============================================================================

float4 GetDiffuseColor(float4 transformUV, MSOutput input) {
	
	// texture
	float4 diffuseColor = gTextures[textureIndex].Sample(gSampler, transformUV.xy);
	
	return diffuseColor;
}
//============================================================================
//	include
//============================================================================

#include "EffectMesh.hlsli"

//============================================================================
//	structure
//============================================================================

struct Material {

	// color
	float4 color;
	
	// texture
	uint textureIndex;
	uint noiseTextureIndex;
	
	// flags
	uint useNoiseTexture;
	uint useVertexColor;
	
	// reference
	float textureAlphaReference;
	float noiseTextureAlphaReference;
	
	// emission
	float emissiveIntensity;
	float3 emissionColor;
	
	// uv
	float4x4 uvTransform;
};

//============================================================================
//	buffer
//============================================================================

StructuredBuffer<Material> gMaterials : register(t0);

//============================================================================
//	texture Sampler
//============================================================================

Texture2D<float4> gTextures[] : register(t1, space0);
SamplerState gSampler : register(s0);

//============================================================================
//	functions
//============================================================================

float4 GetTextureColor(uint id, MSOutput input) {
	
	// uv
	float4 transformUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterials[id].uvTransform);
	// texture
	float4 diffuseColor = gTextures[gMaterials[id].textureIndex].Sample(gSampler, transformUV.xy);
	return diffuseColor;
}

uint IsNoiseTextureDiscard(uint id, MSOutput input) {
	
	// discard == false
	if (gMaterials[id].useNoiseTexture == 0) {
		return 0;
	}
	
	// getNoiseTextureAlpha
	float noiseAlpha = gTextures[gMaterials[id].noiseTextureIndex].Sample(gSampler, input.texcoord).a;
	
	// discardCheck
	if (noiseAlpha < gMaterials[id].noiseTextureAlphaReference) {
		
		// discard == true
		return 1;
	}
	// discard == false
	return 0;
}
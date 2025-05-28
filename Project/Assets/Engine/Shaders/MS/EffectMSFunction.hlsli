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
	
	// noise
	float edgeSize;
	float4 edgeColor;
	float edgeEmissiveIntensity;
	float3 edgeEmissionColor;
	
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

float4 GetTextureColor(uint id, MSOutput input, float4 transformUV) {
	
	// texture
	float4 textureColor = gTextures[gMaterials[id].textureIndex].Sample(gSampler, transformUV.xy);
	return textureColor;
}

bool ApplyNoiseDiscardAndEdge(uint id, float4 transformUV, out float4 outColor) {
	
	Material material = gMaterials[id];
	
	float alphaNoise = gTextures[material.noiseTextureIndex].Sample(gSampler, transformUV.xy).r;
	
	float dist = alphaNoise - material.noiseTextureAlphaReference;
	bool shouldDiscard = dist < 0.0f;
	
	float grad = fwidth(alphaNoise);
	float band = grad * material.edgeSize;
	
	float edgeMask = smoothstep(-band, -band * 0.4f, dist) * (1.0f - smoothstep(band * 0.4f, band, dist));
	
	// this edge
	if (edgeMask > 0.0f) {
		
		outColor = material.edgeColor;
		return true;
	}
	
	// discard == true
	if (shouldDiscard) {
		
		discard;
	}
	
	// discard == false
	return false;
}
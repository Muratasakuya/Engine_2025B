//============================================================================
//	include
//============================================================================

#include "InstancingObjectShadowDepth.hlsli"

//============================================================================
//	Input
//============================================================================

struct VSInput {

	float3 position : POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
};

//============================================================================
//	CBuffer
//============================================================================

cbuffer ShadowLight : register(b0) {
	
	float4x4 lightViewProjection;
};

//============================================================================
//	StructuredBuffer
//============================================================================

struct TransformationMatrix {

	float4x4 world;
	float4x4 worldInverseTranspose;
};

StructuredBuffer<TransformationMatrix> gMatrices : register(t0);

//============================================================================
//	Main
//============================================================================
VSOutput main(VSInput input, uint32_t instanceId : SV_InstanceID) {
	
	VSOutput output;
	float4 worldPos = float4(input.position, 1.0f);
	output.position = mul(mul(worldPos, gMatrices[instanceId].world), lightViewProjection);
	
	return output;
}
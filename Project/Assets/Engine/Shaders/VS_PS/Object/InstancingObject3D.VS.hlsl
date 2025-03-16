//============================================================================
//	include
//============================================================================

#include "InstancingObject3D.hlsli"

//============================================================================
//	Input
//============================================================================

struct VSInput {

	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL0;
};

//============================================================================
//	CBuffer
//============================================================================

cbuffer CameraData : register(b0) {
	
	float4x4 viewProjection;
};


cbuffer ShadowLight : register(b1) {
	
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

	float4x4 wvp = mul(gMatrices[instanceId].world, viewProjection);
	output.position = mul(input.position, wvp);
	output.texcoord = input.texcoord;
	output.normal = normalize(mul(input.normal, (float3x3) gMatrices[instanceId].worldInverseTranspose));
	
	output.worldPosition = mul(input.position, gMatrices[instanceId].world).xyz;
	
	float4 worldPos = float4(output.worldPosition, 1.0f);
	output.positionInLVP = mul(worldPos, lightViewProjection);
	
	// instanceIdÅAPixelÇ≤Ç∆ÇÃèàóù
	output.instanceID = instanceId;

	return output;
}
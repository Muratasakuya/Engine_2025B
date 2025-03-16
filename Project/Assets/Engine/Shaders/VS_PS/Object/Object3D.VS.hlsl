//============================================================================
//	include
//============================================================================

#include "Object3D.hlsli"

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

cbuffer TransformationMatrix : register(b0) {
	
	float4x4 world;
	float4x4 worldInverseTranspose;
};

cbuffer CameraData : register(b1) {
	
	float4x4 viewProjection;
};

cbuffer ShadowLight : register(b2) {
	
	float4x4 lightViewProjection;
};

//============================================================================
//	Main
//============================================================================
VSOutput main(VSInput input) {

	VSOutput output;

	float4x4 wvp = mul(world, viewProjection);
	output.position = mul(input.position, wvp);
	output.texcoord = input.texcoord;
	output.normal = normalize(mul(input.normal, (float3x3) worldInverseTranspose));
	
	output.worldPosition = mul(input.position, world).xyz;
	
	float4 worldPos = float4(output.worldPosition, 1.0f);
	output.positionInLVP = mul(worldPos, lightViewProjection);

	return output;
}
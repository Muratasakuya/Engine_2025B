#include "ShadowDepth.hlsli"

struct LightMatrix {
	
	float4x4 lightViewProjection;
};

struct TransformationMatrix {

	float4x4 World;
	float4x4 WVP;
	float4x4 WorldInverseTranspose;
};

struct VertexShaderInput {
	
	float3 position : POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
};

ConstantBuffer<LightMatrix> gLightMatrix : register(b0);
StructuredBuffer<TransformationMatrix> gMatrices : register(t0);

VertexShaderOutput main(VertexShaderInput input, uint32_t instanceId : SV_InstanceID) {
	
	VertexShaderOutput output;
	float4 worldPos = float4(input.position, 1.0f);
	output.position = mul(mul(worldPos, gMatrices[instanceId].World), gLightMatrix.lightViewProjection);
	
	return output;
}
//============================================================================*/
//	include
//============================================================================*/

#include "InstancingObject3D.hlsli"

//============================================================================*/
//	InstancingObject3D VS
//============================================================================*/

struct TransformationMatrix {

	float4x4 world;
	float4x4 wvp;
	float4x4 worldInverseTranspose;
};

struct ShadowLight {
	
	float4x4 viewProjection;
};

struct VertexShaderInput {

	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL0;
};

StructuredBuffer<TransformationMatrix> gMatrices : register(t0);
ConstantBuffer<ShadowLight> gShadowLight : register(b0);

VertexShaderOutput main(VertexShaderInput input, uint32_t instanceId : SV_InstanceID) {

	VertexShaderOutput output;

	output.position = mul(input.position, gMatrices[instanceId].wvp);
	output.texcoord = input.texcoord;
	output.normal = normalize(mul(input.normal, (float3x3) gMatrices[instanceId].worldInverseTranspose));
	
	output.worldPosition = mul(input.position, gMatrices[instanceId].world).xyz;
	
	float4 worldPos = float4(output.worldPosition, 1.0f);
	output.positionInLVP = mul(worldPos, gShadowLight.viewProjection);
	
	// instanceIdÅAPixelÇ≤Ç∆ÇÃèàóù
	output.instanceID = instanceId;

	return output;
}
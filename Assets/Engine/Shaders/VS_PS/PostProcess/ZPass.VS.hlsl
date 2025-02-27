//============================================================================*/
//	include
//============================================================================*/

#include "ZPass.hlsli"

//============================================================================*/
//	ZPass VS
//============================================================================*/

struct TransformationMatrix {

	float4x4 world;
	float4x4 worldInverseTranspose;
};

struct LightMatrix {
	
	float4x4 lightViewProjection;
};

struct VertexShaderInput {
	
	float3 position : POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
};

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);
ConstantBuffer<LightMatrix> gLightMatrix : register(b1);

VertexShaderOutput main(VertexShaderInput input) {
	
	VertexShaderOutput output;
	float4 worldPos = float4(input.position, 1.0f);
	output.position = mul(mul(worldPos, gTransformationMatrix.world), gLightMatrix.lightViewProjection);
	
	return output;
}
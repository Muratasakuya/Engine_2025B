//============================================================================*/
//	include
//============================================================================*/

#include "Object3D.hlsli"

//============================================================================*/
//	Object3D VS
//============================================================================*/

struct TransformationMatrix {

	float4x4 world;
	float4x4 worldInverseTranspose;
};

struct CameraData {
	
	float4x4 viewProjection;
};

struct ShadowLight {
	
	float4x4 lightViewProjection;
};

struct VertexShaderInput {

	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL0;
};

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);
ConstantBuffer<CameraData> gCameraData : register(b1);
ConstantBuffer<ShadowLight> gShadowLight : register(b2);

VertexShaderOutput main(VertexShaderInput input) {

	VertexShaderOutput output;

	float4x4 wvp = mul(gTransformationMatrix.world, gCameraData.viewProjection);
	output.position = mul(input.position, wvp);
	output.texcoord = input.texcoord;
	output.normal = normalize(mul(input.normal, (float3x3) gTransformationMatrix.worldInverseTranspose));
	
	output.worldPosition = mul(input.position, gTransformationMatrix.world).xyz;
	
	float4 worldPos = float4(output.worldPosition, 1.0f);
	output.positionInLVP = mul(worldPos, gShadowLight.lightViewProjection);

	return output;
}
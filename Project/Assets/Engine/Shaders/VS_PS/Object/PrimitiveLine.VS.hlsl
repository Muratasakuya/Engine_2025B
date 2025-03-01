#include "PrimitiveLine.hlsli"

/*===========================================================
                   PrimitiveLine VS Shader
===========================================================*/

struct Camera {
	
	float4x4 viewProjectionMatrix;
};
struct VertexShaderInput {

	float4 position : POSITION0;
	float4 color : COLOR0;
};

ConstantBuffer<Camera> gCamera : register(b0);

VertexShaderOutput main(VertexShaderInput input) {
	
	VertexShaderOutput output;
	output.position = mul(input.position, gCamera.viewProjectionMatrix);
	output.color = input.color;

	return output;
}
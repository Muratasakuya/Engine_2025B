#include "Object2D.hlsli"

struct TransformationMatrix {
	
	float4x4 WVP;
};

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

struct VertexShaderInput {
	
	float2 position : POSITION0;
	float2 texcoord : TEXCOORD0;
};

VertexShaderOutput main(VertexShaderInput input) {
	
	VertexShaderOutput output;
	output.texcoord = input.texcoord;
	output.position = mul(float4(input.position, 0.0f, 1.0f), gTransformationMatrix.WVP);
    
	return output;
}
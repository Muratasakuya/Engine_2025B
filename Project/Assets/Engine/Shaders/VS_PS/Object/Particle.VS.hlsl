#include "Particle.hlsli"

/*===========================================================
                     Particle VS Shader
===========================================================*/

struct ParticleForGPU {
	
	float4x4 World;
	float4x4 WVP;
	float4 color;
};

struct VertexShaderInput {
	
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
};

StructuredBuffer<ParticleForGPU> gParticle : register(t0);

VertexShaderOutput main(VertexShaderInput input, uint32_t instanceId : SV_InstanceID) {
	
	VertexShaderOutput output;
	output.position = mul(input.position, gParticle[instanceId].WVP);
	output.texcoord = input.texcoord;
	output.color = gParticle[instanceId].color;

	return output;
}
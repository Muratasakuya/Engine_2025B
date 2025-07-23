//============================================================================
//	include
//============================================================================

#include "../../ParticleCommonSturctures.hlsli"
#include "../../ParticleEmitterStructures.hlsli"
#include "../../../../../../Engine/Effect/Particle/ParticleConfig.h"

#include "../../../Math/Math.hlsli"

//============================================================================
//	CBuffer
//============================================================================

// Œ`ó
struct EmitterBox {
	
	float3 size;
	float3 translation;
	float4x4 rotationMatrix;
};

ConstantBuffer<EmitterCommon> gEmitterCommon : register(b0);
ConstantBuffer<EmitterBox> gEmitterBox : register(b1);
ConstantBuffer<PerFrame> gPerFrame : register(b2);

//============================================================================
//	RWStructuredBuffer
//============================================================================

RWStructuredBuffer<Particle> gParticles : register(u0);
RWStructuredBuffer<Transform> gTransform : register(u1);
RWStructuredBuffer<Material> gMaterials : register(u2);

RWStructuredBuffer<int> gFreeListIndex : register(u3);
RWStructuredBuffer<uint> gFreeList : register(u4);

//============================================================================
//	Functions
//============================================================================

float3 GetRandomPoint(RandomGenerator generator) {
	
	float3 local = (generator.Generate3D() - 0.5f) * gEmitterBox.size;
	return local;
}

//============================================================================
//	Main
//============================================================================
[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	
	// ”­¶‹–‰Â‚ª‰º‚è‚Ä‚¢‚È‚¯‚ê‚Îˆ—‚µ‚È‚¢
	if (gEmitterCommon.emit == 0) {
		return;
	}
	
	// —”¶¬
	RandomGenerator generator;
	generator.seed = (DTid + gPerFrame.time) * gPerFrame.time;

	// ”­¶•ª‚¾‚¯ˆ—
	for (uint i = 0; i < gEmitterCommon.count; ++i) {
		
		int freeListIndex;
		InterlockedAdd(gFreeListIndex[0], -1, freeListIndex);
		
		if (0 <= freeListIndex && freeListIndex < kMaxParticles) {
			
			uint particleIndex = gFreeList[freeListIndex];
			
			Particle particle;
			particle.currentTime = 0.0f;
			particle.lifeTime = gEmitterCommon.lifeTime;
			
			// Œü‚¢‚Ä‚¢‚é•ûŒü(+Z)•ûŒü‚É”ò‚Î‚·
			float3 worldForward = mul(float3(0.0f, 0.0f, 1.0f), (float3x3) gEmitterBox.rotationMatrix);
			particle.velocity = normalize(worldForward) * generator.Generate3D();
			
			Transform transform = (Transform) 0;
			transform.translation = gEmitterBox.translation + mul(
			float4(GetRandomPoint(generator), 1.0f), gEmitterBox.rotationMatrix).xyz;
			transform.scale = gEmitterCommon.scale;
			
			Material material = (Material) 0;
			material.color = gEmitterCommon.color;
			
			// ’l‚ğİ’è
			gParticles[particleIndex] = particle;
			gTransform[particleIndex] = transform;
			gMaterials[particleIndex] = material;
		} else {

			InterlockedAdd(gFreeListIndex[0], 1);
			break;
		}
	}
}
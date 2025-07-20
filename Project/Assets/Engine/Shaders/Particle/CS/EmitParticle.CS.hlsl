//============================================================================
//	include
//============================================================================

#include "../ParticleCommonSturctures.hlsli"
#include "../ParticleEmitterStructures.hlsli"
#include "../../../../../Engine/GPUEffect/Particle/ParticleConfig.h"

#include "../../Math/Math.hlsli"

//============================================================================
//	CBuffer
//============================================================================

ConstantBuffer<EmitterSphere> gEmitterSphere : register(b0);
ConstantBuffer<PerFrame> gPerFrame : register(b1);

//============================================================================
//	RWStructuredBuffer
//============================================================================

RWStructuredBuffer<Particle> gParticles : register(u0);
RWStructuredBuffer<int> gFreeListIndex : register(u1);
RWStructuredBuffer<uint> gFreeList : register(u2);

//============================================================================
//	Main
//============================================================================
[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	
	if (gEmitterSphere.emit == 0) {
		return;
	}
	
	RandomGenerator generator;
	generator.seed = (DTid + gPerFrame.time) * gPerFrame.time;
	
	for (uint i = 0; i < gEmitterSphere.count; ++i) {
		
		int freeListIndex;
		InterlockedAdd(gFreeListIndex[0], -1, freeListIndex);
		
		if (0 <= freeListIndex && freeListIndex < kMaxParticles) {
			
			uint particleIndex = gFreeList[freeListIndex];
			
			Particle particle;
			particle.translation = gEmitterSphere.translation + generator.Generate3D() * gEmitterSphere.radius;
			particle.scale = float3(1.0f, 1.0f, 1.0f);
			particle.velocity = normalize(generator.Generate3D() * 2.0f - 1.0f) * 3.0f;
			particle.lifeTime = 5.0f;
			particle.currentTime = 0.0f;
			
			gParticles[particleIndex] = particle;
		} else {

			InterlockedAdd(gFreeListIndex[0], 1);
			break;
		}
	}
}
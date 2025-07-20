//============================================================================
//	include
//============================================================================

#include "../ParticleCommonSturctures.hlsli"
#include "../../../../../Engine/GPUEffect/Particle/ParticleConfig.h"

//============================================================================
//	CBuffer
//============================================================================

ConstantBuffer<PerFrame> gPerFrame : register(b0);

//============================================================================
//	RWStructuredBuffer
//============================================================================

RWStructuredBuffer<Particle> gParticles : register(u0);
RWStructuredBuffer<int> gFreeListIndex : register(u1);
RWStructuredBuffer<uint> gFreeList : register(u2);

//============================================================================
//	Main
//============================================================================
[numthreads(1024, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	
	uint particleIndex = DTid.x;
	if (kMaxParticles <= particleIndex) {
		return;
	}
	
	Particle particle = gParticles[particleIndex];
	if (particle.currentTime < particle.lifeTime) {
		
		particle.translation += particle.velocity * gPerFrame.deltaTime;
		particle.currentTime += gPerFrame.deltaTime;

		gParticles[particleIndex] = particle;
		return;
	}

	gParticles[particleIndex] = (Particle) 0;

	int posPrev;
	InterlockedAdd(gFreeListIndex[0], 1, posPrev);
	uint writePos = posPrev + 1;

	if (writePos < kMaxParticles) {
		
		gFreeList[writePos] = particleIndex;
	} else {
		
		InterlockedAdd(gFreeListIndex[0], -1);
	}
}
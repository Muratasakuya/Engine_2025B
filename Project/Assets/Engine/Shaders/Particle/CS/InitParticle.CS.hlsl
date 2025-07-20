//============================================================================
//	include
//============================================================================

#include "../ParticleCommonSturctures.hlsli"
#include "../ParticleConfig.hlsli"

//============================================================================
//	RWStructuredBuffer
//============================================================================

RWStructuredBuffer<Particle> gParticles : register(u0);
RWStructuredBuffer<int> gFreeListIndex : register(u1);
RWStructuredBuffer<uint> gFreeList: register(u2);

//============================================================================
//	Main
//============================================================================
[numthreads(1024, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	
	uint particleIndex = DTid.x;
	if (particleIndex < kMaxParticles) {
		
		// ‘S‚Ä0–„‚ß
		gParticles[particleIndex] = (Particle) 0;
		gFreeList[particleIndex] = particleIndex;
	}
	if (particleIndex == 0) {
		
		gFreeListIndex[0] = kMaxParticles - 1;
	}
}
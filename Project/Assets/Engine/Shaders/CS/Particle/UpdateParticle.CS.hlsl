
static const uint kMaxParticles = 1024;

cbuffer PerFrame : register(b0) {
	
	float time;
	float deltaTime;
};

struct Particle {
	
	float3 translation;
	float3 scale;
	float lifeTime;
	float currentTime;
	float3 velocity;
};

RWStructuredBuffer<Particle> gParticles : register(u0);
RWStructuredBuffer<int> gFreeListIndex : register(u1);
RWStructuredBuffer<uint> gFreeList : register(u2);

[numthreads(1024, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	
	uint particleIndex = DTid.x;
	if (kMaxParticles <= particleIndex) {
		return;
	}
	
	Particle particle = gParticles[particleIndex];
	if (particle.currentTime < particle.lifeTime) {
		
		particle.translation += particle.velocity * deltaTime;
		particle.currentTime += deltaTime;

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
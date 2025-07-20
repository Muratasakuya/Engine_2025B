
static const uint kMaxParticles = 1024;

struct Particle {
	
	float3 translation;
	float3 scale;
	float lifeTime;
	float currentTime;
	float3 velocity;
};

RWStructuredBuffer<Particle> gParticles : register(u0);
RWStructuredBuffer<int> gFreeCounter : register(u1);

[numthreads(1024, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	
	uint particleIndex = DTid.x;
	if (particleIndex < kMaxParticles) {
		
		// ‘S‚Ä0–„‚ß
		gParticles[particleIndex] = (Particle) 0;
	}
	if (particleIndex == 0) {
		
		gFreeCounter[0] = 0;
	}
}
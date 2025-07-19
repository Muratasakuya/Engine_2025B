
static const uint kMaxParticles = 1024;

struct Particle {
	
	float3 translation;
	float3 scale;
	float lifeTime;
	float currentTime;
	float3 velocity;
};

RWStructuredBuffer<Particle> gParticles : register(u0);

[numthreads(1024, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	
	uint particleIndex = DTid.x;
	if (particleIndex < kMaxParticles) {
		
		// ‘S‚Ä0–„‚ß
		gParticles[particleIndex] = (Particle) 0;
		
		gParticles[particleIndex].scale = float3(4.0f, 4.0f, 4.0f);
	}
}
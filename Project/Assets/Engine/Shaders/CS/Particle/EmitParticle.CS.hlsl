
static const uint kMaxParticles = 1024;

cbuffer EmitterSphere : register(b0) {

	float3 translation;
	float radius;
	uint count;
	float frequency;
	float frequencyTime;
	uint emit;
};

cbuffer PerFrame : register(b1) {
	
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

float Rand3DTo1D(float3 value, float3 dotDir = float3(12.9898f, 78.233f, 37.719f)) {
	
	float3 smallValue = sin(value);
	float random = dot(smallValue, dotDir);
	random = frac(sin(random) * 143758.5453f);
	return random;
}
float3 Rand3DTo3D(float3 value) {
	
	return float3(
		Rand3DTo1D(value, float3(12.989, 78.233, 37.719)),
		Rand3DTo1D(value, float3(39.346, 11.135, 83.155)),
		Rand3DTo1D(value, float3(73.156, 52.235, 09.151))
	);
}

class RandomGenerator {
	
	float3 seed;
	float3 Generate3D() {
		
		seed = Rand3DTo3D(seed);
		return seed;
	}
	
	float Generate1D() {
		
		float result = Rand3DTo1D(seed);
		seed.x = result;
		return result;
	}
};

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	
	if (emit == 0) {
		return;
	}
	
	RandomGenerator generator;
	generator.seed = (DTid + time) * time;
	
	for (uint i = 0; i < count; ++i) {
		
		int freeListIndex;
		InterlockedAdd(gFreeListIndex[0], -1, freeListIndex);
		
		if (0 <= freeListIndex && freeListIndex < kMaxParticles) {
			
			uint particleIndex = gFreeList[freeListIndex];
			
			Particle particle;
			particle.translation = translation + generator.Generate3D() * radius;
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
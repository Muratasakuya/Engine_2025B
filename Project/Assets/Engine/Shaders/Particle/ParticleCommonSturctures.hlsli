//============================================================================
//	Structures
//============================================================================

struct Particle {
	
	float3 translation;
	float3 scale;
	float lifeTime;
	float currentTime;
	float3 velocity;
}; 

struct PerFrame {
	
	float time;
	float deltaTime;
};
//============================================================================
//	Structures
//============================================================================

struct Transform {
	
	float3 translation;
	float3 scale;
};

struct Material {
	
	float4 color;
};

struct Particle {
	
	float lifeTime;
	float currentTime;

	float3 velocity;
}; 

struct PerFrame {
	
	float time;
	float deltaTime;
};

struct PerView {
	
	float4x4 viewProjection;
	float4x4 billboardMatrix;
};
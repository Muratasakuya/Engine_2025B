//============================================================================
//	structure
//============================================================================

struct ShadowPayload {
	
	int isHit;
};

//============================================================================
//	buffer
//============================================================================

cbuffer Scene : register(b0) {
	
	// camera
	float3 cameraPos;
	
	// light
	float3 lightDirection;
};

RaytracingAccelerationStructure gSceneBVH : register(t0);
RWTexture2D<float> gShadowMask : register(u0);
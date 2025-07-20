//============================================================================
//	define
//============================================================================

#ifndef SHADOW_RAY_HLSLI
#define SHADOW_RAY_HLSLI

//============================================================================
//	structure
//============================================================================

// shadowRay
struct ShadowPayload {
	
	bool occluded;
};

struct MeshVertex {
	
	float4 position;
	float2 texcoord;
	float3 normal;
	float4 color;
	float3 tangent;
	float3 biNormal;
};

//============================================================================
//	buffer
//============================================================================

RaytracingAccelerationStructure gScene : register(t0);
StructuredBuffer<MeshVertex> gVertices : register(t1);
StructuredBuffer<uint> gIndices : register(t2);
RWTexture2D<float> gShadowMask : register(u0);

#endif // SHADOW_RAY_HLSLI
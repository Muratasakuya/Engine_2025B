//============================================================================
//	include
//============================================================================

#include "ShadowRay.hlsli"

//============================================================================
//	Closesthit
//============================================================================
[shader("closesthit")]
void ClosestHit(inout RadiancePayload payload, in BuiltInTriangleIntersectionAttributes attributes) {
	
	uint primitiveID = PrimitiveIndex();
	
	// indexæ“¾
	uint3 index = uint3(
		gIndices[primitiveID * 3 + 0],
		gIndices[primitiveID * 3 + 1],
		gIndices[primitiveID * 3 + 2]);
	
	// ’¸“_‘®«æ“¾
	MeshVertex v0 = gVertices[index.x];
	MeshVertex v1 = gVertices[index.y];
	MeshVertex v2 = gVertices[index.z];
	
	float3 p0 = v0.position.xyz;
	float3 p1 = v1.position.xyz;
	float3 p2 = v2.position.xyz;

	float3 n0 = v0.normal;
	float3 n1 = v1.normal;
	float3 n2 = v2.normal;
	
	// •âŠÔˆ—
	float3 bary = float3(1.0 - attributes.barycentrics.x - attributes.barycentrics.y,
		attributes.barycentrics.x, attributes.barycentrics.y);
	
	payload.worldPos = p0 * bary.x + p1 * bary.y + p2 * bary.z;
	payload.worldNormal = normalize(n0 * bary.x + n1 * bary.y + n2 * bary.z);
	payload.hitT = RayTCurrent();
}
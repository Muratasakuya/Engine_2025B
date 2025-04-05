//============================================================================
//	include
//============================================================================

#include "Triangle.hlsli"

//============================================================================
//	Input
//============================================================================

struct MSInput {
	
	float4 position;
	float4 color;
};

//============================================================================
//	CBuffer structure
//============================================================================

// Transform
cbuffer Transform : register(b0) {
	
	float4x4 world;
	float4x4 viewProjection;
};

//============================================================================
//	StructuredBuffer
//============================================================================

StructuredBuffer<MSInput> gVertices : register(t0);
StructuredBuffer<uint3> gIndices : register(t1);

//============================================================================
//	Main
//============================================================================
[numthreads(64, 1, 1)]
[outputtopology("triangle")]
void main(uint groupIndex : SV_GroupIndex,
out vertices MSOutput verts[3], out indices uint3 tris[1]) {
	
	// スレッドグループの頂点と、プリミティブの数を設定
	SetMeshOutputCounts(3, 1);
	
	// 頂点番号を設定
	if (groupIndex < 1) {
		
		tris[groupIndex] = gIndices[groupIndex];
	}
	
	// 頂点データを設定
	if (groupIndex < 3) {
		
		MSOutput output = (MSOutput) 0;

		float4 worldPos = mul(world, gVertices[groupIndex].position);
		float4 projPos = mul(viewProjection, worldPos);

		output.position = projPos;
		output.color = gVertices[groupIndex].color;

		verts[groupIndex] = output;
	}
}
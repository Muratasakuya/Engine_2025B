#include "StaticMesh.hlsli"

/*===========================================================
                    StaticMesh VS Shader
===========================================================*/

struct StaticMesh {
	
	float4x4 WVP;
	float4 color;
};

struct TriangleData {
	
	float4 vertex0;
	float4 vertex1;
	float4 vertex2;
};

struct VertexShaderInput {
	
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
};

StructuredBuffer<StaticMesh> gMesh : register(t0);
StructuredBuffer<TriangleData> gTriangleData : register(t1);

VertexShaderOutput main(VertexShaderInput input, uint vertexID : SV_VertexID, uint instanceId : SV_InstanceID) {
	VertexShaderOutput output;

	// インスタンスIDに基づいて三角形データを取得
	TriangleData tri = gTriangleData[instanceId];
	
	 // 頂点を選択
	float4 position;
	if (vertexID == 0)
		position = tri.vertex0; else if (vertexID == 1)
		position = tri.vertex1; else if (vertexID == 2)
		position = tri.vertex2;

	position = mul(position, gMesh[instanceId].WVP);

	float4 color = gMesh[instanceId].color;

	output.position = position;
	output.texcoord = input.texcoord;
	output.color = color;

	return output;
}
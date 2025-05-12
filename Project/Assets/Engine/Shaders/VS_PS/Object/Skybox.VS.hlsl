//============================================================================
//	include
//============================================================================

#include "Skybox.hlsli"

//============================================================================
//	Input
//============================================================================

struct VSInput {

	float4 position : POSITION0;
};

//============================================================================
//	CBuffer
//============================================================================

cbuffer MatrixData : register(b0) {
	
	float4x4 world;
};

cbuffer CameraData : register(b1) {
	
	float4x4 viewProjection;
};

//============================================================================
//	Main
//============================================================================
VSOutput main(VSInput input) {
	
	VSOutput output;
	
	// スクリーン座標に変換
	float4x4 wvp = mul(world, viewProjection);
	float4 pos = mul(input.position, wvp);
	pos.z = pos.w;
	output.position = pos;
	// texcoordにinputPosをそのまま入れる
	output.texcoord = input.position.xyz;

	return output;
}
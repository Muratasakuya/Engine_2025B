//============================================================================
//	Output
//============================================================================

struct MSOutput {
	
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL0;
	float4 meshletColor : COLOR0;
	float4 vertexColor : COLOR1;
};
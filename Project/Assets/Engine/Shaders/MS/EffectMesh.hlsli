//============================================================================
//	Output
//============================================================================

struct MSOutput {
	
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD0;
	float4 vertexColor : COLOR1;
	uint instanceID : INSTANCEID;
};
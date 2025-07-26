//============================================================================
//	Output
//============================================================================

struct MSOutput {
	
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD0;
	uint instanceID : INSTANCEID;
};
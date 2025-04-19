//============================================================================
//	Output
//============================================================================

struct MSOutput {
	
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL0;
	float3 worldPosition : POSITION0;
	float4 positionInLVP : TEXCOORD1;
	float4 meshletColor : COLOR0;
	float4 vertexColor : COLOR1;
	float3 tangent : TANGENT;
	float3 biNormal : BINORMAL;
	uint instanceID : INSTANCEID;
};

//============================================================================
//	Structures
//============================================================================

struct DirectionalLight {

	float4 color;
	float3 direction;
	float intensity;
};

struct PointLight {
	
	float4 color;
	float3 pos;
	float intensity;
	float radius;
	float decay;
};

struct SpotLight {

	float4 color;
	float3 pos;
	float intensity;
	float3 direction;
	float distance;
	float decay;
	float cosAngle;
	float cosFalloffStart;
};
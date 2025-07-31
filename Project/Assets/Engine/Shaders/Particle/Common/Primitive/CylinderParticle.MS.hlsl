//============================================================================
//	include
//============================================================================

#include "../ParticleOutput.hlsli"
#include "../ParticleCommonSturctures.hlsli"

#include "../../../Math/Math.hlsli"

//============================================================================
// Constant
//============================================================================

#ifndef CYL_MAX_DIVIDE
#define CYL_MAX_DIVIDE 32
#endif

static const uint CYL_MAX_VERTS = (CYL_MAX_DIVIDE + 1) * 2;
static const uint CYL_MAX_TRIS = CYL_MAX_DIVIDE * 2;

//============================================================================
//	CBuffer
//============================================================================

ConstantBuffer<PerView> gPerView : register(b0);

//============================================================================
//	StructuredBuffer
//============================================================================

struct Cylinder {
	
	float topRadius;
	float bottomRadius;
	float height;
	uint divide;
};

StructuredBuffer<Cylinder> gCylinders : register(t0);
StructuredBuffer<Transform> gTransform : register(t1);

//============================================================================
//	Main
//============================================================================
[numthreads(1, 1, 1)]
[outputtopology("triangle")]
void main(uint groupThreadId : SV_GroupThreadID, uint groupId : SV_GroupID,
out vertices MSOutput verts[CYL_MAX_VERTS], out indices uint3 polys[CYL_MAX_TRIS]) {
	
	// dispatchMesh�ł�1�����O���[�vID
	uint instanceIndex = groupId;
	// �o�b�t�@�A�N�Z�X
	Cylinder cylinder = gCylinders[instanceIndex];
	Transform transform = gTransform[instanceIndex];
	
	// �����A����𒴂��Ȃ��悤�ɂ���
	uint divide = clamp(cylinder.divide, 3, (uint) CYL_MAX_DIVIDE);
	SetMeshOutputCounts((divide + 1) * 2, divide * 2);
	
	// world�s����쐬
	float4x4 worldMatrix = MakeWorldMatrix(transform, gPerView.billboardMatrix, gPerView.cameraPos);
	worldMatrix[3].xyz = transform.translation;

	// �e�̐ݒ�
	worldMatrix = SetParent(transform, worldMatrix);
	
	// �s��v�Z
	float4x4 wvp = mul(worldMatrix, gPerView.viewProjection);
	// �p�x�X�e�b�v
	float angleStep = PI2 / (float) divide;
	
	// cylinder�𐶐�
	// ���_
	[loop]
	for (uint i = 0; i <= divide; ++i) {

		float a = angleStep * (float) i;
		float s = sin(a);
		float c0 = cos(a);

		float3 pTop = float3(s * cylinder.topRadius, cylinder.height, c0 * cylinder.topRadius);
		float3 pBot = float3(s * cylinder.bottomRadius, 0.0f, c0 * cylinder.bottomRadius);

		float u = (float) i / (float) divide;

		MSOutput vertex;
		vertex.position = mul(float4(pTop, 1), wvp);
		vertex.texcoord = float2(-u, 0.0f);
		verts[i] = vertex;

		vertex.position = mul(float4(pBot, 1), wvp);
		vertex.texcoord = float2(-u, 1.0f);
		verts[divide + 1 + i] = vertex;
		
		vertex.instanceID = instanceIndex;
	}

	// �C���f�b�N�X
	[loop]
	for (uint i = 0; i < divide; ++i) {
		
		uint t0 = i;
		uint t1 = i + 1;
		uint b0 = (divide + 1) + i;
		uint b1 = (divide + 1) + i + 1;
		
		polys[i * 2 + 0] = uint3(t0, b0, t1);
		polys[i * 2 + 1] = uint3(b0, b1, t1);
	}
}
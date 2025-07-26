//============================================================================
//	include
//============================================================================

#include "../ParticleOutput.hlsli"
#include "../ParticleCommonSturctures.hlsli"

#include "../../../Math/Math.hlsli"

//============================================================================
// Constant
//============================================================================

#ifndef RING_MAX_DIVIDE
#define RING_MAX_DIVIDE 32
#endif

static const uint RING_MAX_VERTS = RING_MAX_DIVIDE * 4;
static const uint RING_MAX_TRIS = RING_MAX_DIVIDE * 2;

//============================================================================
//	CBuffer
//============================================================================

ConstantBuffer<PerView> gPerView : register(b0);

//============================================================================
//	StructuredBuffer
//============================================================================

struct Ring {
	
	float outerRadius;
	float innerRadius;
	uint divide;
};

StructuredBuffer<Ring> gRings : register(t0);
StructuredBuffer<Transform> gTransform : register(t1);

//============================================================================
//	Main
//============================================================================
[numthreads(1, 1, 1)]
[outputtopology("triangle")]
void main(uint groupThreadId : SV_GroupThreadID, uint groupId : SV_GroupID,
out vertices MSOutput verts[RING_MAX_VERTS], out indices uint3 polys[RING_MAX_TRIS]) {
	
	// dispatchMesh�ł�1�����O���[�vID
	uint instanceIndex = groupId;
	// �o�b�t�@�A�N�Z�X
	Ring ring = gRings[instanceIndex];
	Transform transform = gTransform[instanceIndex];
	
	// �����A����𒴂��Ȃ��悤�ɂ���
	uint divide = max(3, min(ring.divide, (uint) RING_MAX_DIVIDE));
	
	// ���_���A�o�͎O�p�`��
	uint vertexCount = divide * 4;
	uint primitiveCount = divide * 2;
	SetMeshOutputCounts(vertexCount, primitiveCount);
	
	// world�s����쐬
	float4x4 worldMatrix = MakeWorldMatrix(transform, gPerView.billboardMatrix, gPerView.cameraPos);
	worldMatrix[3].xyz = transform.translation;

	// �s��v�Z
	float4x4 wvp = mul(worldMatrix, gPerView.viewProjection);
	 // �p�x�X�e�b�v
	float angleStep = PI2 / (float) divide;
	
	// ring�𐶐�
	[loop]
	for (uint i = 0; i < divide; ++i) {
		
		float a0 = angleStep * (float) i;
		float a1 = angleStep * (float) (i + 1);

		// sin/cos
		float s0 = sin(a0);
		float c0 = cos(a0);
		float s1 = sin(a1);
		float c1 = cos(a1);

		// ���[�J���ʒu�AXY���ʁA+Z������
		// 4���_����: outer0, outer1, inner0, inner1
		float3 p0 = float3(s0 * ring.outerRadius, c0 * ring.outerRadius, 0.0f);
		float3 p1 = float3(s1 * ring.outerRadius, c1 * ring.outerRadius, 0.0f);
		float3 p2 = float3(s0 * ring.innerRadius, c0 * ring.innerRadius, 0.0f);
		float3 p3 = float3(s1 * ring.innerRadius, c1 * ring.innerRadius, 0.0f);

		// U: ���W�͊p�x�ɔ��
		float u0 = (float) i / (float) divide;
		float u1 = (float) (i + 1) / (float) divide;
		// V: outer=0, inner=1
		float2 uv0 = float2(u0, 0.0f);
		float2 uv1 = float2(u1, 0.0f);
		float2 uv2 = float2(u0, 1.0f);
		float2 uv3 = float2(u1, 1.0f);

		// �������݊J�n�C���f�b�N�X
		uint vBase = i * 4;
		uint tBase = i * 2;

		// Transforms
		float4 pos0 = mul(float4(p0, 1), wvp);
		float4 pos1 = mul(float4(p1, 1), wvp);
		float4 pos2 = mul(float4(p2, 1), wvp);
		float4 pos3 = mul(float4(p3, 1), wvp);

		// ���_�o��
		MSOutput vertex;
		vertex.position = pos0;
		vertex.texcoord = uv0;
		verts[vBase + 0] = vertex;
		vertex.position = pos1;
		vertex.texcoord = uv1;
		verts[vBase + 1] = vertex;
		vertex.position = pos2;
		vertex.texcoord = uv2;
		verts[vBase + 2] = vertex;
		vertex.position = pos3;
		vertex.texcoord = uv3;
		verts[vBase + 3] = vertex;
		
		vertex.instanceID = instanceIndex;

		// ��
		// outer0, outer1, inner0
		polys[tBase + 0] = uint3(vBase + 0, vBase + 1, vBase + 2);
		// inner0, outer1, inner1
		polys[tBase + 1] = uint3(vBase + 2, vBase + 1, vBase + 3);
	}
}
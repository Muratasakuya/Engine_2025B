//============================================================================
//	include
//============================================================================

#include "../ParticleOutput.hlsli"
#include "../ParticleCommonSturctures.hlsli"

#include "../../../Math/Math.hlsli"

//============================================================================
// Constant
//============================================================================

#ifndef CRESCENT_MAX_DIVIDE
#define CRESCENT_MAX_DIVIDE 32
#endif

//============================================================================
//	CBuffer
//============================================================================

ConstantBuffer<PerView> gPerView : register(b0);

//============================================================================
//	StructuredBuffer
//============================================================================

struct Crescent {
	
	float outerRadius;
	float innerRadius;
	float startAngle;
	float endAngle;
	float lattice;
	float2 pivot;
	uint divide;
	uint uvMode;
};

StructuredBuffer<Crescent> gCrescents : register(t0);
StructuredBuffer<Transform> gTransform : register(t1);

//============================================================================
//	Main
//============================================================================
[numthreads(CRESCENT_MAX_DIVIDE * 2, 1, 1)]
[outputtopology("triangle")]
void main(uint groupThreadId : SV_GroupThreadID, uint groupId : SV_GroupID,
out vertices MSOutput verts[(CRESCENT_MAX_DIVIDE + 1) * 2], out indices uint3 polys[CRESCENT_MAX_DIVIDE * 2]) {
	
	// dispatchMesh�ł�1�����O���[�vID
	uint instanceIndex = groupId;
	// �o�b�t�@�A�N�Z�X
	Crescent crescent = gCrescents[instanceIndex];
	Transform transform = gTransform[instanceIndex];
	
	// ���_���A�o�͎O�p�`��
	uint vertCount = (crescent.divide + 1) * 2;
	uint triCount = crescent.divide * 2;
	SetMeshOutputCounts(vertCount, triCount);
	
	// �C���f�b�N�X��������
	if (groupThreadId < crescent.divide) {
		
		polys[groupThreadId] = uint3(groupThreadId * 2, groupThreadId * 2 + 1, groupThreadId * 2 + 2);
		polys[groupThreadId + crescent.divide] = uint3(groupThreadId * 2 + 2, groupThreadId * 2 + 1, groupThreadId * 2 + 3);
	}
	
	// ���_����
	if (groupThreadId < vertCount) {
		
		bool isOuter = (groupThreadId & 1) == 0;
		uint segIndex = groupThreadId >> 1;
		float t = segIndex / (float) crescent.divide;
		float angRad = lerp(crescent.startAngle, crescent.endAngle, t);

		// �[�Ō��݂�0.0f�ɂ��Đ�点��
		float thickness = crescent.outerRadius - crescent.innerRadius;
		float innerDynamic = crescent.outerRadius - thickness * sin(t * PI);
		
		// ���e�B�X�c��
		float latticeOff = (isOuter ? 1.0f : -1.0f) * crescent.lattice * 0.5f * sin(t * PI);
		float r = (isOuter ? crescent.outerRadius : innerDynamic) + latticeOff;
		
		float2 pos2 = float2(cos(angRad), sin(angRad)) * r;

		// �s�{�b�g�␳
		float2 halfExtent = float2(crescent.outerRadius, crescent.outerRadius);
		float2 pivotOff = lerp(-halfExtent, halfExtent, crescent.pivot);
		pos2 -= pivotOff;

		// world�s����쐬
		float4x4 worldMatrix = MakeWorldMatrix(transform, gPerView.billboardMatrix, gPerView.cameraPos);
		worldMatrix[3].xyz = transform.translation;
		
		// �e�̐ݒ�
		worldMatrix = SetParent(transform, worldMatrix);

		// �s��v�Z
		float4x4 wvp = mul(worldMatrix, gPerView.viewProjection);
		float4 pos = mul(float4(pos2.x, pos2.y, 0.0f, 1.0f), wvp);
		
		// ���_����ݒ�
		MSOutput vertex;
		vertex.position = pos;
		vertex.texcoord = (crescent.uvMode == 0) ?
		float2(t, isOuter ? 0.0f : 1.0f) : float2(isOuter ? 0.0f : 1.0f, t);
		vertex.instanceID = instanceIndex;

		verts[groupThreadId] = vertex;
	}
}
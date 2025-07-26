//============================================================================
//	include
//============================================================================

#include "../ParticleOutput.hlsli"
#include "../ParticleCommonSturctures.hlsli"

#include "../../../Math/Math.hlsli"

//============================================================================
//	CBuffer
//============================================================================

ConstantBuffer<PerView> gPerView : register(b0);

//============================================================================
//	StructuredBuffer
//============================================================================

struct Plane {
	
	float2 size;
};

StructuredBuffer<Plane> gPlanes : register(t0);
StructuredBuffer<Transform> gTransform : register(t1);

//============================================================================
//	Main
//============================================================================
[numthreads(4, 1, 1)]
[outputtopology("triangle")]
void main(uint groupThreadId : SV_GroupThreadID, uint groupId : SV_GroupID,
out vertices MSOutput verts[4], out indices uint3 polys[2]) {
	
	// dispatchMesh�ł�1�����O���[�vID
	uint instanceIndex = groupId;
	// �o�b�t�@�A�N�Z�X
	Plane plane = gPlanes[instanceIndex];
	Transform transform = gTransform[instanceIndex];
	
	// ���_��4,�o�͎O�p�`��2
	SetMeshOutputCounts(4, 2);
	
	// �O�p�`�C���f�b�N�X�͍ŏ���2�X���b�h�ŏ�������
	if (groupThreadId < 2) {

		const uint3 index[2] = { uint3(0, 1, 2), uint3(2, 1, 3) };
		polys[groupThreadId] = index[groupThreadId];
	}
	
	// plane�𐶐�
	if (groupThreadId < 4) {
		
		// �c�Ɖ��̔����̃T�C�Y
		float halfWidth = plane.size.x * 0.5f;
		float halfHeight = plane.size.y * 0.5f;

		float3 localPos;
		float2 uv = float2(0.0f, 0.0f);
		switch (groupThreadId) {
			// ����
			case 0:

				localPos = float3(-halfWidth, -halfHeight, 0.0f);
				uv = float2(0.0f, 1.0f);
				break;
			// ���E
			case 1:

				localPos = float3(halfWidth, -halfHeight, 0.0f);
				uv = float2(1.0f, 1.0f);
				break;
			// �㍶
			case 2:

				localPos = float3(-halfWidth, halfHeight, 0.0f);
				uv = float2(0.0f, 0.0f);
				break;
			// ��E
			default:

				localPos = float3(halfWidth, halfHeight, 0.0f);
				uv = float2(1.0f, 0.0f);
				break;
		}
		
		// world�s����쐬
		float4x4 worldMatrix = MakeWorldMatrix(transform, gPerView.billboardMatrix, gPerView.cameraPos);
		worldMatrix[3].xyz = transform.translation;

		// �s��v�Z
		float4x4 wvp = mul(worldMatrix, gPerView.viewProjection);
		float4 pos = mul(float4(localPos, 1.0f), wvp);

		// ���_����ݒ�
		MSOutput vertex;
		vertex.position = pos;
		vertex.texcoord = uv;
		vertex.instanceID = instanceIndex;
		
		verts[groupThreadId] = vertex;
	}
}
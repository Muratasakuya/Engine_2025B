
//============================================================================
//	CBuffer
//============================================================================

cbuffer SkinningInformation : register(b0) {
	
	uint numVertices;
	uint numBones;
};

//============================================================================
//	StructuredBuffer
//============================================================================

struct Vertex {
	
	float4 position;
	float2 texcoord;
	float3 normal;
	float4 color;
	float3 tangent;
	float3 biNormal;
};

struct Well {
	
	float4x4 skeletonSpaceMatrix;
	float4x4 skeletonSpaceInverseTransposeMatrix;
};

struct VertexInfluence {
	
	float4 weight;
	int4 index;
};

StructuredBuffer<Well> gMatrixPalette : register(t0);
StructuredBuffer<Vertex> gInputVertices : register(t1);
StructuredBuffer<VertexInfluence> gInfluences : register(t2);
// Skinning�v�Z��̒��_�f�[�^�A�����MS�ɓn��
RWStructuredBuffer<Vertex> gOutputVertices : register(u0);

//============================================================================
//	Functions
//============================================================================

// �ʒu�̕ϊ�
float4 CalPosition(Vertex input, VertexInfluence influence, uint boneOffset) {
	
	float4 position;
	
	position = mul(input.position, gMatrixPalette[influence.index.x + boneOffset].skeletonSpaceMatrix) * influence.weight.x;
	position += mul(input.position, gMatrixPalette[influence.index.y + boneOffset].skeletonSpaceMatrix) * influence.weight.y;
	position += mul(input.position, gMatrixPalette[influence.index.z + boneOffset].skeletonSpaceMatrix) * influence.weight.z;
	position += mul(input.position, gMatrixPalette[influence.index.w + boneOffset].skeletonSpaceMatrix) * influence.weight.w;
	position.w = 1.0f;
	
	return position;
}

// �@���̕ϊ�
float3 CalNormal(Vertex input, VertexInfluence influence, uint boneOffset) {
	
	float3 normal;
	
	normal = mul(input.normal, (float3x3) gMatrixPalette[influence.index.x + boneOffset].skeletonSpaceInverseTransposeMatrix) * influence.weight.x;
	normal += mul(input.normal, (float3x3) gMatrixPalette[influence.index.y + boneOffset].skeletonSpaceInverseTransposeMatrix) * influence.weight.y;
	normal += mul(input.normal, (float3x3) gMatrixPalette[influence.index.z + boneOffset].skeletonSpaceInverseTransposeMatrix) * influence.weight.z;
	normal += mul(input.normal, (float3x3) gMatrixPalette[influence.index.w + boneOffset].skeletonSpaceInverseTransposeMatrix) * influence.weight.w;
	// ���K�����Ė߂�
	normal = normalize(normal);
	
	return normal;
}

// �����x�N�g���̕ϊ�
float3 TransformDirection(float3 v, VertexInfluence influence, uint boneOffset) {
	
	float3 outputVector;
	
	outputVector = mul(v, (float3x3) gMatrixPalette[influence.index.x + boneOffset].skeletonSpaceInverseTransposeMatrix) * influence.weight.x;
	outputVector += mul(v, (float3x3) gMatrixPalette[influence.index.y + boneOffset].skeletonSpaceInverseTransposeMatrix) * influence.weight.y;
	outputVector += mul(v, (float3x3) gMatrixPalette[influence.index.z + boneOffset].skeletonSpaceInverseTransposeMatrix) * influence.weight.z;
	outputVector += mul(v, (float3x3) gMatrixPalette[influence.index.w + boneOffset].skeletonSpaceInverseTransposeMatrix) * influence.weight.w;
	// ���K�����Ė߂�
	outputVector = normalize(outputVector);

	return outputVector;
}

//============================================================================
//	Main
//============================================================================
[numthreads(1024, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupID) {
	
	// ���_�C���f�b�N�X
	uint vertexIndex = DTid.x;
	uint instanceID = GTid.y;
	// �͈͊O��� (�o�b�t�@�I�[�o�[����)
	if (vertexIndex >= numVertices) {
		return;
	}

	Vertex input = gInputVertices[vertexIndex];
	VertexInfluence influence = gInfluences[vertexIndex];
	
	// �{�[�����ŃI�t�Z�b�g��������
	uint boneOffset = instanceID * numBones;
		
	// Skinning��̒��_���v�Z
	Vertex skinned;
		
	// �ʒu�̕ϊ�
	skinned.position = CalPosition(input, influence, boneOffset);
	// �@���̕ϊ�
	skinned.normal = CalNormal(input, influence, boneOffset);
	// �����x�N�g���̕ϊ�
	skinned.tangent = TransformDirection(input.tangent, influence, boneOffset);
	skinned.biNormal = TransformDirection(input.biNormal, influence, boneOffset);
	skinned.tangent = normalize(skinned.tangent - skinned.normal * dot(skinned.tangent, skinned.normal));
	skinned.biNormal = normalize(cross(skinned.normal, skinned.tangent));
		
	// ���L�̒l�͂��̂܂ܓ����
	skinned.texcoord = input.texcoord;
	skinned.color = input.color;
		
	// Skinning��̒��_�f�[�^���i�[
	// ���_���ŃI�t�Z�b�g��������
	uint instanceOffset = instanceID * numVertices;
	gOutputVertices[vertexIndex + instanceOffset] = skinned;
}
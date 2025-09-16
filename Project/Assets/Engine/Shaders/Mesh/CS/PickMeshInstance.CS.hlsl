//============================================================================
//	CBuffer
//============================================================================

// �s�b�L���O�������ɔ�����s���p�̃f�[�^
cbuffer PickingBuffer : register(b0) {

	uint inputPixelX;
	uint inputPixelY;
	uint textureWidth;
	uint textureHeight;
	float4x4 inverseViewProjection;
	float3 cameraWorldPos;
	float rayMax;
};

//============================================================================
//	StructuredBuffer
//============================================================================

// �s�b�L���O���ʂ��i�[����\����
struct Pick {

	int id;
};

RaytracingAccelerationStructure gScene : register(t0);
RWStructuredBuffer<Pick> gOutput : register(u0);

//============================================================================
//	Functions
//============================================================================

// NDC�֕ϊ�
float3 NDCFromPixel(uint2 pixel, uint2 size, float depth) {

	float2 uv = (float2(pixel) + 0.5f) / float2(size);
	float2 ndc = float2(uv.x * 2.0f - 1.0f, 1.0f - uv.y * 2.0f);
	return float3(ndc, depth);
}

//============================================================================
//	Main
//============================================================================
[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	
	// �N���b�N�s�N�Z���̋߁A���_�����[���h�ɕϊ�����
	uint2 textureSize = uint2(textureWidth, textureHeight);
	uint2 inputPixel = uint2(inputPixelX, inputPixelY);
	float4 nearPos = mul(float4(NDCFromPixel(inputPixel, textureSize, 0.0f), 1.0f), inverseViewProjection);
	float4 farPos = mul(float4(NDCFromPixel(inputPixel, textureSize, 1.0f), 1.0f), inverseViewProjection);
	nearPos /= nearPos.w;
	farPos /= farPos.w;
	
	// �f�X�N�ݒ�
	RayDesc rayDesc;
	// �J�������W���n�_�ɂ��ă��C���΂�
	rayDesc.Origin = cameraWorldPos;
	rayDesc.Direction = normalize(farPos.xyz - nearPos.xyz);
	rayDesc.TMin = 0.0f;
	rayDesc.TMax = rayMax;
	
	RayQuery < 0 > rayQuery;
	rayQuery.TraceRayInline(gScene, 0, 0xFF, rayDesc);

	while (rayQuery.Proceed()) {
		// �����̃I�u�W�F�N�g�͏������Ȃ�
		if (rayQuery.CandidateType() == CANDIDATE_NON_OPAQUE_TRIANGLE) {

			// �q�b�g�����O�p�`������
			rayQuery.CommitNonOpaqueTriangleHit();
		}
	}
	
	gOutput[0].id = -1;
	// �q�b�g�����C���X�^���X��ID���擾����
	if (rayQuery.CommittedStatus() == COMMITTED_TRIANGLE_HIT) {
		
		gOutput[0].id = rayQuery.CommittedInstanceID();
	}
}
//============================================================================
//	include
//============================================================================

#include "../../ParticleCommonSturctures.hlsli"
#include "../../ParticleEmitterStructures.hlsli"
#include "../../../../../../Engine/Effect/Particle/ParticleConfig.h"

#include "../../../Math/Math.hlsli"

//============================================================================
//	CBuffer
//============================================================================

// �`��
struct EmitterCone {
	
	float baseRadius;
	float topRadius;
	float height;
	
	float3 translation;
	float4x4 rotationMatrix;
};

ConstantBuffer<EmitterCommon> gEmitterCommon : register(b0);
ConstantBuffer<EmitterCone> gEmitterCone : register(b1);
ConstantBuffer<PerFrame> gPerFrame : register(b2);

//============================================================================
//	RWStructuredBuffer
//============================================================================

RWStructuredBuffer<Particle> gParticles : register(u0);
RWStructuredBuffer<Transform> gTransform : register(u1);
RWStructuredBuffer<Material> gMaterials : register(u2);

RWStructuredBuffer<int> gFreeListIndex : register(u3);
RWStructuredBuffer<uint> gFreeList : register(u4);

//============================================================================
//	Functions
//============================================================================

float3 GetFacePoint(RandomGenerator generator, float radius, float height) {
	
	float angle = generator.Generate(0.0f, PI2);
	float radiusRadom = generator.Generate(0.0f, radius);

	return float3(radiusRadom * cos(angle), height, radiusRadom * sin(angle));
}

//============================================================================
//	Main
//============================================================================
[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	
	// ������������Ă��Ȃ���Ώ������Ȃ�
	if (gEmitterCommon.emit == 0) {
		return;
	}
	
	// ��������
	RandomGenerator generator;
	generator.seed = (DTid + gPerFrame.time) * gPerFrame.time;

	// ��������������
	for (uint i = 0; i < gEmitterCommon.count; ++i) {
		
		int freeListIndex;
		InterlockedAdd(gFreeListIndex[0], -1, freeListIndex);
		
		if (0 <= freeListIndex && freeListIndex < kMaxParticles) {
			
			uint particleIndex = gFreeList[freeListIndex];
			
			Particle particle;
			particle.currentTime = 0.0f;
			particle.lifeTime = gEmitterCommon.lifeTime;
			
			// �����Ă������(+Z)�����ɔ�΂�
			// ��̖ʂƉ��̖ʂ̍��W�擾
			// ���[�J��
			float3 baseLocal = GetFacePoint(generator, gEmitterCone.baseRadius, 0.0f);
			float3 topLocal = GetFacePoint(generator, gEmitterCone.topRadius, gEmitterCone.height);
			// ���[���h
			float3 baseWorld = gEmitterCone.translation + mul(float4(baseLocal, 1.0f), gEmitterCone.rotationMatrix).xyz;
			float3 topWorld = gEmitterCone.translation + mul(float4(topLocal, 1.0f), gEmitterCone.rotationMatrix).xyz;
			float3 direction = normalize(topWorld - baseWorld);
			particle.velocity = direction * generator.Generate3D();
			
			Transform transform = (Transform) 0;
			transform.translation = baseWorld;
			transform.scale = gEmitterCommon.scale;
			
			Material material = (Material) 0;
			material.color = gEmitterCommon.color;
			
			// �l��ݒ�
			gParticles[particleIndex] = particle;
			gTransform[particleIndex] = transform;
			gMaterials[particleIndex] = material;
		} else {

			InterlockedAdd(gFreeListIndex[0], 1);
			break;
		}
	}
}
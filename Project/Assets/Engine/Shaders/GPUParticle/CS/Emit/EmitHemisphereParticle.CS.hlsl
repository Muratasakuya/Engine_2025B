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
struct EmitterHemisphere {
	
	float radius;
	
	float3 translation;
	float4x4 rotationMatrix;
};

ConstantBuffer<EmitterCommon> gEmitterCommon : register(b0);
ConstantBuffer<EmitterHemisphere> gEmitterHemisphere : register(b1);
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

float3 GetRandomDirection(RandomGenerator generator) {
	
	float phi = generator.Generate(0.0f, PI2);
	float z = generator.Generate(-1.0f, 1.0f);
	float sqrtOneMinusZ2 = sqrt(1.0f - z * z);
	float3 direction = float3(sqrtOneMinusZ2 * cos(phi), sqrtOneMinusZ2 * sin(phi), z);
	
	// ���������Ɍ��肷��
	if (direction.y < 0.0f) {
		direction.y -= direction.y;
	}
	
	return normalize(direction);
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
			
			// ���ʏ�փ����_���Ȍ�����ݒ�
			float3 direction = GetRandomDirection(generator);
			// ��]��K��
			float3 rotatedDirection = mul(direction, (float3x3) gEmitterHemisphere.rotationMatrix);
			particle.velocity = normalize(rotatedDirection) * generator.Generate3D();
			
			Transform transform = (Transform) 0;
			transform.translation = gEmitterHemisphere.translation + direction * gEmitterHemisphere.radius;
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
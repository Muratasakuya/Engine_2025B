//============================================================================
//	include
//============================================================================

#include "../../GPUParticle/ParticleOutput.hlsli"

//============================================================================
//	Output
//============================================================================

struct PSOutput {
	
	float4 color : SV_TARGET0;
};

//============================================================================
//	StructuredBuffer
//============================================================================

struct Material {
	
	float4 color;

	// ����
	float emissiveIntensity;
	float3 emissionColor;

	// 臒l
	float alphaReference;
	float noiseAlphaReference;

	float4x4 uvTransform;
};

struct TextureInfo {
	
	// texture
	uint colorTextureIndex;
	uint noiseTextureIndex;
	
	// sampler
	// 0...WRAP
	// 1...CLAMP
	uint samplerType;
	
	// flags
	int useNoiseTexture;
};

StructuredBuffer<Material> gMaterials : register(t0);
StructuredBuffer<TextureInfo> gTextueInfos : register(t1);

//============================================================================
//	texture Sampler
//============================================================================

Texture2D<float4> gTextures[] : register(t2, space0);
SamplerState gWRAPSampler : register(s0);
SamplerState gCLAMPSampler : register(s1);

//============================================================================
//	Functions
//============================================================================

void CheckDiscard(float alpha, float reference) {
	
	// 臒l�ȉ��Ȃ���p
	if (alpha < reference) {
		discard;
	}
}

float4 GetTextureColor(TextureInfo textureInfo, float4 transformUV) {
	
	float4 textureColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	// �T���v���[�̎�ނŕ���
	if (textureInfo.samplerType == 0) {
		
		textureColor = gTextures[textureInfo.colorTextureIndex].Sample(gWRAPSampler, transformUV.xy);
	} else if (textureInfo.samplerType == 1) {
		
		textureColor = gTextures[textureInfo.colorTextureIndex].Sample(gCLAMPSampler, transformUV.xy);
	}
	return textureColor;
}

void CheckNoiseDiscard(Material material, TextureInfo textureInfo, float4 transformUV) {
	
	// �m�C�Y���g�p���Ȃ��Ȃ炻�������������Ȃ�
	if (textureInfo.useNoiseTexture == 0) {
		return;
	}
	
	float4 color = gTextures[textureInfo.noiseTextureIndex].Sample(gWRAPSampler, transformUV.xy);
	
	// 臒l�ȉ��Ȃ���p
	CheckDiscard(color.a, material.noiseAlphaReference);
}

void ApplyEmissive(inout float3 color, float3 textureRGB, Material material) {
	
	// �����F
	float3 emission = material.emissionColor * material.emissiveIntensity;
	color += emission * textureRGB;
}

//============================================================================
//	Main
//============================================================================
PSOutput main(MSOutput input) {
	
	PSOutput output;
	
	// instanceId�Apixel���Ƃ̏���
	uint id = input.instanceID;
	Material material = gMaterials[id];
	TextureInfo textureInfo = gTextueInfos[id];
	
	// uv�ό`
	float4 transformUV = mul(float4(input.texcoord, 0.0f, 1.0f), material.uvTransform);

	// �m�C�Y���p����
	CheckNoiseDiscard(material, textureInfo, transformUV);

	// �e�N�X�`���F�擾�A���p����
	float4 textureColor = GetTextureColor(textureInfo, transformUV);
	CheckDiscard(textureColor.a, material.alphaReference);
	
	// �F
	output.color.rgb = material.color.rgb * textureColor.rgb;
	
	// ��������
	ApplyEmissive(output.color.rgb, textureColor.rgb, material);
	
	// ���l
	output.color.a = material.color.a * textureColor.a;
	
	return output;
}
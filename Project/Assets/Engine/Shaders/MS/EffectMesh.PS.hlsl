//============================================================================
//	include
//============================================================================

#include "EffectMSFunction.hlsli"

//============================================================================
//	Output
//============================================================================

struct PSOutput {
	
	float4 color : SV_TARGET0;
};

//============================================================================
//	Main
//============================================================================
PSOutput main(MSOutput input) {
	
	PSOutput output;
	
	// instanceId�APixel���Ƃ̏���
	uint id = input.instanceID;
	Material material = gMaterials[id];
	
	// uv
	float4 transformUV = mul(float4(input.texcoord, 0.0f, 1.0f), material.uvTransform);
	
	// noiceTexture�ɂ��pixel���p�Aedge����
	if (material.useNoiseTexture == 1) {
	
		float4 edgeColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
		if (ApplyNoiseDiscardAndEdge(id, transformUV, edgeColor)) {

			output.color = edgeColor;
			// emission����
			// �����F
			float3 emission = material.edgeEmissionColor * material.edgeEmissiveIntensity;
			// emission�����Z
			output.color.rgb += emission;

			return output;
		}
	}
	
	// textureColor�̎擾�Aalpha�l��pixel���p����
	float4 textureColor = GetTextureColor(id, input, transformUV);
	if (textureColor.a < material.textureAlphaReference) {
		discard;
	}
	
	// �F
	output.color.rgb = material.color.rgb * textureColor.rgb;
	
	// emission����
	// �����F
	float3 emission = material.emissionColor * material.emissiveIntensity;
	// emission�����Z
	output.color.rgb += emission * textureColor.rgb;
	
	//���_�J���[�K��
	if (material.useVertexColor == 1) {

		// rgb�̂�
		output.color.rgb *= input.vertexColor.rgb;
	}
	
	// ���l
	output.color.a = material.color.a * input.vertexColor.a * textureColor.a;
	
	return output;
}
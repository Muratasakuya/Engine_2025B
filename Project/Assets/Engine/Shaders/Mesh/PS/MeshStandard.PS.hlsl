//============================================================================
//	include
//============================================================================

#include "../StandardMSFunction.hlsli"

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
	
	// buffer�A�N�Z�X
	Material material = gMaterials[id];
	Lighting lighting = gLightings[id];
	
	// �f�B�U�����������s����
	if (material.enableDithering == 1) {
		
		// pixel�̋������v�Z
		float distanceToEye = distance(worldPosition, input.worldPosition);
		ApplyDistanceDither(input.position.xy, distanceToEye);
	}

	// uv
	float4 transformUV = mul(float4(input.texcoord, 0.0f, 1.0f), material.uvTransform);
	// diffuseColor
	float4 diffuseColor = GetDiffuseColor(material, transformUV, input);
	// ���l
	output.color.a = material.color.a * diffuseColor.a;
	
	// normal
	float3 normal = input.normal;
	if (material.enableNormalMap == 1) {
		
		// normalMapTexture
		normal = GetNormalMap(material, transformUV, input);
	}
	
	// lighting
	if (lighting.enableLighting == 1) {
		if (lighting.enableHalfLambert == 1) {
			
			// halfLambert����
			output.color.rgb = CalculateLambertLighting(material, normal, diffuseColor.rgb);
		}
		if (lighting.enableBlinnPhongReflection == 1) {
			
			// blinnPhong����
			output.color.rgb += CalculateBlinnPhongLighting(material, lighting, normal, diffuseColor.rgb, input);
		}
	} else {

		// lighting����
		output.color.rgb = material.color.rgb * diffuseColor.rgb;
	}
	if (lighting.enableImageBasedLighting == 1) {
	
		// imageBasedLighting����
		output.color.rgb += CalculateImageBasedLighting(normal, lighting.environmentCoefficient, input);
	}
	
	// emissive
	float3 emission = material.emissionColor * material.emissiveIntensity;
	output.color.rgb += emission * diffuseColor.rgb;
	
	if (lighting.shadowRate < 1.0f) {
		
		// �e����
		bool isShadowed = IsShadowed(input.worldPosition);
		// �e�̎�
		if (isShadowed) {
		
			// ������
			output.color.rgb *= lighting.shadowRate;
		}
	}
	
	return output;
}
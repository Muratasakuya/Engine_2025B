//============================================================================
//	include
//============================================================================

#include "StandardMSFunction.hlsli"

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
	
	// instanceIdÅAPixelÇ≤Ç∆ÇÃèàóù
	uint id = input.instanceID;

	// uv
	float4 transformUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterials[id].uvTransform);
	// diffuseColor
	float4 diffuseColor = GetDiffuseColor(id, transformUV, input);
	// Éøíl
	output.color.a = gMaterials[id].color.a * diffuseColor.a;
	
	// normal
	float3 normal = input.normal;
	if (gMaterials[id].enableNormalMap == 1) {
		
		// normalMapTexture
		normal = GetNormalMap(id, transformUV, input);
	}
	
	// lighting
	if (gMaterials[id].enableLighting == 1) {
		if (gMaterials[id].enableHalfLambert == 1) {
			
			// halfLambertèàóù
			output.color.rgb = CalculateLambertLighting(id, normal, diffuseColor.rgb);
		}
		if (gMaterials[id].enableBlinnPhongReflection == 1) {
			
			// blinnPhongèàóù
			output.color.rgb += CalculateBlinnPhongLighting(id, normal, diffuseColor.rgb, input);
		}
	} else {

		// lightingñ≥å¯
		output.color.rgb = gMaterials[id].color.rgb * diffuseColor.rgb;
	}
	if (gMaterials[id].enableImageBasedLighting == 1) {
	
		// imageBasedLightingèàóù
		output.color.rgb += CalculateImageBasedLighting(id, normal,
		gMaterials[id].environmentCoefficient, input);
	}
	
	// emissive
	float3 emission = gMaterials[id].emissionColor * gMaterials[id].emissiveIntensity;
	output.color.rgb += emission * diffuseColor.rgb;
	// shadow
	output.color.rgb = ApplyShadow(id, normal, output.color.rgb, input);
	
	return output;
}
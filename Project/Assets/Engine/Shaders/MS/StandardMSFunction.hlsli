//============================================================================
//	include
//============================================================================

#include "MeshStandard.hlsli"

//============================================================================
//	structure
//============================================================================

struct Material {

	float4 color;
	uint textureIndex;
	uint normalMapTextureIndex;
	int enableNormalMap;
	int enableLighting;
	int enableHalfLambert;
	int enableBlinnPhongReflection;
	int enableImageBasedLighting;
	float environmentCoefficient;
	float shadowRate;
	float shininess;
	float3 specularColor;
	float emissiveIntensity;
	float3 emissionColor;
	float4x4 uvTransform;
};

//============================================================================
//	buffer
//============================================================================

cbuffer PunctualLight : register(b0) {
	
	DirectionalLight directionalLight;
	PointLight pointLight;
	SpotLight spotLight;
};

cbuffer Camera : register(b1) {

	float3 worldPosition;
};

StructuredBuffer<Material> gMaterials : register(t0);

//============================================================================
//	texture Sampler
//============================================================================

Texture2D<float4> gTextures[] : register(t1, space0);
Texture2D<float3> gShadowTexture : register(t2, space1);
TextureCube<float4> gEnvironmentTexture : register(t3, space2);

SamplerState gSampler : register(s0);
SamplerComparisonState gShadowSampler : register(s1);

//============================================================================
//	functions
//============================================================================

float4 GetDiffuseColor(uint id, float4 transformUV, MSOutput input) {
	
	// texture
	float4 diffuseColor = gTextures[gMaterials[id].textureIndex].Sample(gSampler, transformUV.xy);
	
	return diffuseColor;
}

float3 GetNormalMap(uint id, float4 transformUV, MSOutput input) {
	
	// normalMap
	float3 localNormal = gTextures[gMaterials[id].normalMapTextureIndex].Sample(gSampler, transformUV.xy).xyz;
	localNormal = (localNormal - 0.5f) * 2.0f;
	float3 normal = input.tangent * localNormal.x + input.biNormal * localNormal.y + input.normal * localNormal.z;
	
	return normal;
}

float3 CalculateLambertLighting(uint id, float3 normal, float3 diffuseColor) {
	
	float NdotL = dot(normalize(normal), normalize(-directionalLight.direction));
	float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
	return gMaterials[id].color.rgb * diffuseColor.rgb * directionalLight.color.rgb * cos * directionalLight.intensity;
}

float3 CalculateBlinnPhongLighting(uint id, float3 normal, float3 diffuseColor, MSOutput input) {
	
	float3 result = float3(0.0f, 0.0f, 0.0f);

	float3 pointLightDirection = normalize(input.worldPosition - pointLight.pos);
	float distancePointLight = length(pointLight.pos - input.worldPosition);
	float factorPointLight = pow(saturate(-distancePointLight / pointLight.radius + 1.0f), pointLight.decay);

	float3 spotLightDirection = normalize(input.worldPosition - spotLight.pos);
	float distanceSpotLight = length(spotLight.pos - input.worldPosition);
	float factorSpotLight = pow(saturate(-distanceSpotLight / spotLight.distance + 1.0f), spotLight.decay);
	float cosAngle = dot(spotLightDirection, spotLight.direction);
	float falloffFactor = saturate((cosAngle - spotLight.cosAngle) / (spotLight.cosFalloffStart - spotLight.cosAngle));

	float3 toEye = normalize(worldPosition - input.worldPosition);
	
	float3 halfPL = normalize(-pointLightDirection + toEye);
	float specPL = pow(saturate(dot(normalize(normal), halfPL)), gMaterials[id].shininess);
	float cosPL = pow(dot(normalize(normal), -pointLightDirection) * 0.5f + 0.5f, 2.0f);

	float3 halfSL = normalize(-spotLightDirection + toEye);
	float specSL = pow(saturate(dot(normalize(normal), halfSL)), gMaterials[id].shininess);
	float cosSL = pow(dot(normalize(normal), -spotLightDirection) * 0.5f + 0.5f, 2.0f);

	float3 diffusePL = gMaterials[id].color.rgb * diffuseColor.rgb * pointLight.color.rgb * cosPL * pointLight.intensity * factorPointLight;
	float3 specularPL = pointLight.color.rgb * pointLight.intensity * factorPointLight * specPL * gMaterials[id].specularColor;

	float3 diffuseSL = gMaterials[id].color.rgb * diffuseColor.rgb * spotLight.color.rgb * cosSL * spotLight.intensity * falloffFactor * factorSpotLight;
	float3 specularSL = spotLight.color.rgb * spotLight.intensity * falloffFactor * factorSpotLight * specSL * gMaterials[id].specularColor;

	result += diffusePL + specularPL + diffuseSL + specularSL;
	return result;
}

float3 CalculateImageBasedLighting(uint id, float3 normal, float environmentCoefficient, MSOutput input) {
	
	float3 cameraToPos = normalize(input.worldPosition - worldPosition);
	float3 reflectedVector = reflect(cameraToPos, normalize(normal));
	float4 environmentColor = gEnvironmentTexture.Sample(gSampler, reflectedVector);

	return environmentColor.rgb * environmentCoefficient;
}

float3 ApplyShadow(uint id, float3 normal, float3 color, MSOutput input) {
	
	float2 shadowMapUV = input.positionInLVP.xy / input.positionInLVP.w;
	shadowMapUV *= float2(0.5f, -0.5f);
	shadowMapUV += 0.5f;
	
	float zInLVp = input.positionInLVP.z / input.positionInLVP.w;
	
	float3 outputColor = color.rgb;
	
	if (input.normal.y == 0.0f) {
		return outputColor;
	}
	
	if (shadowMapUV.x > 0.0f && shadowMapUV.x < 1.0f &&
		shadowMapUV.y > 0.0f && shadowMapUV.y < 1.0f) {
		
		float NdotL = max(0.0f, dot(normal, directionalLight.direction));

		float slopeScale = 1.0f;
		float constantBias = 0.001f;
		float bias = NdotL + constantBias;
		
		float zInShadowMap = gShadowTexture.Sample(gSampler, shadowMapUV).r;

		if (zInLVp - bias > zInShadowMap) {

			float shadow = gShadowTexture.SampleCmpLevelZero(
			gShadowSampler, shadowMapUV, zInLVp);

			float3 shadowColor = color.rgb * gMaterials[id].shadowRate;
			outputColor = lerp(color.rgb, shadowColor, shadow);
		}
	}
	
	return outputColor;
}
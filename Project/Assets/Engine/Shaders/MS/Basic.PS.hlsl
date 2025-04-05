//============================================================================
//	include
//============================================================================

#include "Basic.hlsli"

//============================================================================
//	Output
//============================================================================

struct PSOutput {
	
	float4 color : SV_TARGET0;
};

//============================================================================
//	CBuffer
//============================================================================

cbuffer Material : register(b0) {

	float4 color;
	int enableLighting;
	int enableHalfLambert;
	int enablePhongReflection;
	int enableBlinnPhongReflection;
	float shadowRate;
	float shininess;
	float3 specularColor;
	float emissiveIntensity;
	float3 emissionColor;
	float4x4 uvTransform;
};

cbuffer PunctualLight : register(b1) {
	
	DirectionalLight directionalLight;
	PointLight pointLight;
	SpotLight spotLight;
};

cbuffer Camera : register(b2) {

	float3 worldPosition;
};

//============================================================================
//	Texture Sampler
//============================================================================

Texture2D<float4> gTexture : register(t0);
Texture2D<float3> gShadowTexture : register(t1);

SamplerState gSampler : register(s0);
SamplerComparisonState gShadowSampler : register(s1);

//============================================================================
//	Main
//============================================================================
PSOutput main(MSOutput input) {
	
	PSOutput output;
	
	float4 transformUV = mul(float4(input.texcoord, 0.0f, 1.0f), uvTransform);
	float4 textureColor = gTexture.Sample(gSampler, transformUV.xy);
	
	//========================================================================*/
	//* Lighting *//
	
	if (enableLighting == 1) {
		
		if (enableHalfLambert == 1) {

			float NdotL = dot(normalize(input.normal), normalize(-directionalLight.direction));
			float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
			
			// rgb
			output.color.rgb = color.rgb * textureColor.rgb * directionalLight.color.rgb * cos * directionalLight.intensity;
		} else {

			float cos = saturate(dot(normalize(input.normal), -directionalLight.direction));
			
			// rgb
			output.color.rgb =
			color.rgb * textureColor.rgb * directionalLight.color.rgb * cos * directionalLight.intensity;
		}
		
		if (enablePhongReflection == 1) {
			
			// PointLight‚Ì“üŽËŒõ
			float3 pointLightDirection = normalize(input.worldPosition - pointLight.pos);
			// PointLight‚Ö‚Ì‹——£
			float distancePointLight = length(pointLight.pos - input.worldPosition);
			// Žw”‚É‚æ‚éƒRƒ“ƒgƒ[ƒ‹
			float factorPointLight = pow(saturate(-distancePointLight / pointLight.radius + 1.0f), pointLight.decay);
			// SpotLight‚Ì“üŽËŒõ
			float3 spotLightDirectionOnSurface = normalize(input.worldPosition - spotLight.pos);
			// SpotLight‚Ö‚Ì‹——£
			float distanceSpotLight = length(spotLight.pos - input.worldPosition);
			// Žw”‚É‚æ‚éƒRƒ“ƒgƒ[ƒ‹
			float factorSpotLight = pow(saturate(-distanceSpotLight / spotLight.distance + 1.0f), spotLight.decay);
			// Camera•ûŒüŽZo
			float3 toEye = normalize(worldPosition - input.worldPosition);

			/*-------------------------------------------------------------------------------------------------*/
			/// PointLight

			// “üŽËŒõ‚Ì”½ŽËƒxƒNƒgƒ‹‚ÌŒvŽZ
			float3 reflectPointLight = reflect(pointLightDirection, normalize(input.normal));
			float RdotEPointLight = dot(reflectPointLight, toEye);
			float specularPowPointLight = pow(saturate(RdotEPointLight), shininess);
			float NdotLPointLight = dot(normalize(input.normal), -pointLightDirection);
			float cosPointLight = pow(NdotLPointLight * 0.5f + 0.5f, 2.0f);
			// ŠgŽU”½ŽË
			float3 diffusePointLight =
			color.rgb * textureColor.rgb * pointLight.color.rgb * cosPointLight * pointLight.intensity * factorPointLight;
			// ‹¾–Ê”½ŽË
			float3 specularPointLight =
			pointLight.color.rgb * pointLight.intensity * factorPointLight * specularPowPointLight * specularColor;
			
			/*-------------------------------------------------------------------------------------------------*/
			/// SpotLight
			
			// “üŽËŒõ‚Ì”½ŽËƒxƒNƒgƒ‹‚ÌŒvŽZ
			float3 reflectSpotLight = reflect(spotLightDirectionOnSurface, normalize(input.normal));
			float RdotESpotLight = dot(reflectSpotLight, toEye);
			float specularPowSpotLight = pow(saturate(RdotESpotLight), shininess);
			float NdotLSpotLight = dot(normalize(input.normal), -spotLightDirectionOnSurface);
			float cosSpotLight = pow(NdotLSpotLight * 0.5f + 0.5f, 2.0f);
			float cosAngle = dot(spotLightDirectionOnSurface, spotLight.direction);
			float falloffFactor = saturate((cosAngle - spotLight.cosAngle) / (spotLight.cosFalloffStart - spotLight.cosAngle));
			// ŠgŽU”½ŽË
			float3 diffuseSpotLight =
			color.rgb * textureColor.rgb * spotLight.color.rgb * cosSpotLight * spotLight.intensity * factorSpotLight * falloffFactor;
			// ‹¾–Ê”½ŽË
			float3 specularSpotLight =
			spotLight.color.rgb * spotLight.intensity * factorSpotLight * falloffFactor * specularPowSpotLight * specularColor;

			/*-------------------------------------------------------------------------------------------------*/

			// ŠgŽU”½ŽË + ‹¾–Ê”½ŽË PointLight + SpotLight
			output.color.rgb += diffusePointLight + specularPointLight + diffuseSpotLight + specularSpotLight;
		}
		
		if (enableBlinnPhongReflection == 1) {

			// PointLight‚Ì“üŽËŒõ
			float3 pointLightDirection = normalize(input.worldPosition - pointLight.pos);
			// PointLight‚Ö‚Ì‹——£
			float distancePointLight = length(pointLight.pos - input.worldPosition);
			// Žw”‚É‚æ‚éƒRƒ“ƒgƒ[ƒ‹
			float factorPointLight = pow(saturate(-distancePointLight / pointLight.radius + 1.0f), pointLight.decay);
			// SpotLight‚Ì“üŽËŒõ
			float3 spotLightDirectionOnSurface = normalize(input.worldPosition - spotLight.pos);
			// SpotLight‚Ö‚Ì‹——£
			float distanceSpotLight = length(spotLight.pos - input.worldPosition);
			// Žw”‚É‚æ‚éƒRƒ“ƒgƒ[ƒ‹
			float factorSpotLight = pow(saturate(-distanceSpotLight / spotLight.distance + 1.0f), spotLight.decay);
			// Camera•ûŒüŽZo
			float3 toEye = normalize(worldPosition - input.worldPosition);

			/*-------------------------------------------------------------------------------------------------*/
			/// PointLight

			float3 halfVectorPointLight = normalize(-pointLightDirection + toEye);
			float NDotHPointLight = dot(normalize(input.normal), halfVectorPointLight);
			float specularPowPointLight = pow(saturate(NDotHPointLight), shininess);

			float NdotLPointLight = dot(normalize(input.normal), -pointLightDirection);
			float cosPointLight = pow(NdotLPointLight * 0.5f + 0.5f, 2.0f);

			// ŠgŽU”½ŽË
			float3 diffusePointLight =
			color.rgb * textureColor.rgb * pointLight.color.rgb * cosPointLight * pointLight.intensity * factorPointLight;

			// ‹¾–Ê”½ŽË
			float3 specularPointLight =
			pointLight.color.rgb * pointLight.intensity * factorPointLight * specularPowPointLight * specularColor;

			/*-------------------------------------------------------------------------------------------------*/
			/// SpotLight

			float3 halfVectorSpotLight = normalize(-spotLightDirectionOnSurface + toEye);
			float NDotHSpotLight = dot(normalize(input.normal), halfVectorSpotLight);
			float specularPowSpotLight = pow(saturate(NDotHSpotLight), shininess);

			float NdotLSpotLight = dot(normalize(input.normal), -spotLightDirectionOnSurface);
			float cosSpotLight = pow(NdotLSpotLight * 0.5f + 0.5f, 2.0f);

			float cosAngle = dot(spotLightDirectionOnSurface, spotLight.direction);
			float falloffFactor = saturate((cosAngle - spotLight.cosAngle) / (spotLight.cosFalloffStart - spotLight.cosAngle));

			// ŠgŽU”½ŽË
			float3 diffuseSpotLight =
			color.rgb * textureColor.rgb * spotLight.color.rgb * cosSpotLight * spotLight.intensity * falloffFactor * factorSpotLight;

			// ‹¾–Ê”½ŽË
			float3 specularSpotLight =
			spotLight.color.rgb * spotLight.intensity * falloffFactor * factorSpotLight * specularPowSpotLight * specularColor;

			/*-------------------------------------------------------------------------------------------------*/

			// ŠgŽU”½ŽË + ‹¾–Ê”½ŽË LightDirecion + PointLight + SpotLight
			output.color.rgb += diffusePointLight + specularPointLight + diffuseSpotLight + specularSpotLight;
		}
	} else {

		output.color.rgb = color.rgb * textureColor.rgb;
	}
	
	//========================================================================*/
	//* emission *//
	
	// ”­ŒõF
	float3 emission = emissionColor * emissiveIntensity;
	// Emission‚ð‰ÁŽZ
	output.color.rgb += emission * textureColor.rgb;
	
	//========================================================================*/
	//* shadow *//
	
	// wœŽZ‚Å³‹K‰»ƒXƒNƒŠ[ƒ“À•WŒn‚É•ÏŠ·‚·‚é
	float2 shadowMapUV = input.positionInLVP.xy / input.positionInLVP.w;
	shadowMapUV *= float2(0.5f, -0.5f);
	shadowMapUV += 0.5f;
	
	// ƒ‰ƒCƒgƒrƒ…[ƒXƒNƒŠ[ƒ“‹óŠÔ‚Å‚ÌZ’l‚ðŒvŽZ‚·‚é
	float zInLVp = input.positionInLVP.z / input.positionInLVP.w;
	
	if (shadowMapUV.x > 0.0f && shadowMapUV.x < 1.0f &&
		shadowMapUV.y > 0.0f && shadowMapUV.y < 1.0f) {
		
		// –@ü‚ÆŒõ‚Ì•ûŒü‚Ì“àÏ‚ðŒvŽZ
		float NdotL = max(0.0f, dot(input.normal, directionalLight.direction));

		float slopeScale = 1.0f;
		float constantBias = 0.001f;
		float bias = NdotL + constantBias;
		
		// –@ü‚ª‰¡Œü‚«‚È‚ç‰e‚ð–³Œø‰»
		if (abs(input.normal.y) < 0.1f) {
			bias = 1.0f;
		}

		// ƒVƒƒƒhƒEƒ}ƒbƒv‚Ì[“x’l‚ðŽæ“¾
		float zInShadowMap = gShadowTexture.Sample(gSampler, shadowMapUV).r;

		if (zInLVp - bias > zInShadowMap) {
			
			// ŽÕ•Á—¦‚ÌŽæ“¾
			float shadow = gShadowTexture.SampleCmpLevelZero(
			gShadowSampler, shadowMapUV, zInLVp);

			float3 shadowColor = output.color.rgb * shadowRate;
			output.color.rgb = lerp(output.color.rgb, shadowColor, shadow);
		}
	}

	// ƒ¿’l
	output.color.a = color.a * textureColor.a;
	
	return output;
}
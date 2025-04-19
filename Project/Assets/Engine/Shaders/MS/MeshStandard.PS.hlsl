//============================================================================
//	include
//============================================================================

#include "MeshStandard.hlsli"

//============================================================================
//	Output
//============================================================================

struct PSOutput {
	
	float4 color : SV_TARGET0;
};

//============================================================================
//	CBuffer
//============================================================================

cbuffer PunctualLight : register(b0) {
	
	DirectionalLight directionalLight;
	PointLight pointLight;
	SpotLight spotLight;
};

cbuffer Camera : register(b1) {

	float3 worldPosition;
};

//============================================================================
//	StructuredBuffer
//============================================================================

struct Material {

	float4 color;
	uint textureIndex;
	uint normalMapTextureIndex;
	int enableNormalMap;
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

StructuredBuffer<Material> gMaterials : register(t0);

//============================================================================
//	Texture Sampler
//============================================================================

Texture2D<float4> gTextures[] : register(t1, space0);
Texture2D<float3> gShadowTexture : register(t2, space1);

SamplerState gSampler : register(s0);
SamplerComparisonState gShadowSampler : register(s1);

//============================================================================
//	Main
//============================================================================
PSOutput main(MSOutput input) {
	
	PSOutput output;
	
	// instanceId、Pixelごとの処理
	uint id = input.instanceID;

	float4 transformUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterials[id].uvTransform);
	float4 textureColor = gTextures[gMaterials[id].textureIndex].Sample(gSampler, transformUV.xy);
	
	//========================================================================*/
	//* NormalMap *//
	
	float3 normal = input.normal;
	
	if (gMaterials[id].enableNormalMap == 1) {
		
		// 法線マップを使用するときは法線を再設定する
		float3 localNormal = gTextures[gMaterials[id].normalMapTextureIndex].Sample(gSampler, transformUV.xy).xyz;
		// 法線を0～1の範囲から-1～1の範囲に復元する
		localNormal = (localNormal - 0.5f) * 2.0f;
		// タンジェントスペースの法線をワールドスペースに変換する
		normal = input.tangent * localNormal.x + input.biNormal * localNormal.y + normal * localNormal.z;
	}
	
	//========================================================================*/
	//* Lighting *//
	
	if (gMaterials[id].enableLighting == 1) {
		
		if (gMaterials[id].enableHalfLambert == 1) {

			float NdotL = dot(normalize(normal), normalize(-directionalLight.direction));
			float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
			
			// rgb
			output.color.rgb = gMaterials[id].color.rgb * textureColor.rgb * directionalLight.color.rgb * cos * directionalLight.intensity;
		} else {

			float cos = saturate(dot(normalize(normal), -directionalLight.direction));
			
			// rgb
			output.color.rgb =
			gMaterials[id].color.rgb * textureColor.rgb * directionalLight.color.rgb * cos * directionalLight.intensity;
		}
		
		if (gMaterials[id].enablePhongReflection == 1) {
			
			// PointLightの入射光
			float3 pointLightDirection = normalize(input.worldPosition - pointLight.pos);
			// PointLightへの距離
			float distancePointLight = length(pointLight.pos - input.worldPosition);
			// 指数によるコントロール
			float factorPointLight = pow(saturate(-distancePointLight / pointLight.radius + 1.0f), pointLight.decay);
			// SpotLightの入射光
			float3 spotLightDirectionOnSurface = normalize(input.worldPosition - spotLight.pos);
			// SpotLightへの距離
			float distanceSpotLight = length(spotLight.pos - input.worldPosition);
			// 指数によるコントロール
			float factorSpotLight = pow(saturate(-distanceSpotLight / spotLight.distance + 1.0f), spotLight.decay);
			// Camera方向算出
			float3 toEye = normalize(worldPosition - input.worldPosition);

			/*-------------------------------------------------------------------------------------------------*/
			/// PointLight

			// 入射光の反射ベクトルの計算
			float3 reflectPointLight = reflect(pointLightDirection, normalize(normal));
			float RdotEPointLight = dot(reflectPointLight, toEye);
			float specularPowPointLight = pow(saturate(RdotEPointLight), gMaterials[id].shininess);
			float NdotLPointLight = dot(normalize(normal), -pointLightDirection);
			float cosPointLight = pow(NdotLPointLight * 0.5f + 0.5f, 2.0f);
			// 拡散反射
			float3 diffusePointLight =
			gMaterials[id].color.rgb * textureColor.rgb * pointLight.color.rgb * cosPointLight * pointLight.intensity * factorPointLight;
			// 鏡面反射
			float3 specularPointLight =
			pointLight.color.rgb * pointLight.intensity * factorPointLight * specularPowPointLight * gMaterials[id].specularColor;
			
			/*-------------------------------------------------------------------------------------------------*/
			/// SpotLight
			
			// 入射光の反射ベクトルの計算
			float3 reflectSpotLight = reflect(spotLightDirectionOnSurface, normalize(normal));
			float RdotESpotLight = dot(reflectSpotLight, toEye);
			float specularPowSpotLight = pow(saturate(RdotESpotLight), gMaterials[id].shininess);
			float NdotLSpotLight = dot(normalize(normal), -spotLightDirectionOnSurface);
			float cosSpotLight = pow(NdotLSpotLight * 0.5f + 0.5f, 2.0f);
			float cosAngle = dot(spotLightDirectionOnSurface, spotLight.direction);
			float falloffFactor = saturate((cosAngle - spotLight.cosAngle) / (spotLight.cosFalloffStart - spotLight.cosAngle));
			// 拡散反射
			float3 diffuseSpotLight =
			gMaterials[id].color.rgb * textureColor.rgb * spotLight.color.rgb * cosSpotLight * spotLight.intensity * factorSpotLight * falloffFactor;
			// 鏡面反射
			float3 specularSpotLight =
			spotLight.color.rgb * spotLight.intensity * factorSpotLight * falloffFactor * specularPowSpotLight * gMaterials[id].specularColor;

			/*-------------------------------------------------------------------------------------------------*/

			// 拡散反射 + 鏡面反射 PointLight + SpotLight
			output.color.rgb += diffusePointLight + specularPointLight + diffuseSpotLight + specularSpotLight;
		}
		
		if (gMaterials[id].enableBlinnPhongReflection == 1) {

			// PointLightの入射光
			float3 pointLightDirection = normalize(input.worldPosition - pointLight.pos);
			// PointLightへの距離
			float distancePointLight = length(pointLight.pos - input.worldPosition);
			// 指数によるコントロール
			float factorPointLight = pow(saturate(-distancePointLight / pointLight.radius + 1.0f), pointLight.decay);
			// SpotLightの入射光
			float3 spotLightDirectionOnSurface = normalize(input.worldPosition - spotLight.pos);
			// SpotLightへの距離
			float distanceSpotLight = length(spotLight.pos - input.worldPosition);
			// 指数によるコントロール
			float factorSpotLight = pow(saturate(-distanceSpotLight / spotLight.distance + 1.0f), spotLight.decay);
			// Camera方向算出
			float3 toEye = normalize(worldPosition - input.worldPosition);

			/*-------------------------------------------------------------------------------------------------*/
			/// PointLight

			float3 halfVectorPointLight = normalize(-pointLightDirection + toEye);
			float NDotHPointLight = dot(normalize(normal), halfVectorPointLight);
			float specularPowPointLight = pow(saturate(NDotHPointLight), gMaterials[id].shininess);

			float NdotLPointLight = dot(normalize(normal), -pointLightDirection);
			float cosPointLight = pow(NdotLPointLight * 0.5f + 0.5f, 2.0f);

			// 拡散反射
			float3 diffusePointLight =
			gMaterials[id].color.rgb * textureColor.rgb * pointLight.color.rgb * cosPointLight * pointLight.intensity * factorPointLight;

			// 鏡面反射
			float3 specularPointLight =
			pointLight.color.rgb * pointLight.intensity * factorPointLight * specularPowPointLight * gMaterials[id].specularColor;

			/*-------------------------------------------------------------------------------------------------*/
			/// SpotLight

			float3 halfVectorSpotLight = normalize(-spotLightDirectionOnSurface + toEye);
			float NDotHSpotLight = dot(normalize(normal), halfVectorSpotLight);
			float specularPowSpotLight = pow(saturate(NDotHSpotLight), gMaterials[id].shininess);

			float NdotLSpotLight = dot(normalize(normal), -spotLightDirectionOnSurface);
			float cosSpotLight = pow(NdotLSpotLight * 0.5f + 0.5f, 2.0f);

			float cosAngle = dot(spotLightDirectionOnSurface, spotLight.direction);
			float falloffFactor = saturate((cosAngle - spotLight.cosAngle) / (spotLight.cosFalloffStart - spotLight.cosAngle));

			// 拡散反射
			float3 diffuseSpotLight =
			gMaterials[id].color.rgb * textureColor.rgb * spotLight.color.rgb * cosSpotLight * spotLight.intensity * falloffFactor * factorSpotLight;

			// 鏡面反射
			float3 specularSpotLight =
			spotLight.color.rgb * spotLight.intensity * falloffFactor * factorSpotLight * specularPowSpotLight * gMaterials[id].specularColor;

			/*-------------------------------------------------------------------------------------------------*/

			// 拡散反射 + 鏡面反射 LightDirecion + PointLight + SpotLight
			output.color.rgb += diffusePointLight + specularPointLight + diffuseSpotLight + specularSpotLight;
		}
	} else {

		output.color.rgb = gMaterials[id].color.rgb * textureColor.rgb;
	}
	
	//========================================================================*/
	//* emission *//
	
	// 発光色
	float3 emission = gMaterials[id].emissionColor * gMaterials[id].emissiveIntensity;
	// Emissionを加算
	output.color.rgb += emission * textureColor.rgb;
	
	//========================================================================*/
	//* shadow *//
	
	// w除算で正規化スクリーン座標系に変換する
	float2 shadowMapUV = input.positionInLVP.xy / input.positionInLVP.w;
	shadowMapUV *= float2(0.5f, -0.5f);
	shadowMapUV += 0.5f;
	
	// ライトビュースクリーン空間でのZ値を計算する
	float zInLVp = input.positionInLVP.z / input.positionInLVP.w;
	
	if (shadowMapUV.x > 0.0f && shadowMapUV.x < 1.0f &&
		shadowMapUV.y > 0.0f && shadowMapUV.y < 1.0f) {
		
		// 法線と光の方向の内積を計算
		float NdotL = max(0.0f, dot(normal, directionalLight.direction));

		float slopeScale = 1.0f;
		float constantBias = 0.001f;
		float bias = NdotL + constantBias;
		
		// シャドウマップの深度値を取得
		float zInShadowMap = gShadowTexture.Sample(gSampler, shadowMapUV).r;

		if (zInLVp - bias > zInShadowMap) {
			
			// 遮蔽率の取得
			float shadow = gShadowTexture.SampleCmpLevelZero(
			gShadowSampler, shadowMapUV, zInLVp);

			float3 shadowColor = output.color.rgb * gMaterials[id].shadowRate;
			output.color.rgb = lerp(output.color.rgb, shadowColor, shadow);
		}
	}

	// α値
	output.color.a = gMaterials[id].color.a * textureColor.a;
	
	return output;
}
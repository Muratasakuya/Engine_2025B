//============================================================================*/
//	include
//============================================================================*/

#include "Object3D.hlsli"

//============================================================================*/
//	Object3D PS
//============================================================================*/

struct Material {

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

/// 平行光源
struct DirectionalLight {

	float4 color;
	float3 direction;
	float intensity;
};

/// ポイントライト
struct PointLight {
	
	float4 color;
	float3 pos;
	float intensity;
	float radius;
	float decay;
};

/// スポットライト
struct SpotLight {

	float4 color;
	float3 pos;
	float intensity;
	float3 direction;
	float distance;
	float decay;
	float cosAngle;
	float cosFalloffStart;
};

// 全てのライト
struct PunctualLight {
	
	DirectionalLight directionalLight;
	PointLight pointLight;
	SpotLight spotLight;
};

// カメラ
struct Camera {

	float3 worldPosition;
};

struct PixelShaderOutput {
    
	float4 color : SV_TARGET0;
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<PunctualLight> gPunctual : register(b1);
ConstantBuffer<Camera> gCamera : register(b2);

Texture2D<float4> gTexture : register(t0);
Texture2D<float3> gShadowTexture : register(t1);
SamplerState gSampler : register(s0);
SamplerComparisonState gShadowSampler : register(s1);

PixelShaderOutput main(VertexShaderOutput input) {
   
	PixelShaderOutput output;

	float4 transformUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
	float4 textureColor = gTexture.Sample(gSampler, transformUV.xy);
	
	//========================================================================*/
	//* Lighting *//
	
	if (gMaterial.enableLighting == 1) {
		
		if (gMaterial.enableHalfLambert == 1) {

			float NdotL = dot(normalize(input.normal), normalize(-gPunctual.directionalLight.direction));
			float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
			
			// rgb
			output.color.rgb = gMaterial.color.rgb * textureColor.rgb * gPunctual.directionalLight.color.rgb * cos * gPunctual.directionalLight.intensity;
		} else {

			float cos = saturate(dot(normalize(input.normal), -gPunctual.directionalLight.direction));
			
			// rgb
			output.color.rgb =
			gMaterial.color.rgb * textureColor.rgb * gPunctual.directionalLight.color.rgb * cos * gPunctual.directionalLight.intensity;
		}
		
		if (gMaterial.enablePhongReflection == 1) {
			
			// PointLightの入射光
			float3 pointLightDirection = normalize(input.worldPosition - gPunctual.pointLight.pos);
			// PointLightへの距離
			float distancePointLight = length(gPunctual.pointLight.pos - input.worldPosition);
			// 指数によるコントロール
			float factorPointLight = pow(saturate(-distancePointLight / gPunctual.pointLight.radius + 1.0f), gPunctual.pointLight.decay);
			// SpotLightの入射光
			float3 spotLightDirectionOnSurface = normalize(input.worldPosition - gPunctual.spotLight.pos);
			// SpotLightへの距離
			float distanceSpotLight = length(gPunctual.spotLight.pos - input.worldPosition);
			// 指数によるコントロール
			float factorSpotLight = pow(saturate(-distanceSpotLight / gPunctual.spotLight.distance + 1.0f), gPunctual.spotLight.decay);
			// Camera方向算出
			float3 toEye = normalize(gCamera.worldPosition - input.worldPosition);

			/*-------------------------------------------------------------------------------------------------*/
			/// PointLight

			// 入射光の反射ベクトルの計算
			float3 reflectPointLight = reflect(pointLightDirection, normalize(input.normal));
			float RdotEPointLight = dot(reflectPointLight, toEye);
			float specularPowPointLight = pow(saturate(RdotEPointLight), gMaterial.shininess);
			float NdotLPointLight = dot(normalize(input.normal), -pointLightDirection);
			float cosPointLight = pow(NdotLPointLight * 0.5f + 0.5f, 2.0f);
			// 拡散反射
			float3 diffusePointLight =
			gMaterial.color.rgb * textureColor.rgb * gPunctual.pointLight.color.rgb * cosPointLight * gPunctual.pointLight.intensity * factorPointLight;
			// 鏡面反射
			float3 specularPointLight =
			gPunctual.pointLight.color.rgb * gPunctual.pointLight.intensity * factorPointLight * specularPowPointLight * gMaterial.specularColor;
			
			/*-------------------------------------------------------------------------------------------------*/
			/// SpotLight
			
			// 入射光の反射ベクトルの計算
			float3 reflectSpotLight = reflect(spotLightDirectionOnSurface, normalize(input.normal));
			float RdotESpotLight = dot(reflectSpotLight, toEye);
			float specularPowSpotLight = pow(saturate(RdotESpotLight), gMaterial.shininess);
			float NdotLSpotLight = dot(normalize(input.normal), -spotLightDirectionOnSurface);
			float cosSpotLight = pow(NdotLSpotLight * 0.5f + 0.5f, 2.0f);
			float cosAngle = dot(spotLightDirectionOnSurface, gPunctual.spotLight.direction);
			float falloffFactor = saturate((cosAngle - gPunctual.spotLight.cosAngle) / (gPunctual.spotLight.cosFalloffStart - gPunctual.spotLight.cosAngle));
			// 拡散反射
			float3 diffuseSpotLight =
			gMaterial.color.rgb * textureColor.rgb * gPunctual.spotLight.color.rgb * cosSpotLight * gPunctual.spotLight.intensity * factorSpotLight * falloffFactor;
			// 鏡面反射
			float3 specularSpotLight =
			gPunctual.spotLight.color.rgb * gPunctual.spotLight.intensity * factorSpotLight * falloffFactor * specularPowSpotLight * gMaterial.specularColor;

			/*-------------------------------------------------------------------------------------------------*/

			// 拡散反射 + 鏡面反射 PointLight + SpotLight
			output.color.rgb += diffusePointLight + specularPointLight + diffuseSpotLight + specularSpotLight;
		}
		
		if (gMaterial.enableBlinnPhongReflection == 1) {

			// PointLightの入射光
			float3 pointLightDirection = normalize(input.worldPosition - gPunctual.pointLight.pos);
			// PointLightへの距離
			float distancePointLight = length(gPunctual.pointLight.pos - input.worldPosition);
			// 指数によるコントロール
			float factorPointLight = pow(saturate(-distancePointLight / gPunctual.pointLight.radius + 1.0f), gPunctual.pointLight.decay);
			// SpotLightの入射光
			float3 spotLightDirectionOnSurface = normalize(input.worldPosition - gPunctual.spotLight.pos);
			// SpotLightへの距離
			float distanceSpotLight = length(gPunctual.spotLight.pos - input.worldPosition);
			// 指数によるコントロール
			float factorSpotLight = pow(saturate(-distanceSpotLight / gPunctual.spotLight.distance + 1.0f), gPunctual.spotLight.decay);
			// Camera方向算出
			float3 toEye = normalize(gCamera.worldPosition - input.worldPosition);

			/*-------------------------------------------------------------------------------------------------*/
			/// PointLight

			float3 halfVectorPointLight = normalize(-pointLightDirection + toEye);
			float NDotHPointLight = dot(normalize(input.normal), halfVectorPointLight);
			float specularPowPointLight = pow(saturate(NDotHPointLight), gMaterial.shininess);

			float NdotLPointLight = dot(normalize(input.normal), -pointLightDirection);
			float cosPointLight = pow(NdotLPointLight * 0.5f + 0.5f, 2.0f);

			// 拡散反射
			float3 diffusePointLight =
			gMaterial.color.rgb * textureColor.rgb * gPunctual.pointLight.color.rgb * cosPointLight * gPunctual.pointLight.intensity * factorPointLight;

			// 鏡面反射
			float3 specularPointLight =
			gPunctual.pointLight.color.rgb * gPunctual.pointLight.intensity * factorPointLight * specularPowPointLight * gMaterial.specularColor;

			/*-------------------------------------------------------------------------------------------------*/
			/// SpotLight

			float3 halfVectorSpotLight = normalize(-spotLightDirectionOnSurface + toEye);
			float NDotHSpotLight = dot(normalize(input.normal), halfVectorSpotLight);
			float specularPowSpotLight = pow(saturate(NDotHSpotLight), gMaterial.shininess);

			float NdotLSpotLight = dot(normalize(input.normal), -spotLightDirectionOnSurface);
			float cosSpotLight = pow(NdotLSpotLight * 0.5f + 0.5f, 2.0f);

			float cosAngle = dot(spotLightDirectionOnSurface, gPunctual.spotLight.direction);
			float falloffFactor = saturate((cosAngle - gPunctual.spotLight.cosAngle) / (gPunctual.spotLight.cosFalloffStart - gPunctual.spotLight.cosAngle));

			// 拡散反射
			float3 diffuseSpotLight =
			gMaterial.color.rgb * textureColor.rgb * gPunctual.spotLight.color.rgb * cosSpotLight * gPunctual.spotLight.intensity * falloffFactor * factorSpotLight;

			// 鏡面反射
			float3 specularSpotLight =
			gPunctual.spotLight.color.rgb * gPunctual.spotLight.intensity * falloffFactor * factorSpotLight * specularPowSpotLight * gMaterial.specularColor;

			/*-------------------------------------------------------------------------------------------------*/

			// 拡散反射 + 鏡面反射 LightDirecion + PointLight + SpotLight
			output.color.rgb += diffusePointLight + specularPointLight + diffuseSpotLight + specularSpotLight;
		}
	} else {

		output.color.rgb = gMaterial.color.rgb * textureColor.rgb;
	}
	
	//========================================================================*/
	//* emission *//
	
	// 発光色
	float3 emissionColor = gMaterial.emissionColor * gMaterial.emissiveIntensity;
	// Emissionを加算
	output.color.rgb += emissionColor * textureColor.rgb;
	
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
		float NdotL = max(0.0f, dot(input.normal, gPunctual.directionalLight.direction));

		float slopeScale = 1.0f;
		float constantBias = 0.001f;
		float bias = NdotL + constantBias;
		
		// 法線が横向きなら影を無効化
		if (abs(input.normal.y) < 0.1f) {
			bias = 1.0f;
		}

		// シャドウマップの深度値を取得
		float zInShadowMap = gShadowTexture.Sample(gSampler, shadowMapUV).r;

		if (zInLVp - bias > zInShadowMap) {
			
			// 遮蔽率の取得
			float shadow = gShadowTexture.SampleCmpLevelZero(
			gShadowSampler, shadowMapUV, zInLVp);

			float3 shadowColor = output.color.rgb * gMaterial.shadowRate;
			output.color.rgb = lerp(output.color.rgb, shadowColor, shadow);
		}
	}

	// α値
	output.color.a = gMaterial.color.a * textureColor.a;
	
	return output;
}
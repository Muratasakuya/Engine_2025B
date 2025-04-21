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

	// uv
	float4 transformUV = mul(float4(input.texcoord, 0.0f, 1.0f), uvTransform);
	// diffuseColor
	float4 diffuseColor = GetDiffuseColor(transformUV, input);
	
	// discardによるpixel棄却
	if (diffuseColor.a < alphaReference) {
		discard;
	}
	
	// 色
	output.color.rgb = color.rgb * diffuseColor.rgb;
	// α値
	output.color.a = color.a * input.vertexColor.a * diffuseColor.a;
	
	//頂点カラー適応
	if (useVertexColor == 1) {

		// rgbのみ
		output.color.rgb *= input.vertexColor.rgb;
	}
	
	// emission処理
	// 発光色
	float3 emission = emissionColor * emissiveIntensity;
	// emissionを加算
	output.color.rgb += emission * diffuseColor.rgb;
	
	return output;
}
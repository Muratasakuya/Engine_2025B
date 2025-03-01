#include "Particle.hlsli"

/*===========================================================
                     Particle PS Shader
===========================================================*/

struct PixelShaderOutput {

	float4 color : SV_TARGET0;
};

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

PixelShaderOutput main(VertexShaderOutput input) {

	float4 textureColor = gTexture.Sample(gSampler, input.texcoord);
	
	// texture‚Ìƒ¿’l‚ª0.5fˆÈ‰º‚ÌŽž‚ÉPixel‚ðŠü‹p
	if (textureColor.a <= 0.5f) {

		discard;
	}

	PixelShaderOutput output;

	output.color.rgb = textureColor.rgb * input.color.rgb;
	output.color.a = textureColor.a * input.color.a;

	return output;
}
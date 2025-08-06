//============================================================================
//	include
//============================================================================

#include "../../Math/Math.hlsli"

//============================================================================
//	CBuffer
//============================================================================

struct CRTMaterial {
	
	float2 resolution; // �o�͉𑜓x
	float2 invResolution;
	
	float barrel; // �c�ȗ�
	float zoom; // �c�ȂŌ����镪�̊g��

	float cornerRadius; // �ۊp���a
	float cornerFeather; // �ۊp�t�F�U�[��
	
	float vignette; // ���B�l�b�g���x
	float aberration; // �F�����̗�
	
	float scanlineIntensity; // ���������x
	float scanlineCount; // �������̖{��
	
	float noise; // �m�C�Y���x
	float time; // �b
};
ConstantBuffer<CRTMaterial> gMaterial : register(b0);

//============================================================================
//	Texture
//============================================================================

RWTexture2D<float4> gOutputTexture : register(u0);
Texture2D<float4> gRenderTexture : register(t0);

SamplerState gSampler : register(s0);

//============================================================================
//	Functions
//============================================================================

// [-1,1]��Ԃł̃o�����ʑ� f(x)=x*(1+k|x|^2)
float2 BarrelDistort(float2 p, float k) {
	
	float r2 = dot(p, p);
	return p * (1.0f + k * r2);
}

// �t�ʑ��i�j���[�g���@�j�ŃT���v��UV���ߎ��擾
float2 InverseBarrelUV(float2 uv, float k, float zoom) {

	float2 p = (uv * 2.0f - 1.0f) / zoom;
	float2 x = p;

	[unroll]
	for (int i = 0; i < 4; ++i) {
		
		float r2 = dot(x, x);
		float a = 1.0 + k * r2;
		float2 fx = x * a - p; // f(x)-p

		// ���R�r�A�� J = a*I + 2k x x^T �̋t�s�����͌v�Z
		float b = 2.0 * k;
		float xx = x.x, yy = x.y;
		float j00 = a + b * yy * yy;
		float j11 = a + b * xx * xx;
		float j01 = -b * xx * yy;
		float det = j00 * j11 - j01 * j01;
		float2 dx = float2(j11 * fx.x - j01 * fx.y, -j01 * fx.x + j00 * fx.y) / det;

		x -= dx;
	}

	return (x * zoom) * 0.5f + 0.5f;
}

// �ۊp�}�X�N
float CornerMask(float2 uv, float cornerRadiusPx, float featherPx) {
	
	float2 res = gMaterial.resolution;
	float2 p = uv * res; // pixel space
	float2 hs = res * 0.5f - cornerRadiusPx; // ���T�C�Y-r
	float2 center = res * 0.5f;
	float d = SdRoundRect(p - center, hs, cornerRadiusPx);
	// d<=0 �������B�O���֌����ăt�F�U�[
	float alpha = 1.0f - SStep(0.0f, featherPx, d);
	return saturate(alpha);
}

// ���B�l�b�g
float VignetteMask(float2 uv, float strength) {
	
	float2 c = uv - 0.5f;
	float r = length(c) * 1.41421356f; // �Ίp���~1
	float v = 1.0f - strength * r * r; // ��
	return saturate(v);
}

// ������
float Scanline(float2 uv, float intensity, float count) {

	if (intensity <= 0.0f) {
		return 1.0f;
	}
	float y = uv.y * count;
	float s = 0.5f + 0.5f * cos(2.0f * PI * y);
	return lerp(1.0f, s, intensity);
}

// �F����
float3 ApplyAberration(float2 uv, float aberr) {
	
	float2 c = uv - 0.5f;
	float2 dir = normalize(c + 1e-6f);
	float2 duv = dir * aberr; // �e�N�Z���P��
	float r = gRenderTexture.SampleLevel(gSampler, uv + duv, 0).r;
	float g = gRenderTexture.SampleLevel(gSampler, uv, 0).g;
	float b = gRenderTexture.SampleLevel(gSampler, uv - duv, 0).b;
	return float3(r, g, b);
}

float3 AddGrain(float3 col, float2 uv, float time, float amount) {
	
	if (amount <= 0.0f) {
		
		return col;
	}
	float n = Hash12(uv * gMaterial.resolution + time * 60.0f);
	return col + (n - 0.5f) * amount;
}

//============================================================================
//	main
//============================================================================
[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	
	// �͈͊O�A�N�Z�X���
	if (DTid.x >= (uint) gMaterial.resolution.x ||
		DTid.y >= (uint) gMaterial.resolution.y) {
		return;
	}

	float2 uv = (float2) DTid.xy / gMaterial.resolution;
	float2 suv = InverseBarrelUV(uv, gMaterial.barrel, gMaterial.zoom);

	// �ۊp + ���B�l�b�g
	float mask = CornerMask(uv, gMaterial.cornerRadius, gMaterial.cornerFeather);
	mask *= VignetteMask(uv, gMaterial.vignette);

	// �͈͊O�̓}�X�N�ŏ���
	float inside = step(0.0f, suv.x) * step(0.0f, suv.y) * step(suv.x, 1.0f) * step(suv.y, 1.0f);
	mask *= inside;

	float3 color;
	if (gMaterial.aberration > 0.0f) {
		
		color = ApplyAberration(suv, gMaterial.aberration * gMaterial.invResolution.x);
	} else {
		
		color = gRenderTexture.SampleLevel(gSampler, suv, 0).rgb;
	}

	// ������
	color *= Scanline(uv, gMaterial.scanlineIntensity, max(gMaterial.scanlineCount, 1.0f));

	// �m�C�Y
	color = AddGrain(color, uv, gMaterial.time, gMaterial.noise);

	// �}�X�N�K�p
	color *= mask;

	gOutputTexture[DTid.xy] = float4(color, 1.0f);
}
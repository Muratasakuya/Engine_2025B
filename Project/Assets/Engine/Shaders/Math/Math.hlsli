//============================================================================
//	Constant
//============================================================================

static const float PI = 3.141592653589793f;
static const float PI2 = PI * 2.0f;

//============================================================================
//	Functions
//============================================================================

// https://gist.github.com/h3r/3a92295517b2bee8a82c1de1456431dc
float Rand4DTo1D(float4 value, float4 dotDir = float4(12.9898f, 78.233f, 37.719f, 17.4265f)) {
	
	float4 smallValue = sin(value);
	float random = dot(smallValue, dotDir);
	random = frac(sin(random) * 143758.5453f);
	return random;
}

float Rand3DTo1D(float3 value, float3 dotDir = float3(12.9898f, 78.233f, 37.719f)) {
	
	float3 smallValue = sin(value);
	float random = dot(smallValue, dotDir);
	random = frac(sin(random) * 143758.5453f);
	return random;
}

float Rand2DTo1D(float2 value, float2 dotDir = float2(12.9898f, 78.233f)) {
	
	float2 smallValue = sin(value);
	float random = dot(smallValue, dotDir);
	random = frac(sin(random) * 143758.5453f);
	return random;
}

float2 Rand3DTo2D(float3 value) {
	
	return float2(
		Rand3DTo1D(value, float3(12.989f, 78.233f, 37.719f)),
		Rand3DTo1D(value, float3(39.346f, 11.135f, 83.155f)));
}

float2 Rand2DTo2D(float2 value) {
	
	return float2(
		Rand2DTo1D(value, float2(12.989f, 78.233f)),
		Rand2DTo1D(value, float2(39.346f, 11.135f)));
}

float3 Rand3DTo3D(float3 value) {
	
	return float3(
		Rand3DTo1D(value, float3(12.989f, 78.233f, 37.719f)),
		Rand3DTo1D(value, float3(39.346f, 11.135f, 83.155f)),
		Rand3DTo1D(value, float3(73.156f, 52.235f, 09.151f)));
}

float SStep(float a, float b, float x) {
	
	return saturate((x - a) / max(b - a, 1e-6f));
}

float SdRoundRect(float2 p, float2 hs, float r) {
	
	float2 q = abs(p) - hs + r;
	return length(max(q, 0.0f)) - r + min(max(q.x, q.y), 0.0f);
}

float Hash12(float2 p) {
	
	float3 p3 = frac(float3(p.xyx) * 0.1031f);
	p3 += dot(p3, p3.yzx + 33.33f);
	return frac((p3.x + p3.y) * p3.z);
}

//============================================================================
//	Classes
//============================================================================

class RandomGenerator {
	
	float3 seed;
	float Generate1D() {
		
		float result = Rand3DTo1D(seed);
		seed.x = result;
		return result;
	}
	float2 Generate2D() {
		
		float2 result = Rand3DTo2D(seed);
		seed.xy = result;
		return result;
	}
	float3 Generate3D() {
		
		seed = Rand3DTo3D(seed);
		return seed;
	}
	
	float Generate(float minValue, float maxValue) {
		
		return lerp(minValue, maxValue, Generate1D());
	}
	float2 Generate(float2 minValue, float2 maxValue) {
		
		return lerp(minValue, maxValue, Generate2D());
	}
	float3 Generate(float3 minValue, float3 maxValue) {
		
		return lerp(minValue, maxValue, Generate3D());
	}
};
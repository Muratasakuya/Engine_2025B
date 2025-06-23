//============================================================================
//	define
//============================================================================
#ifndef DITHER_HLSLI
#define DITHER_HLSLI

//============================================================================
//	buffer
//============================================================================

cbuffer Dither : register(b3) {
	
	int4x4 pattern;
	float maxValue;
	float fadeStart;
	float fadeLength;
}

//============================================================================
//	functions
//============================================================================

void ApplyDistanceDither(float2 pos, float distanceToEye) {

	int2 posIndex = int2(fmod(pos, 4.0f));
	int threshold = pattern[posIndex.y][posIndex.x];
	
	float clipRate = saturate(1.0f - (distanceToEye - fadeStart) / fadeLength);
	
	clip(threshold - maxValue * clipRate);
}

#endif // DITHER_HLSLI
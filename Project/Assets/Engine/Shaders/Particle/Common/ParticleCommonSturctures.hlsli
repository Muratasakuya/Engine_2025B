//============================================================================
//	Structures
//============================================================================

struct Transform {
	
	float3 translation;
	float3 scale;
	float4x4 rotationMatrix;
	float4x4 parentMatrix;
	
	uint billboardMode;
	uint aliveParent;
};

struct Material {
	
	float4 color;
};

struct Particle {
	
	float lifeTime;
	float currentTime;

	float3 velocity;
};

struct PerFrame {
	
	float time;
	float deltaTime;
};

struct PerView {
	
	float3 cameraPos;
	
	float4x4 viewProjection;
	float4x4 billboardMatrix;
};

//============================================================================
//	Functions
//============================================================================

float4x4 MakeWorldMatrix(Transform transform, float4x4 billboardMatrix, float3 cameraPos) {
	
	// scale
	float4x4 scaleMatrix = float4x4(
		transform.scale.x, 0.0f, 0.0f, 0.0f,
		0.0f, transform.scale.y, 0.0f, 0.0f,
		0.0f, 0.0f, transform.scale.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	
	// fullBillboard
	if (transform.billboardMode == 0) {
		
		return mul(scaleMatrix, billboardMatrix);
	}
	
	// yAxisBillboard
	if (transform.billboardMode == 1) {
		
		float3 cameraDirection = normalize(cameraPos - transform.translation);
		float3 forward = normalize(float3(cameraDirection.x, 0.0f, cameraDirection.z));

		float3 right = normalize(cross(float3(0.0f, 1.0f, 0.0f), forward));

		float4x4 yBillboard;
		yBillboard[0] = float4(right, 0.0f);
		yBillboard[1] = float4(float3(0.0f, 1.0f, 0.0f), 0.0f);
		yBillboard[2] = float4(forward, 0.0f);
		yBillboard[3] = float4(0.0f, 0.0f, 0.0f, 1.0f);

		// この時rotationMatrixにはYaw成分を除いた行列を入れる
		float4x4 world = mul(scaleMatrix, mul(yBillboard, transform.rotationMatrix));
		return world;
	}
	
	return mul(scaleMatrix, transform.rotationMatrix);
}

float4x4 SetParent(Transform transform, float4x4 worldMatrix) {
	
	// 親の設定がいらない場合はそのまま返す
	if (transform.aliveParent == 0) {
		
		return worldMatrix;
	}
	
	 // 親の平行移動成分
	float3 parentPos = transform.parentMatrix[3].xyz;
	worldMatrix[3].xyz += parentPos;

	return worldMatrix;
}